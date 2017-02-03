/*!****************************************************************************
 * Utility, that takes a definition on net template written in PNAL
 * and spitting out a header file for c simulator program with defines to be
 * used directly as an objects of object memory.
 *
 * This saves a lot of initialization time on simulator startup and especially
 * it saves a lot of RAM on embedded systems.
 *
 * Output header file will contain these defines:
 *   * PLATFORM_NAME        - a string with net template name
 *   * PLATFORM_PLACE_COUNT - integer
 *   * PLATFORM_PLACES      - array of strings with place names
 *   * PLATFORM_SYMBOLS     - serialized tuple of net template symbols
 *   * PLATFROM_CODE_LENGTH - length of net template code
 *                          - or a number of bytes required in netTemplateCode
 *                            array
 *   * PLATFORM_TRANSITIONS - offset to net template code to the first
 *                            opening bracket of first simple transition
 *                            (not an uplink, not an init)
 *   * PLATFORM_CODE        - array of bytes to store in netTemplateCode
 *
 * PLATFORM_PLACES are supposed to be used like this:
 *   char const * const places[] = PLATFORM_PLACES;
 *   //... make a Tuple with Strings out of it and store them in Template
 *   // object
 * 
 * PLATFORM_CODE can be used directly in generated plugin:
 *   static char netTemplateCode[NetTemplateCodeSize] = PLATFORM_CODE;
 *
 * @author Michal Minar <xminar06@stud.fit.vutbr.cz>
 * @year 2013
 */
#include "pnvm_types.h"
#include "pnvm_plugin.h"
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <boost/program_options.hpp>
#include <log4cxx/layout.h>
#include <log4cxx/level.h>
#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::string;

namespace po = boost::program_options;

// These are expected by squeak plugin.
log4cxx::LoggerPtr memlog = log4cxx::Logger::getLogger("pnvm.memory");
log4cxx::LoggerPtr exlog  = log4cxx::Logger::getLogger("pnvm.executing");
log4cxx::LoggerPtr iolog  = log4cxx::Logger::getLogger("pnvm.io");
log4cxx::LoggerPtr parselog  = log4cxx::Logger::getLogger("pnvm.parsing");

namespace {

    //! Main logger - just for this module.
    log4cxx::LoggerPtr mlog(log4cxx::Logger::getLogger("pnvm"));

    /*!
     * Template instance is all we need to generate the header.
     */
    void print_template(PNVMTemplate *tmpl, std::ostream &output) {
        char *cstr;
        output << "#ifndef PNVM_PLATFORM_TEMPLATE" << endl
               << "#define PNVM_PLATFORM_TEMPLATE" << endl << endl;
        cstr = primitiveStringToCStr(pnvmTemplateName(tmpl));
        output << "#define PLATFORM_NAME \"" << cstr << '"' << endl;
        free(cstr);
        output << "#define PLATFORM_PLACE_COUNT "
               << pnvmTemplatePlaceCount(tmpl) << endl
               << "#define PLATFORM_PLACES {";
        PNVMTupleIter ti = pnvmValueIterator(pnvmTemplatePlaces(tmpl));
        while (!pnvmIterAtEnd(ti)) {
            if (pnvmIterIndex(ti) > 0) output << ", ";
            cstr = primitiveStringToCStr(
                    (PNVMStringHead *) pnvmTokenPointer(pnvmIterNext(ti)));
            output << '"' << cstr << '"';
            free(cstr);
        }
        output << '}' << endl
               << "#define PLATFORM_SYMBOLS ";
        PNVMStringHead *tmpstr = pnvmStringNew();
        PNVMStringIter striter = pnvmValueIterator(tmpstr);
        primitiveSerializeValueto(pnvmTemplateSymbols(tmpl), striter);
        PNVMStringHead *rendered = primitiveRenderAsString(tmpstr);
        pnvmValueDelete(tmpstr);
        striter = pnvmValueIterator(rendered);
        while (!pnvmIterAtEnd(striter)) {
            output << pnvmIterNext(striter);
        }
        pnvmValueDelete(rendered);

        output << endl
               << "#define PLATFORM_CODE_LENGTH "
               << pnvmTemplateCodeLength(tmpl) << endl
               << "#define PLATFORM_TRANSITIONS "
               << pnvmTemplateTransitions(tmpl) << endl << endl
               << "#define PLATFORM_CODE \\" << endl
               << "    {";
        cstr = primitiveCodeIteratorAt(0);
        for (CodeLength_t i=0; i < pnvmTemplateCodeLength(tmpl); ++i) {
            if (i > 0) output << ',';
            output << std::setw(4) << (int) *(cstr++);
            if (i > 0 && !((i + 1) % 10)) output << " \\" << endl << "    ";
        }
        output << '}' << endl << endl
               << "#endif // PNVM_PLATFORM_TEMPLATE" << endl;
    }

    /*!
     * Take any input stream with valid pnal code, make Template instance
     * out of it, generate the header contents and stream it to output.
     */
    void process_file(std::istream &input, std::ostream &output) {
        PNVMStringHead *tmp = pnvmStringNew();
        PNVMStringIter si = pnvmValueIterator(tmp);
        char c;
        while (input.good()) {
            input >> c;
            if (!isspace(c)) {
                pnvmIterWrite(si, c);
            }
        }
        PNVMTemplate *tmpl = primitiveParseNetTemplate(tmp);
        print_template(tmpl, output);
        pnvmValueDelete(pnvmTemplateName(tmpl));
        pnvmValueDelete(pnvmTemplatePlaces(tmpl));
        pnvmValueDelete(pnvmTemplateSymbols(tmpl));
        pnvmValueDelete(tmpl);
    }

}

/******************************************************************************
 * Main
 */
int main(int argc, char **argv) {
    string input_file;
    string output_file;
    std::ifstream ifs;
    std::istream * is = NULL;
    std::ofstream ofs;
    std::ostream *os = NULL;

    log4cxx::BasicConfigurator::configure();
    log4cxx::Logger::getRootLogger()->setLevel(log4cxx::Level::getError());

    po::options_description cmdline_options;
    cmdline_options.add_options()
        ( "help,h", "Print help message." )
        ( "output-file,o", po::value<string>(&output_file),
          "File path, where the platform header file will be stored."
          " Defaults to stdout." )
        ( "input-file,i", po::value<string>(&input_file),
          "Path to file containing net template code in PNAL."
          " It defaults to stdin." );

    // Parse options
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, cmdline_options), vm);
    po::notify(vm);

    if (vm.count("help")) {
        cout << "Usage: " << argv[0] << " [options]" << endl
             << cmdline_options << endl;
        return 0;
    }

    if (vm.count("input-file")) {
        ifs.open(input_file.c_str());
        if (ifs.bad()) {
            LOG4CXX_FATAL(mlog, "can not open input file \""
                 << input_file << "\"")
            return 1;
        }
        LOG4CXX_DEBUG(mlog, "opened input from file \""
                << input_file << "\"")
        is = &ifs;
    }else {
        is = &cin;
        LOG4CXX_DEBUG(mlog, "reading input from stdin")
    }

    if (vm.count("output-file")) {
        ofs.open(output_file.c_str());
        if (ofs.bad()) {
            LOG4CXX_FATAL(mlog, "can not open output file \""
                 << output_file << "\"")
            return 1;
        }
        LOG4CXX_DEBUG(mlog, "opened output from file \""
                << output_file << "\"")
        os = &ofs;
    }else {
        os = &cout;
        LOG4CXX_DEBUG(mlog, "writing output to stdout")
    }

    process_file(*is, *os);

}

