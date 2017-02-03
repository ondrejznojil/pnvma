#!/usr/bin/env python

"""
Post processing script for generated Squeak plugin source files.
It is a state machine operating on whole lines.
"""

import os
import sys
import re
import argparse

SPACES_FOR_TAB = 4

#******************************************************************************
# Regular expressions
#******************************************************************************
# header files to remove
RE_HEADERS_REMOVE = re.compile(
    r'^#include\s+[<"](?:time|sq.*)\.h[>"]|^#pragma')
RE_MACROS_REMOVE = re.compile(
    r'^#define\s+(?:true|false|null|'
    r'NetTemplateCodeSize.*|'
    r'EXPORT\(.*|'
    r'(?:TokenType(?:Integer|NetRef|Pointer))|'
    r'(?:TypeSign[a-zA-Z]+))')
RE_ENUM = re.compile(r'^#define\s+(?P<valuename>(?P<name>Error)[a-zA-Z]+)\s+'
    r'(?P<number>\d+)')
# functions to remove
RE_FUNC_REMOVE = re.compile(r'^(?:EXPORT\(sqInt\)\s*setInterpreter|'
    #r'(?:static\s+)?void\s+dereference|'
    #r'(?:static\s+)?void\s*\*\s*\*\s*makePointer|'
    r'EXPORT\(.*\)\s*getModuleName\b|'
    r'(?:static\s+)?(?:int|sqInt)\s+halt'
    r')\s*\([^;{]*([{;])\s*$')
# variables to remove
RE_VARS_REMOVE = re.compile(r'static\s+char\s+__buildInfo.*;\s*|'
    r'static\s+char\s+(?:output|input)Pins[^;]+;|'
    r'struct\s+VirtualMachine\s*\*.*;')

RE_LOG_MESSAGE = re.compile(r'^(?P<indent>\s*)'
        r'categorylevelmessage\(LoggingEvent,\s*"(?P<category>[^"]+)",'
        r'\s*"(?P<level>[^"]+)",\s*(?P<message>.*?)\)\s*;\s*$',
        re.MULTILINE | re.DOTALL)

# module name regexps
RE_DECL_MODULE_NAME = re.compile(r'^static\s+const\s+char\s*\*\s*moduleName')
RE_MODULE_NAME = re.compile(r'"(?P<name>[a-zA-Z0-9]+)\s+'
        r'(?P<date>(\s*[a-zA-Z0-9]+){3})\s+\(.*\)"')

# remove everything in this #ifdef block
RE_BUILT_IN = re.compile(r'^#ifdef\s+SQUEAK_BUILTIN_PLUGIN')
RE_ENDIF = re.compile('^#endif')

# debugging statements
RE_DEBUG = re.compile(r'^\s*(?:debug)\(.*\)\s*;\s*$')
RE_ASSERT = re.compile(r'(^\s*)(assert\((.*)\);)\s*$')

# comments
RE_ONE_LINE_COMMENT = re.compile(r'^\s*/\*.*\*/\s*$')
RE_COMMENT_START = re.compile(r'^\s*/\*')

# post-processing substitutions, contains pairs: (pattern, replacement)
GLOBAL_REPLACE = (
    (re.compile(r'\busqInt\b'), 'unsigned'),
    (re.compile(r'\bsqInt\b'), 'int'),
    # replace all calls to PNVM* objects with simple function calls
    (re.compile(r'(\bpnvm\w*New\w*\()PNVM\w+(?:\s*,\s*)?'), r'\1'),
    (re.compile(r'\bnull\b'), r'NULL'),
    # remove '*' operator in incementing statements
    # for pointers and iterators to prevent "unused-value" warnings
    (re.compile(r'^(\s*)\*([a-zA-Z]\w*\+\+;)$'), r'\1\2'),
    (re.compile(r'\bTokenTypeNetRef\b'), 'TOKEN_TYPE_NET_REF'),
    (re.compile(r'\bTokenType(Integer|Pointer)\b'),
        lambda matchobj: 'TOKEN_TYPE_'+matchobj.group(1).upper()),
    (re.compile(r'\bTypeSignNetInst\b'), 'SIGN_NET_INSTANCE'),
    (re.compile(r'\bTypeSign([a-zA-Z]+)\b'),
        lambda matchobj: 'SIGN_'+matchobj.group(1).upper()),
    (re.compile(r'^EXPORT\(([^)]+)\)'), r'\1')
)

# check for functions for export to header file
RE_FUNC_EXPORT = re.compile(r'^EXPORT\((.*)\)(\s*[a-zA-Z][\w_]*\s*\(.*\);)')
RE_HEADER = re.compile(r'^#include\s+')

# log message parsing
# match both function call and cast; cast ends with double ')'
RE_FUNC_START = re.compile(r'\s*([a-zA-Z_]+|\(\([a-zA-Z]+\s*\*?\s*\))\s*\(')
RE_SIMPLE_TOKEN = re.compile(r'\s*(?:[a-zA-Z_0-9]+(?:\s*\[[^]]+\])?'
    r'|[-+]?[0-9.]+)')
RE_PARAM_SEP = re.compile(r'\s*,\s*')

# *****************************************************************************
# global variables
CURRENT_ENUM = None # name of enumeration being currently parsed without
                    # "_t" suffix, which will be appended at the end of
                    # typedef

#******************************************************************************
# Utilities
#******************************************************************************
def tabs_to_spaces(line):
    """
    Replace tabulators at line start with spaces.

    :rtype: (``str``) Modified line.
    """
    i = 0
    while i < len(line) and line[i] == '\t':
        i += 1
    return (" "*(SPACES_FOR_TAB*i)) + line[i:]

#******************************************************************************
# Line post-processing functions
#******************************************************************************
def line_postprocess(line, replace_tabs=True, do_replacements=True,
        **_kwargs):
    """
    Generic line postprocessor.

    :param replace_tabs: (``bool``) Whether to replace tabulators with spaces.
    :param do_replacements: (``bool``) Whether to apply replacements in
        GLOBAL_REPLACE array to line.
    :rtype: (``str``) Modified line.
    """
    if replace_tabs:
        line = tabs_to_spaces(line)
    if do_replacements:
        for reg, replacement in GLOBAL_REPLACE:
            line = reg.sub(replacement, line)
    return line

def assert_postprocess(line, match, **_kwargs):
    """
    Line postprocessor called on assert statement.
    """
    if 'interpreterProxy' in match.group(3):
        line = '%s/* %s */\n' % (match.group(1), match.group(2))
    return line_postprocess(line)

def parse_string(msg, index):
    """
    Parse string inside of logging statement.

    :param msg: (``str``) Whole logging line.
    :param index: (``int``) Position in string pointing at the first
        character we should try to match.

    Return parsed string.
    """
    re_string_start = re.compile(r'\s*"')

    match = re_string_start.match(msg, index)
    if match:
        operand = '"'
        index = match.end()
        while msg[index] != '"':
            while msg[index] not in '\\"':
                operand += msg[index]
                index += 1
            if msg[index] == '\\':
                operand += '\\'
                index += 1
                operand += msg[index]
                index += 1
        operand += '"'
        index += 1
        return operand

def parse_function(msg, index):
    """
    Parse functions inside of log message. This is applied recursively on
    nested function calls.

    :param msg: (``str``) Whole logging line.
    :param index: (``int``) Index of the first character of function name or
        white space just before it.

    Return parsed function with exact copy of all matched characters.
    """
    re_func_end = re.compile(r'\s*\)')

    match = RE_FUNC_START.match(msg, index)
    if not match:
        return
    res = match.group(0)
    index = match.end()
    is_cast = match.group(1).startswith('(')
    while index > 0 and index < len(msg) and \
            not re_func_end.match(msg, index):
        match = RE_FUNC_START.match(msg, index)
        if match:
            nested = parse_function(msg, index)
            if not nested:
                return
            res += nested
            index += len(nested)
        else:
            str_res = parse_string(msg, index)
            if str_res:
                res += str_res
                index += len(str_res)
            else: 
                match = RE_SIMPLE_TOKEN.match(msg, index)
                if match:
                    res += match.group(0)
                    index = match.end()
                else:
                    return
        match = RE_PARAM_SEP.match(msg, index)
        if match:
            res += match.group(0)
            index = match.end()
        elif not re_func_end.match(msg, index):
            return
    match = re_func_end.match(msg, index)
    res += match.group(0)
    index += 1
    if is_cast:
        match = re_func_end.match(msg, index)
        res += match.group(0)
        index += 1
    return res

def parse_log_operand(msg, index, messages):
    """
    Parse log message operand. It may be string, simple value (possibly with
    type operator) or function call.

    :param msg: (``str``) Whole message line.
    :param index: (``int``) Index of first character in msg, where we should
        start parsing.
    :param messages: (``list``) List of strings, that will form the logging
        message for ``log4cxx`` macro. Here the parsed operand will be appended.

    Return index to ``msg`` just after the last character of operand.
    """
    re_ternary_op = re.compile(r'\s*,\s*\([^?()]+\?[^:()]+:[^()]+\)',
            re.MULTILINE)
    re_concat = re.compile(r'\s*,\s*\(')
    re_concat_end = re.compile(r'\s*\)')

    match = re_ternary_op.match(msg, index)
    if match:
        messages.append(match.group(0))
        return match.end()
    match = re_concat.match(msg, index)
    if match:
        index = parse_log_operand(msg, match.end(), messages)
        if index < 0:
            return index
        match = RE_PARAM_SEP.match(msg, index)
        if not match:
            return -1
        index = parse_log_operand(msg, match.end(), messages)
        if index < 0:
            return index
        match = re_concat_end.match(msg, index)
        if not match:
            return -1
        return match.end()
    res = parse_string(msg, index)
    if res:
        messages.append(res)
        return index + len(res)
    match = RE_FUNC_START.match(msg, index)
    if match:
        res = parse_function(msg, index)
        if not res:
            return -1
        messages.append(res)
        return index + len(res)
    match = RE_SIMPLE_TOKEN.match(msg, index)
    if match:
        messages.append(match.group(0))
        return match.end()
    return -1

def parse_log_message(message):
    """
    Process recursive application of ',' operator on message parts.
    Concatenation operator in slang is translated to ',' applied to all
    operands in infix form. For example:

      LoggingEvent category: cat level: error
        message: 'part1 ',value,', end_part'.

    Will be generated as:
      categorylevelmessage(LoggingEvent, cat, error,
        ,(,("part1", value), "end_part"))

    Translate them into application of "<<" operators in infix form.
    """
    messages = []
    res = parse_log_operand(message, 0, messages)
    if res < 0 or re.match(r'\s*;', message[res:]):
        return
    return " << ".join(messages)

def log_postprocess(line, match, **_kwargs):
    """
    Line postprocessor called on logging statement.
    """
    try:
        logger = {
                "pnvmExecuting" : "exlog",
                "pnvmInOut"     : "iolog",
                "pnvmParsing"   : "parselog",
            }[match.group('category')]
    except KeyError:
        logger = "exlog"
    res = parse_log_message(match.group('message'))
    if not res:
        # failed to parse, let's comment it out
        line = '%s/* %s */\n' % (match.group('indent'),
                line[match.end('indent'):])
        return line_postprocess(line)
    line = "%(indent)sLOG4CXX_%(level)s(%(logger)s, %(message)s)\n" % {
                "indent"  : match.group("indent"),
                "level"   : match.group("level").upper(),
                "logger"  : logger,
                "message" : res
            }
    return line

def start_enum(line, match, header, **kwargs):
    """
    Handle first line of enumeration defines. Write enumeration typedef.
    """
    global CURRENT_ENUM
    if match.group('name') == 'Error':
        enum_name = match.group('name')
    else:
        raise ValueError("unknown enumeration: %s" % match.group('name'))
    header.write('typedef enum %s {\n' % match.group('name'))
    CURRENT_ENUM = enum_name
    return handle_enum(line, header, **kwargs)[1]

#******************************************************************************
# Post processor states
#******************************************************************************
# this state machine operates on whole lines
( STATE_CODE        # initial state
, STATE_COMMENT     # in multiline comment
, STATE_FUNC_REMOVE # in body of function, that will be removed
, STATE_MODULE_NAME # in preprocessor block defining moduleName
, STATE_BUILT_IN    # in SQUEAK_BUILDIN_PLUGIN #ifdef block
, STATE_ENUM        # we are parsing sequence of defines, that will be made
                    #  into enumerations
) = range(6)

# flags for each state; particular flag says, whether to print the line
# if not state transition passed
STATE_PRINT_DEFAULTS = [True, True, False, False, False, False]
# particular flag says, whether to check, if the line should be exported to
# header file
STATE_HEADER_EXPORT = [True, False, False, False, False, False]

# *****************************************************************************
# Guard functions
# *****************************************************************************
def guard_func_remove_decl(line):
    """
    Check, whether function to remove is a declaration or definition.
    This passes for the former case.
    """
    match = RE_FUNC_REMOVE.match(line)
    return match and match.group(1) == ';'

# *****************************************************************************
# State handlers - each return a tuple (next_state, line)
#   where
#     line is either modified line that will be postprocessed and written
#       to output or None
# *****************************************************************************
def handle_module_name(line, **_kwargs):
    """
    This is a transition handler for MODULE_NAME state.
    """
    match = RE_MODULE_NAME.search(line)
    if getattr(handle_module_name, 'module_name', None) is None and match:
        handle_module_name.module_name = \
                '/* static const char * moduleName = "%s %s" */' % (
                    match.group('name'), match.group('date'))
    if RE_ENDIF.match(line):
        return (STATE_CODE, (' '*8) + getattr(handle_module_name,
            "module_name", "FAILED TO PARSE MODULE NAME"))
    return (STATE_MODULE_NAME, None)

def handle_enum(line, header, **_kwargs):
    """
    This is a transition handler for ENUM state.
    """
    global CURRENT_ENUM
    match = RE_ENUM.search(line)
    if match:
        header.write('    %s = %s,\n' % (
            match.group('valuename'), match.group('number')))
        return (STATE_ENUM, None)
    else:
        header.write('} %s_t;\n\n' % CURRENT_ENUM)
        CURRENT_ENUM = None
        return (STATE_CODE, False)  # handle the same line again

# This defines transitions of parser, it's an array indexed by current state.
# Each state has multiple transitions, they are composed of:
#   action := callable | guard_tuples
#   
#   callable    - a function that will handle the current state (state handler)
#
#   guard_tuples := [(guard, next_state, print_line), ...]
#
#   guard       - regular expression that must match in order to execute
#                 transition or callable
#   next_state  - next state of parser if guard passes
#   print_line  - boolean saying, whether to print matching line
#                 or callable, which will process the line
STATE_TRANSITIONS = (
    # code
    ( (RE_ONE_LINE_COMMENT       , STATE_CODE        , True)
    , (RE_COMMENT_START          , STATE_COMMENT     , True)
    , (RE_BUILT_IN               , STATE_BUILT_IN    , False)
    , (guard_func_remove_decl    , STATE_CODE        , False)
    , (RE_FUNC_REMOVE            , STATE_FUNC_REMOVE , False)
    , (RE_DEBUG                  , STATE_CODE        , False)
    , (RE_LOG_MESSAGE            , STATE_CODE        , log_postprocess)
    , (RE_DECL_MODULE_NAME       , STATE_MODULE_NAME , False)
    , (RE_ENUM                   , STATE_ENUM        , start_enum)
    , (RE_HEADERS_REMOVE         , STATE_CODE        , False)
    , (RE_MACROS_REMOVE          , STATE_CODE        , False)
    , (RE_VARS_REMOVE            , STATE_CODE        , False)
    , (RE_ASSERT                 , STATE_CODE        , assert_postprocess) 
    ),
    # comment
    ( (re.compile(r'^.*\*/\s*$') , STATE_CODE        , True), ), 
    # function to remove
    ( (re.compile(r'^}$')        , STATE_CODE        , False), ), 
    # module name
    handle_module_name,
    # built in block
    ( (RE_ENDIF                  , STATE_CODE        , False), ),
    handle_enum,
)
def parse_args():
    """
    Parse script arguments.
    """
    parser = argparse.ArgumentParser(
            description="Clean squeak plugin source code.")
    parser.add_argument('in_file', type=argparse.FileType('r'),
            default=sys.stdin,
            help="Squeak plugin source code file.")
    parser.add_argument('out_file', type=argparse.FileType('w'),
            default=sys.stdout,
            help="Cleaned version of source code file.")
    parser.add_argument('--header-file', type=argparse.FileType('w'),
            default=None,
            help="Generate header file from input file with this name.")
    parser.add_argument('-g', '--generate-header', action='store_true',
            help="Generate header. If HEADER_FILE option is not given,"
            " it will be named ${in_file}.h.")
    args = parser.parse_args()
    if args.generate_header and args.header_file is None:
        if not args.in_file.name:
            parser.error("File name of header file must be given, when reading"
                    " from stdin.")
        args.header_file = open(
                os.path.splitext(args.in_file.name)[0] + '.h', "w")
    return args

def process_transition(line, transition, **kwargs):
    """
    Try to trigger single transition on single line.
    
    :rtype: (``tuple``) With (next_state, processed_line).

    In case processed_line is a:
      string - it will be written as is
      None   - nothing will be printed
      False  - the same line will be handled again for returned state
    """
    guard, next_state, print_line = transition
    satisfied = False
    match = None
    if callable(guard):
        satisfied = guard(line)
    else:
        match = guard.match(line)
        satisfied = match is not None
    if satisfied:
        if print_line:
            if callable(print_line):
                line = print_line(line, match=match, **kwargs)
            else:
                line = line_postprocess(line, match=match, **kwargs)
        else:
            line = None
        return (next_state, line)

def make_header_include_guard(filename):
    """
    Call this before writing any input to header file. These preprocessor
    directives ensure, that header is included only once.
    """
    return "_".join(filename.split('.')).upper()

def make_header_start(header_file):
    """
    Write preamble of header file.
    """
    name = os.path.basename(header_file.name)
    header_file.write((
            "#ifndef %(filename)s\n"
            "#define %(filename)s\n\n") % {
                "filename" : make_header_include_guard(name) })

def make_header_foot(header_file):
    """
    Write foot of header file.
    """
    name = os.path.basename(header_file.name)
    header_file.write("\n#endif /* %s */\n" % make_header_include_guard(name))

def export_to_header(line):
    """
    Check, whether this line should be exported in header file.
    """
    match = RE_FUNC_EXPORT.match(line)
    if match and not RE_FUNC_REMOVE.match(line):
        return "\n" + match.group(1) + match.group(2) + "\n"
    if RE_HEADER.match(line) and not RE_HEADERS_REMOVE.match(line):
        return line

def write_c_preamble(ofile, header_file=None):
    """
    Include additional headers missing in squeak plugin.
    """
    if header_file is not None:
        ofile.write('#include "%s"\n' % os.path.basename(header_file.name))
    ofile.write(
        "#ifndef TARGET_ARDUINO\n"
        "    #include <ctype.h>\n"
        "#endif\n"
        "#ifndef PLATFORM_CODE\n"
        "    #define PLATFORM_CODE {0}\n"
        "#endif\n\n")

def process_file(ifile, ofile, header_file=None):
    """
    Main loop of text processor.
    It's a state machine processing whole lines as tokens.
    """
    if header_file is not None:
        make_header_start(header_file)
    write_c_preamble(ofile, header_file)
    state = STATE_CODE
    for line in ifile.readlines():
        processed_line = False
        while processed_line is False:
            next_state = state
            if header_file is not None and STATE_HEADER_EXPORT[state]:
                export = export_to_header(line)
                if export:
                    processed_line = export
                export = export is not None
            else:
                export = None
            transitions = STATE_TRANSITIONS[state]
            if callable(transitions):
                (next_state, processed_line) = transitions(
                        line, header=header_file)
            else:
                for trans in transitions:
                    # try to fire transitions until one passes
                    result = process_transition(line, trans, header=header_file)
                    if result is not None:
                        (next_state, processed_line) = result
                        break
                if result is None:
                    if not STATE_PRINT_DEFAULTS[state]:
                        processed_line = None
                    else:
                        processed_line = line_postprocess(line)
            state = next_state
            if processed_line is not None and processed_line is not False:
                if export:
                    header_file.write(processed_line)
                else:
                    # no need to include lines in written to header file
                    ofile.write(processed_line)
    if header_file is not None:
        make_header_foot(header_file)

def main():
    """
    Entry point of script execution.
    """
    args = parse_args()
    process_file(args.in_file, args.out_file, args.header_file)

if __name__ == '__main__':
    main()
