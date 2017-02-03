/*!****************************************************************************
 * Implementation of generic block, that all forwards all messages to correct
 * data type determined by its signature.
 * 
 * This interface handles all calls to PNVMHeaderBlock type.
 *
 * *Note* that using correct type for values is much faster than this.
 *
 * @author Michal Minar <xminar06@stud.fit.vutbr.cz>
 * @year 2013
 */
#ifndef PNVM_GENERAL_BLOCK_H
#define PNVM_GENERAL_BLOCK_H

#if !defined(TARGET_ARDUINO) && DEBUG
    #include <ostream>
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

/******************************************************************************
 * Type interface
 */
template <>
inline bool pnvmTypeIndexable<PNVMHeaderBlock>(PNVMHeaderBlock const *o) {
    return type_indexable_flags[o->signature()];
}

template <>
unsigned int pnvmTypeLastCapacity<PNVMHeaderBlock>(
        PNVMHeaderBlock const *block);
template <>
unsigned int pnvmTypeNotLastCapacity<PNVMHeaderBlock>(
        PNVMHeaderBlock const *block);
template <>
unsigned int pnvmTypeTailLastCapacity<PNVMHeaderBlock>(
        PNVMHeaderBlock const *block);
template <>
unsigned int pnvmTypeTailNotLastCapacity<PNVMHeaderBlock>(
        PNVMHeaderBlock const *block);

template <>
unsigned int pnvmTypeTotalCapacity<PNVMHeaderBlock>(
        PNVMHeaderBlock const *block);

/******************************************************************************
 * Block interface
 */
template <>
inline PNVMTailBlock * pnvmBlockTail<PNVMHeaderBlock>(PNVMHeaderBlock * block)
{   /* treat it as tuple */
    return (PNVMTailBlock *) ((PNVMTupleHead *) block)->tail();
}
template <>
inline PNVMTailBlock * pnvmBlockTail<PNVMTailBlock>(PNVMTailBlock * block)
{   /* treat it as tuple */
    return (PNVMTailBlock *) ((PNVMTupleTail *) block)->tail();
}
template <>
inline PNVMHeaderBlock * pnvmBlockSetTail<PNVMHeaderBlock>(
        PNVMHeaderBlock * block, PNVMTailBlock *tail)
{   /* treat it as tuple */
    ((PNVMTupleHead *) block)->set_tail((PNVMTupleTail *) tail);
    return block;
}
template <>
inline PNVMTailBlock * pnvmBlockSetTail<PNVMTailBlock>(
        PNVMTailBlock * block, PNVMTailBlock *tail)
{   /* treat it as tuple */
    ((PNVMTupleTail *) block)->set_tail((PNVMTupleTail *) tail);
    return block;
}

/******************************************************************************
 * Value interface
 */
template <>
ItemCount_t pnvmValueItemCount<PNVMHeaderBlock>(PNVMHeaderBlock const *block);
template <>
unsigned int pnvmValueBlockCount<PNVMHeaderBlock>(
        PNVMHeaderBlock const *block);

template <>
inline Signature_t signature<PNVMHeaderBlock>(PNVMHeaderBlock const *o) {
    return o->signature();
}

template <>
PNVMHeaderBlock * pnvmValueSetItemCount<PNVMHeaderBlock>(
        PNVMHeaderBlock *block,
        unsigned int coun);

template <>
unsigned int pnvmValueTotalCapacityAt<PNVMHeaderBlock>(
        PNVMHeaderBlock const *block, int inde);

template <>
unsigned int pnvmValueIndexOfBlockAtItem<PNVMHeaderBlock>(
        PNVMHeaderBlock const *block, unsigned int inde);


void value_reference(PNVMHeaderBlock * block);

void value_unreference(PNVMHeaderBlock * block);

#if !defined(TARGET_ARDUINO) && DEBUG
    std::ostream & operator << (std::ostream &s, PNVMHeaderBlock const &);
#endif

#endif /* end of include guard: PNVM_GENERAL_BLOCK_H */
