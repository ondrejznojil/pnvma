/******************************************************************************
 * Tuple user data type declarations.
 *
 * This is a non-modifiable, indexable and collectable collection.
 *
 * @author Michal Minar <xminar06@stud.fit.vutbr.cz>
 * @year 2013
 */
#ifndef PNVM_TUPLE_H
#define PNVM_TUPLE_H

#if !defined(TARGET_ARDUINO) && DEBUG
    #include <ostream>
#endif
#include "pnvm_block.h"
#include "pnvm_token.h"

#ifdef TARGET_ARDUINO
    #include "pnvm_array.h"
    #define PNVMTupleHead PNVMArrayHead
    #define PNVMTupleTail PNVMArrayTail
    #define PNVMTupleIter PNVMArrayIter
    #define pnvmTupleNew() pnvmArrayNew()
#else

    typedef PNVMBlock<SIGN_TUPLE, true, true, HeaderIC, PNVMToken>::Head
        PNVMTupleHead;
    typedef PNVMBlock<SIGN_TUPLE, true, true, HeaderIC, PNVMToken>::Tail
        PNVMTupleTail;
    typedef PNVMIterator<PNVMTupleHead> PNVMTupleIter;

    /**************************************************************************
     * Factories
     */
    PNVMTupleHead * pnvmTupleNew();

    /**************************************************************************
     * Iterator interface
     */
    PNVMToken & pnvmIterSetItem(PNVMTupleIter & i, PNVMToken & value);

    PNVMToken & pnvmIterWrite(PNVMTupleIter & i, PNVMToken & value);

    #if !defined(TARGET_ARDUINO) && DEBUG
        std::ostream & operator << (std::ostream &s, PNVMTupleHead const &);
    #endif

#endif

#endif /* end of include guard: PNVM_TUPLE_H */
