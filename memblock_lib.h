// This file has been prepared for Doxygen automatic documentation generation.
/*! \file *********************************************************************
 *
 * \brief  Memory block manager library header file.
 *
 *      The memory block library provides functions for dynamic memory
 *      allocation using a fragmentation-free memory manager. Upon
 *      initialization, the memory manager gets a large block of memory and
 *      divides it into a number of bags, which in turn are divided into a
 *      number of equally sized memory blocks.
 *
 *      The number of bags and the block size and block count for each bag can
 *      be fine-tuned to match a specific application's memory requirements. In
 *      order to help the fine-tuning process, the memory manager maintains
 *      statistics on memory use for each bag, with minimum and maximum block
 *      size requests and peak and average utilization.
 *
 * \par Application note:
 *      AVR482: DB101 Software
 *
 * \par Documentation
 *      For comprehensive code documentation, supported compilers, compiler
 *      settings and supported devices see readme.html
 *
 * \author
 *      Atmel Corporation: http://www.atmel.com \n
 *      Support email: avr@atmel.com
 *
 * $Revision: 2395 $
 * $URL: http://revisor.norway.atmel.com/AppsAVR8/avr482_db101_software_users_guide/tags/release_A_code_20070917/IAR/memblock_lib/memblock_lib.h $
 * $Date: 2007-09-13 13:16:08 -0600 (to, 13 sep 2007) $  \n
 *
 * Copyright (c) 2006, Atmel Corporation All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of ATMEL may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
 * SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************/

#ifndef MEMBLOCK_LIB_H
#define MEMBLOCK_LIB_H

#include <stdint.h>
#include <stdbool.h>

/*********************
 * Convenience macros
 *********************/

//! Calculate required size in bytes for a bag's workspace.
#define MEMBLOCK_BAGSIZE(blocksize,blockcount) \
    (((blocksize) * (blockcount)) + (((blockcount) + 7) / 8))

/*********************
 * Types and typedefs
 *********************/
//! Data type for block sizes and counts.
typedef uint16_t MEMBLOCK_size_t;

//! A bag of memory blocks of equal size.
typedef struct MEMBLOCK_bag_struct {
    MEMBLOCK_size_t blockSize;      //!< Number of byte per block in this bag.
    MEMBLOCK_size_t blockCount;     //!< Total number of blocks.
    MEMBLOCK_size_t freeBlockCount; //!< Number of free blocks in this bag.
    uint8_t *bufferStart;           /*!< Pointer to first byte of memory
                                     * buffer. */
    uint8_t *freeBlockMap;          /*!< Bit map where "1" means free block.
                                     * MSB first. */

    // Debug purpose info:  
    MEMBLOCK_size_t peakUsage;      /*!< Keeps highest number of used blocks
                                     * ever. */
    MEMBLOCK_size_t minBlockSize;   /*!< Stores minimum block size request
                                     * for this bag. */
    MEMBLOCK_size_t maxBlockSize;   /*!< Stores maximum block size request
                                     * for this bag. */
    uint32_t allocationCount;       //!< Number of allocations.
    uint32_t allocationUtilization; /*!< 100% = 256, accumulated utilization,
                                     * compared to block size. */

} MEMBLOCK_bag_t;

/**********************
 * Function prototypes
 **********************/

//! Initialize entire manager, providing workspace for desired amount of bags.
void MEMBLOCK_InitManager(
    MEMBLOCK_size_t bagCount,
    MEMBLOCK_bag_t * workspace);
/*! Initialize one bag at given index. Ensure that block sizes are sorted,
 * smallest first. */
void MEMBLOCK_InitBag(
    MEMBLOCK_size_t bagIndex,
    MEMBLOCK_size_t blockSize,
    MEMBLOCK_size_t blockCount,
    uint8_t * workspace);
//! Total amount of memory in bytes.
MEMBLOCK_size_t MEMBLOCK_GetTotalMem(void);
//! Total amount of free memory in bytes.
MEMBLOCK_size_t MEMBLOCK_GetFreeMem(void);
//! Smallest free block in bytes.
MEMBLOCK_size_t MEMBLOCK_GetSmallestFreeBlock(void);
//! Largest free block size in bytes.
MEMBLOCK_size_t MEMBLOCK_GetLargestFreeBlock(void);
//! Allocate memory using smallest available block.
void *MEMBLOCK_Allocate(MEMBLOCK_size_t size);
//! Free previously allocated memory. Invalid addresses will be ignored.
void MEMBLOCK_Free(void const *ptr);

/*********************
 * Convenience macros
 *********************/

//! Allocate given a specififc datatype.
#define MEM_ALLOC(datatype)             MEMBLOCK_Allocate( sizeof(datatype) )
//! Allocate for an array of given datatype.
#define MEM_ALLOC_ARRAY(datatype,count) \
    MEMBLOCK_Allocate( sizeof(datatype) * (count) )
//! Free any data type without casting.
#define MEM_FREE(ptr)                   MEMBLOCK_Free( (void *) (ptr) )


#endif
// end of file
