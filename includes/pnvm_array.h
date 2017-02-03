/******************************************************************************
 * Array user data type declarations.
 *
 * This is a modifiable, indexable and collectable collection.
 *
 * @author Michal Minar <xminar06@stud.fit.vutbr.cz>
 * @year 2013
 */
#ifndef PNVM_ARRAY_H
#define PNVM_ARRAY_H

#if !defined(TARGET_ARDUINO) && DEBUG
    #include <ostream>
#endif
#include "pnvm_block.h"
#include "pnvm_token.h"

typedef PNVMBlock<SIGN_ARRAY, true, true, HeaderIC, PNVMToken>::Head PNVMArrayHead;
typedef PNVMBlock<SIGN_ARRAY, true, true, HeaderIC, PNVMToken>::Tail PNVMArrayTail;
typedef PNVMIterator<PNVMArrayHead> PNVMArrayIter;

/******************************************************************************
 * Factories
 */
PNVMArrayHead * pnvmArrayNew();

/******************************************************************************
 * Iterator interface
 */
PNVMToken & pnvmIterSetItem(PNVMArrayIter & i, PNVMToken & value);

PNVMToken & pnvmIterWrite(PNVMArrayIter & i, PNVMToken & value);

#if !defined(TARGET_ARDUINO) && DEBUG
    std::ostream & operator << (std::ostream &s, PNVMArrayHead const &);
#endif

#endif /* end of include guard: PNVM_ARRAY_H */
