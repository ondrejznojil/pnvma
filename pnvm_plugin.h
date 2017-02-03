#ifndef PNVM_PLUGIN_H
#define PNVM_PLUGIN_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pnvm_types.h"
#include "pnvm_utils.h"
typedef enum Error {
    ErrorCalendarEmpty = 54,
    ErrorCollectionFull = 3,
    ErrorExecutionFirst = 50,
    ErrorExecutionLast = 58,
    ErrorExecutionUnhandledOperator = 55,
    ErrorExecutionUnhandledSignature = 56,
    ErrorExecutionUnhandledTransitionElement = 57,
    ErrorExecutionUnspecified = 50,
    ErrorGenericLast = 4,
    ErrorIOFirst = 100,
    ErrorIOLast = 102,
    ErrorInitFailed = 58,
    ErrorNoMemory = 1,
    ErrorNoSuchPlace = 37,
    ErrorNoSuchTemplate = 53,
    ErrorNoSuchUplink = 52,
    ErrorOutOfRange = 2,
    ErrorOutputBufferEmpty = 101,
    ErrorParsingCollection = 32,
    ErrorParsingExpression = 29,
    ErrorParsingFirst = 20,
    ErrorParsingInit = 24,
    ErrorParsingLast = 37,
    ErrorParsingNames = 22,
    ErrorParsingNetInst = 34,
    ErrorParsingNetInstValue = 36,
    ErrorParsingPlace = 35,
    ErrorParsingString = 33,
    ErrorParsingSymbol = 30,
    ErrorParsingTemplate = 20,
    ErrorParsingTemplateSymbols = 21,
    ErrorParsingToken = 31,
    ErrorParsingTransition = 27,
    ErrorParsingTransitionCode = 28,
    ErrorParsingTransitions = 25,
    ErrorParsingUplink = 26,
    ErrorParsingUplinks = 23,
    ErrorSerializationFailed = 102,
    ErrorSuccess = 0,
    ErrorTemplatesDatabaseFull = 51,
    ErrorUnspecified = 4,
} Error_t;

bool canAllocateBlock(void);
PNVMStringHead * cstrToString(char const *cstr);
bool failed(void);
Error_t getError(void);
void initializeModule(void);
void initializeOMModule(void);
bool inputString(PNVMStringHead *message);
PNVMEvent * primitiveCalFront(void);
bool primitiveCalIsEmpty(void);
void primitiveCleanup(void);
void primitiveCleanupModule(void);
char * primitiveCodeIteratorAt(int pos);
CodeLength_t primitiveCodeSize(void);
Time_t primitiveCurrentTime(void);
bool primitiveFailed(void);
bool primitiveInputCStr(char const *msg);
bool primitiveInputString(PNVMStringHead *msg);
PNVMNetInstHead * primitiveInstantiateTemplate(int tmplId);
int primitiveLoadNetTemplate(char const *msg);
Error_t primitiveMakePlatformTemplateplaceCountplacessymbolscodeLengthtransitions(char const *name, int plcCount, char const * const *places, char const *symbols, CodeLength_t codeLength, CodeLength_t transitions);
PNVMTemplate * primitiveNetTemplateAt(int index);
int primitiveNothingChanged(void);
int primitiveNumberOfNetTemplates(void);
bool primitiveOutputBufferEmpty(void);
char * primitiveOutputPopCStr(void);
PNVMStringHead * primitiveOutputPopMessage(void);
PNVMTemplate * primitiveParseNetTemplate(PNVMStringHead *strBlock);
PNVMStringHead * primitiveRenderAsString(PNVMStringHead const *value);
int primitiveSerializeTokento(PNVMToken const & token, PNVMStringIter & o);
int primitiveSerializeValueto(PNVMHeaderBlock const *value, PNVMStringIter & o);
int primitiveStep(void);
int primitiveStepsExecuted(void);
char * primitiveStringToCStr(PNVMStringHead const *str);
Error_t setError(Error_t err);
char const * strError(Error_t err);
void unmanageNetInstance(PNVMNetInstHead *inst);

#endif /* PNVM_PLUGIN_H */
