// This file has been prepared for Doxygen automatic documentation generation.
/*! \file *********************************************************************
 *
 * \brief  Memory block manager library source file
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
 * $Revision: 2417 $
 * $URL: http://revisor.norway.atmel.com/AppsAVR8/avr482_db101_software_users_guide/tags/release_A_code_20070917/IAR/memblock_lib/memblock_lib.c $
 * $Date: 2007-09-17 02:32:17 -0600 (ma, 17 sep 2007) $  \n
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

#include <Arduino.h>
#include "memblock_lib.h"


/*****************************
 * Private types and typedefs
 *****************************/

//! A manager of sorted memory block bags. Smallest block size first.
typedef struct MEMBLOCK_manager_struct {
    MEMBLOCK_size_t bagCount;        //!< Number of bags.
    MEMBLOCK_bag_t *bags;        //!< List of bag objects.
    uint8_t invalidFreeAttemptCount;        //!< For debug purposes.
} MEMBLOCK_manager_t;



/********************
 * Private variables
 ********************/

static MEMBLOCK_manager_t MEMBLOCK_manager;


/***************************
 * Function implementations
 ***************************/

static inline void *MEMBLOCK_ReserveFirstFreeBlock(
    MEMBLOCK_bag_t * bag)
{
    // NOTE: This function assumes that there IS a free block in the bag.

    // Iterate through map and find first "1" bit.
    uint8_t *mapPtr = bag->freeBlockMap;
    MEMBLOCK_size_t blockIndex = 0;

    // Find first byte with at least one "1" bit.
    while (*mapPtr == 0x00) {
        ++mapPtr;
        blockIndex += 8;
    }

    // Find first "1" bit in byte, start with MSB.
    uint8_t bitmask = 0x80;        // MSB=1.
    while ((*mapPtr & bitmask) == 0x00) {
        bitmask >>= 1;
        ++blockIndex;
    }

    // Mark block as not free anymore.
    *mapPtr &= ~bitmask;
    --(bag->freeBlockCount);

    // Return pointer to block.
    void *ptr = ((uint8_t *) bag->bufferStart) + (blockIndex * bag->blockSize);
    return ptr;
}

/*!
 * \param  bagCount  Number of bags
 * \param  workspace  Pointer to an allocated MEMBLOCK_bag_t struct
 */
void MEMBLOCK_InitManager(
    MEMBLOCK_size_t bagCount,
    MEMBLOCK_bag_t * workspace)
{
    MEMBLOCK_manager.bagCount = bagCount;
    MEMBLOCK_manager.bags = workspace;
}


/*!
 * Bags must be sorted after blocksize, with smallest first (ie, bag with index 0 must have smallest blocks)
 *
 * \param  bagIndex  Index of the bag to init
 * \param  blockSize  What blocksize should the bag have?
 * \param  blockCount  And how many blocks?
 * \param  workspace  Pointer to an already allocated memory space where the bag will "allocate" it's data.
 *
 */
void MEMBLOCK_InitBag(
    MEMBLOCK_size_t bagIndex,
    MEMBLOCK_size_t blockSize,
    MEMBLOCK_size_t blockCount,
    uint8_t * workspace)
{
    MEMBLOCK_bag_t *const bag = MEMBLOCK_manager.bags + bagIndex;

    // Init state variables.
    bag->blockSize = blockSize;
    bag->blockCount = blockCount;
    bag->freeBlockCount = blockCount;
    bag->peakUsage = 0;
    bag->minBlockSize = blockSize;
    bag->maxBlockSize = 0;
    bag->allocationCount = 0;
    bag->allocationUtilization = 0;

    // Init pointers, have freeBlockMap located immediately after the blocks themselves.
    bag->bufferStart = workspace;
    bag->freeBlockMap = workspace + (blockSize * blockCount);

    // Set all map bits to "1".
    uint8_t *mapPtr = bag->freeBlockMap;
    MEMBLOCK_size_t mapBytesLeft = ((blockCount + 7) / 8);
    while (mapBytesLeft > 0) {
        *mapPtr = 0xff;
        ++mapPtr;
        --mapBytesLeft;
    }
}

/*!
 * \return  Total available memory in bytes
 */
MEMBLOCK_size_t MEMBLOCK_GetTotalMem(
    void)
{
    // Iterate through bags and add total memory.
    MEMBLOCK_size_t totalMem = 0;
    MEMBLOCK_bag_t const *bag = MEMBLOCK_manager.bags;
    MEMBLOCK_size_t bagsLeft = MEMBLOCK_manager.bagCount;

    while (bagsLeft > 0) {
        totalMem += bag->blockSize * bag->blockCount;
        ++bag;
        --bagsLeft;
    }

    return totalMem;
}


/*!
 * \return  Free memory in bytes
 */
MEMBLOCK_size_t MEMBLOCK_GetFreeMem(
    void)
{
    // Iterate through bags and add free memory.
    MEMBLOCK_size_t freeMem = 0;
    MEMBLOCK_bag_t const *bag = MEMBLOCK_manager.bags;
    MEMBLOCK_size_t bagsLeft = MEMBLOCK_manager.bagCount;

    while (bagsLeft > 0) {
        freeMem += bag->blockSize * bag->freeBlockCount;
        ++bag;
        --bagsLeft;
    }

    return freeMem;
}


/*!
 * \return  How big is the smallest available block in bytes?
 */
MEMBLOCK_size_t MEMBLOCK_GetSmallestFreeBlock(
    void)
{
    // Iterate through bags and find first bag with free blocks.
    MEMBLOCK_bag_t const *bag = MEMBLOCK_manager.bags;
    MEMBLOCK_size_t bagsLeft = MEMBLOCK_manager.bagCount;

    bool found = false;
    while ((bagsLeft > 0) && (found == false)) {
        // Have we found a bag with free blocks?
        if (bag->freeBlockCount > 0) {
            found = true;
        } else {
            ++bag;
            --bagsLeft;
        }
    }

    // Return block size if found, zero otherwise.
    if (found) {
        return bag->blockSize;
    } else {
        return 0;
    }
}

/*!
 * \return  How big is the biggest available block in bytes?
 */
MEMBLOCK_size_t MEMBLOCK_GetLargestFreeBlock(
    void)
{
    // Iterate backwards through bags and find first bag with free blocks.
    MEMBLOCK_bag_t const *bag =
        MEMBLOCK_manager.bags + (MEMBLOCK_manager.bagCount - 1);
    MEMBLOCK_size_t bagsLeft = MEMBLOCK_manager.bagCount;

    bool found = false;
    while ((bagsLeft > 0) && (found == false)) {
        // Have we found a bag with free blocks?
        if (bag->freeBlockCount > 0) {
            found = true;
        } else {
            --bag;
            --bagsLeft;
        }
    }

    // Return block size if found, zero otherwise.
    if (found) {
        return bag->blockSize;
    } else {
        return 0;
    }
}

/*!
 * \param  size  In bytes how much memory we want to allocate
 *
 * \return  Pointer to allocated memory
 */
void *MEMBLOCK_Allocate(
    MEMBLOCK_size_t size)
{
    // Work undisturbed.
    uint8_t savedSREG = SREG;
    noInterrupts();

    // Iterate through bags and find first bag with suitable blocks.
    MEMBLOCK_bag_t *bag = MEMBLOCK_manager.bags;
    MEMBLOCK_size_t bagsLeft = MEMBLOCK_manager.bagCount;

    bool found = false;
    while ((bagsLeft > 0) && (found == false)) {
        // Have we found a bag with large enough blocks and with free blocks?
        if ((bag->blockSize >= size) && (bag->freeBlockCount > 0)) {
            found = true;
        } else {
            ++bag;
            --bagsLeft;
        }
    }

    // If found, allocate it.
    void *ptr;
    if (found) {
        // Mark and update free count.
        ptr = MEMBLOCK_ReserveFirstFreeBlock(bag);

        // Update peak usage if required.
        MEMBLOCK_size_t usedBlocks = bag->blockCount - bag->freeBlockCount;
        if (usedBlocks > bag->peakUsage) {
            bag->peakUsage = usedBlocks;
        }
        // Update allocation utilization tracking.
        if (size > bag->maxBlockSize) {
            bag->maxBlockSize = size;
        }
        if (size < bag->minBlockSize) {
            bag->minBlockSize = size;
        }
        ++(bag->allocationCount);
        uint32_t utilization = (uint32_t) size * 256 / bag->blockSize;
        bag->allocationUtilization += utilization;
    } else {
        ptr = NULL;
    }

    // Restore interrupt state and return.
    interrupts();
    SREG = savedSREG;
    return ptr;
}

/*!
 * \param  ptr  Pointer to allocated memory
 */
void MEMBLOCK_Free(
    void const *ptr)
{
    // Always ok to free a NULL, since users would find that convenient.
    if (ptr == NULL) {
        return;
    }
    // Iterate through bags and find bag that contains the block.
    MEMBLOCK_bag_t *bag = MEMBLOCK_manager.bags;
    MEMBLOCK_size_t bagsLeft = MEMBLOCK_manager.bagCount;

    bool found = false;
    while ((bagsLeft > 0) && (found == false)) {
        // Is pointer within this bag's bounds?
        if ((ptr >= (void *) bag->bufferStart)
            && (ptr < ((void *) bag->freeBlockMap))) {
            found = true;
        } else {
            ++bag;
            --bagsLeft;
        }
    }

    // If found, free it. If not, increase invalid free attempt count in manager.
    if (found) {
        // Calculate block index first.
        MEMBLOCK_size_t bufferIndex = ((uint8_t *) ptr) - bag->bufferStart;
        MEMBLOCK_size_t blockIndex = bufferIndex / bag->blockSize;

        // Mark block as free and update free block count.
        bag->freeBlockMap[blockIndex / 8] |= (0x80 >> (blockIndex % 8));
        ++(bag->freeBlockCount);
    } else {
        ++(MEMBLOCK_manager.invalidFreeAttemptCount);
    }
}
