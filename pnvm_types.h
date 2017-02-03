/******************************************************************************
 * Merges all the types and interfaces into single header.
 *
 * Does some compile time checks. 
 *
 * @author Michal Minar <xminar06@stud.fit.vutbr.cz>
 * @year 2013
 */
#ifndef PNVM_TYPES_H
#define PNVM_TYPES_H

#include <math.h>
#include "static_assert.h"
#ifdef USE_MEMBLOCK
  #include "memblock_lib.h"
#endif

#include "pnvm_block.h"
#include "pnvm_string.h"
#include "pnvm_tuple.h"
#include "pnvm_array.h"
#include "pnvm_place.h"
#include "pnvm_net_inst.h"
#include "pnvm_template.h"
#include "pnvm_event.h"
#include "pnvm_iterator.h"
#include "pnvm_generic_block.h"
#include "pnvm_generic_iterator.h"

/******************************************************************************
 * Type size checks
 */
ASSERTM(AllocationBlockSize % PointerSize == 0,
        BlockSize_not_multiple_of_pointer_size);
ASSERTM(sizeof(PNVMToken) == PointerSize, Token_invalid_size);
ASSERTM(sizeof(PNVMPlaceItem) == 2*PointerSize, Token_invalid_size);
ASSERTM(sizeof(PNVMStringHead) == AllocationBlockSize, StringHead_invalid_size);
ASSERT_SAME_SIZE(PNVMStringHead, PNVMStringTail);
ASSERT_SAME_SIZE(PNVMStringHead, PNVMTupleHead);
ASSERT_SAME_SIZE(PNVMTupleHead, PNVMTupleTail);
ASSERT_SAME_SIZE(PNVMTupleHead, PNVMArrayHead);
ASSERT_SAME_SIZE(PNVMArrayHead, PNVMArrayTail);
ASSERT_SAME_SIZE(PNVMArrayHead, PNVMPlaceHead);
ASSERT_SAME_SIZE(PNVMPlaceHead, PNVMPlaceTail);
ASSERT_SAME_SIZE(PNVMPlaceHead, PNVMNetInstHead);
ASSERT_SAME_SIZE(PNVMNetInstHead, PNVMNetInstTail);
ASSERT_SAME_SIZE(PNVMNetInstHead, PNVMEvent);
#if BLOCK_SIZE != 32
    // this is not true for 32bits
    ASSERT_SAME_SIZE(PNVMEvent, PNVMTemplate);
#else
    // it prevents us to use MEMBLOCK lib for x86
    #ifdef MEMBLOCK_LIB_H
        #error "can not use memblock lib on 32bits because of too" \
            " large template structure"
    #endif
#endif

/******************************************************************************
 * Token interface additions
 */
inline PNVMHeaderBlock const * pnvmTokenPointer(PNVMToken const &t) {
    return (PNVMHeaderBlock *) t.get_address();
}
inline PNVMHeaderBlock * pnvmTokenPointer(PNVMToken &t) {
    return (PNVMHeaderBlock *) t.get_address();
}

inline PNVMNetInstHead const * pnvmTokenNetRef(PNVMToken const &t) {
    return (PNVMNetInstHead *) t.get_address();
}
inline PNVMNetInstHead * pnvmTokenNetRef(PNVMToken &t) {
    return (PNVMNetInstHead *) t.get_address();
}

#endif /* end of include guard: PNVM_TYPES_H */
