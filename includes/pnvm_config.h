/******************************************************************************
 * Configuration file with typedefs, constants and enumerations.
 *
 * Important macros:
 *   * BLOCK_SIZE      - changes size of value blocks; default is
 *                       derermined from the pointer size
 *   * MAX_BLOCK_COUNT - applies only when memblock_lib is used;
 *                     - it specifies maximum number of blocks the heap
 *                       can hold
 *   * DEBUG           - turns on debugging output - for x86
 *
 * @author Michal Minar <xminar06@stud.fit.vutbr.cz>
 * @year 2013
 */
#ifndef PNVM_CONFIG_H
#define PNVM_CONFIG_H

// this needs to be set for C++ to include limits from stdint.h
#ifndef __STDC_LIMIT_MACROS
    #define __STDC_LIMIT_MACROS
#endif
#ifndef __STDC_CONSTANT_MACROS
    #define __STDC_CONSTANT_MACROS
#endif

#include <stdint.h>
#if TARGET_ARDUINO
    #include <Arduino.h>
    #define assert(...)
#else
    #include <cstdlib>
    #include <assert.h>
#endif
/******************************************************************************
 * Interface to main simulator program.
 * It should define functions:
 *   unsigned long millis();
 *   void digitalWrite(int pin, int value);
 *   int digitalRead(int pin);
 */
#include "pnvm_main.h"

#ifndef BLOCK_SIZE
  #if !defined UINTPTR_MAX || !defined UINT16_MAX
    #error "missing limit macros"
  #endif
  #if UINTPTR_MAX <= UINT16_MAX
    #define BLOCK_SIZE 16
  #elif UINTPTR_MAX <= UINT32_MAX
    #define BLOCK_SIZE 32
  #else
    #define BLOCK_SIZE 64
  #endif
#endif

#ifndef MAX_BLOCK_COUNT
  #if UINTPTR_MAX <= UINT16_MAX
    #define MAX_BLOCK_COUNT 30
  #else
    #define MAX_BLOCK_COUNT 1024
  #endif
#endif

#ifndef HIGH
  #define HIGH 1
  #define LOW  0
#endif

unsigned int const AllocationBlockSize = BLOCK_SIZE;
unsigned int const PointerSize = sizeof(void *);
#ifdef USE_MEMBLOCK
unsigned int const TotalAllocationBlocks = MAX_BLOCK_COUNT;
#endif

#if BLOCK_SIZE <= 16
    typedef uint16_t RefCount_t;
    typedef uint16_t ItemCount_t;
    typedef int16_t CodeOffset_t;
    typedef uint16_t CodeLength_t;
#else 
    typedef unsigned int RefCount_t;
    typedef unsigned int ItemCount_t;
    typedef int CodeOffset_t;
    typedef unsigned int CodeLength_t;
#endif
typedef unsigned char TemplateId_t;
typedef uint32_t Time_t;
typedef uintptr_t TokenValue_t;
typedef intptr_t IntegerValue_t;

// value type signatures
typedef enum Signature {
    SIGN_STRING       = 0,
#ifndef TARGET_ARDUINO
    SIGN_TUPLE        = 1,
#else
    SIGN_TUPLE        = 2,
#endif
    SIGN_ARRAY        = 2,
    SIGN_PLACE        = 3,
    SIGN_NET_INSTANCE = 4,
    SIGN_TEMPLATE     = 5,
    SIGN_EVENT        = 6
}Signature_t;

#if BLOCK_SIZE <= 16
    #define SignatureBitSize 4
    #define RefCountBitSize 10
    #define ItemCountBitSize \
                  sizeof(char)*8*3 - (SignatureBitSize) - (RefCountBitSize)
    #define NetTemplateCodeSize 1900
#else
    #define SignatureBitSize sizeof(Signature_t)*8
    #define RefCountBitSize sizeof(RefCount_t)*8
    #define ItemCountBitSize sizeof(ItemCount_t)*8
    #define NetTemplateCodeSize 4096
#endif


typedef enum TokenType {
    TOKEN_TYPE_INTEGER = 0,
    TOKEN_TYPE_NET_REF = 2,
    TOKEN_TYPE_POINTER = 3
}TokenType_t;

extern char const type_indexable_flags[];
extern char const type_collectable_flags[];
extern char const * const signature_names[];

#endif /* end of include guard: PNVM_CONFIG_H */
