/******************************************************************************
 * Generic interface for dynamically allocated values.
 *
 * @author Michal Minar <xminar06@stud.fit.vutbr.cz>
 * @year 2013
 */
#ifndef PNVM_TYPE_INTERFACE_H
#define PNVM_TYPE_INTERFACE_H

#if !defined(TARGET_ARDUINO) && DEBUG
    #include <iostream>
#endif
#include "static_assert.h"
#include "pnvm_config.h"
#ifndef TARGET_ARDUINO
    #include <algorithm>

    using std::max;
#endif

/******************************************************************************
 * Utilities
 */
/**
 * This release resources kept by item of block.
 * It must be specialized for any type that needs to release resources.
 */
template <typename Item>
inline void pnvm_item_delete(Item &) {
    //ASSERTM(false, ItemDelete_not_specialized);
}
/**
 * This only initialize item of block to "nil" value without releasing any
 * referenced resource.
 */
template <typename Item>
inline void pnvm_item_reinitialize(Item &) {
    //ASSERTM(false, ItemReinitialize_not_specialized);
}
/******************************************************************************
 * PNVM generic block interface
 */
template <typename T>
bool pnvmTypeIndexable(T const*) {
    return T::s_indexable;
}

template <typename T>
unsigned int pnvmTypeLastCapacity(T const*) {
    return T::s_last_capacity();
}
template <typename T>
unsigned int pnvmTypeNotLastCapacity(T const*) {
    return T::s_not_last_capacity();
}
template <typename T>
unsigned int pnvmTypeTailLastCapacity(T const*) {
    return T::tail_type_t::s_last_capacity();
}
template <typename T>
unsigned int pnvmTypeTailNotLastCapacity(T const*) {
    return T::tail_type_t::s_not_last_capacity();
}
template <typename T>
unsigned int pnvmTypeTotalCapacity(T const *) {
    return T::head_type_t::s_max_item_count();
}

template <typename T>
typename T::tail_type_t * pnvmBlockAppendBlock(T * block) {
    typename T::tail_type_t * tail = new (typename T::tail_type_t);
    for (unsigned i=T::s_not_last_capacity(); i < T::s_last_capacity(); ++i) {
        (*tail)[i - T::s_not_last_capacity()] = (*block)[i];
        pnvm_item_reinitialize((*block)[i]);
    }
    block->set_tail(tail);
    return tail;
}

template <typename T>
unsigned int pnvmValueBlockCount(T const * block) {
    ASSERTM(T::s_is_head, Block_must_be_head);
    if (!T::s_indexable) return 1;
    if (block->item_count() <= T::s_last_capacity()) return 1;
    int middle_count = block->item_count() - T::s_not_last_capacity()
        - T::tail_type_t::s_last_capacity();
    int one_more = (
               middle_count > 0
            && (middle_count % T::tail_type_t::s_not_last_capacity()) > 0)
        ? 1 : 0;
    return 2 + one_more + max(
            (middle_count/(int)T::tail_type_t::s_not_last_capacity()), 0);
}

template <typename T>
T * pnvmValueIncItemCount(T * block) {
    ASSERTM(T::s_is_head, Block_is_not_head);
    ASSERTM(T::s_indexable, Block_is_not_indexable);
    block->inc_item_count();
    return block;
}

template <typename T>
ItemCount_t pnvmValueItemCount(T const *block) {
    ASSERTM(T::s_is_head, Block_is_not_head);
    ASSERTM(T::s_indexable, Block_is_not_indexable);
    return block->item_count();
}

template <typename T>
typename T::iterator pnvmValueIterator(T * block) {
    ASSERTM(T::s_is_head, Block_is_not_head);
    ASSERTM(T::s_indexable, Block_is_not_indexable);
    if (  pnvmValueBlockCount(block) > 1
       && pnvmValueTotalCapacityAt(block, 0) == 0) {
        return typename T::iterator(block, block->tail(), 0);
    }else {
        return typename T::iterator(block, block, 0);
    }

}

template <typename T>
int pnvmValueRefCount(T * block) {
    ASSERTM(T::s_is_head, Block_is_not_head);
    ASSERTM(T::s_collectable, Block_is_not_collectable);
    return block->ref_count();
}

template <typename T>
T * pnvmValueReference(T * block) {
    ASSERTM(T::s_is_head, Block_is_not_head);
    ASSERTM(T::s_collectable, Block_is_not_collectable);
    #if !defined(TARGET_ARDUINO) && DEBUG
        LOG4CXX_DEBUG(memlog, "referencing " << (*block))
    #endif
    block->inc_ref_count();
    return block;
}

template <typename T>
T * pnvmValueSetItemCount(T * block, unsigned int count) {
    ASSERTM(T::s_is_head, Block_is_not_head);
    ASSERTM(T::s_indexable, Block_is_not_indexable);
    block->set_item_count(count);
    return block;
}

template <typename T>
T * pnvmBlockSetTail(T * block, typename T::tail_type_t * tail) {
    ASSERTM(T::s_indexable, Block_is_not_indexable);
    block->set_tail(tail);
    return block;
}

template <typename T>
typename T::tail_type_t * pnvmBlockTail(T * block) {
    ASSERTM(T::s_indexable, Block_is_not_indexable);
    return block->tail();
}

template <typename T>
unsigned int pnvmValueIndexOfBlockAtItem(
        T const * block,
        unsigned int index)
{
    ASSERTM(T::s_is_head, Block_is_not_head);
    ASSERTM(T::s_indexable, Block_is_not_indexable);
    //assert(index < block->item_count());
    if (block->item_count() <= T::s_last_capacity())
        return 0;
    if ((int) index < (int) T::s_not_last_capacity())
        return 0;
    int rest = (int) index - (int) T::s_not_last_capacity();
    int bc = pnvmValueBlockCount(block) - 2;
    if (rest >= (int) (bc*T::tail_type_t::s_not_last_capacity()))
        return bc + 1;
    return 1 + (rest / (int) T::tail_type_t::s_not_last_capacity());
}

template <typename T>
unsigned int pnvmValueTotalCapacityAt(
        T const * block,
        int index)
{
    ASSERTM(T::s_is_head, Block_is_not_head);
    ASSERTM(T::s_indexable, Block_is_not_indexable);
    if (index < 0)
        return 0;
    if (block->item_count() <= T::s_last_capacity())
        return T::s_last_capacity();
    if (index < 1)
        return T::s_not_last_capacity();
    int last_block_capacity = pnvmValueIsLastBlockAt(block, index) ?
        T::tail_type_t::s_last_capacity() : T::tail_type_t::s_not_last_capacity();
    return ( T::s_not_last_capacity()
           + ((index - 1)*T::tail_type_t::s_not_last_capacity())
           + last_block_capacity);
}

template <typename T>
ItemCount_t pnvmValueIsLastBlockAt(
        T const * block,
        unsigned int index)
{
    return pnvmValueBlockCount(block) <= index + 1;
}

template <typename T>
ItemCount_t pnvmValueBlockCapacityAt(
        T const * block,
        unsigned int index)
{
    ASSERTM(T::s_is_head, Block_is_not_head);
    ASSERTM(T::s_indexable, Block_is_not_indexable);
    if (block->item_count() <= T::s_last_capacity())
        return T::s_last_capacity();
    if (index < 1)
        return T::s_not_last_capacity();
    if (pnvmValueIsLastBlockAt(block, index))
        return T::tail_type_t::s_last_capacity();
    return T::tail_type_t::s_not_last_capacity();
}

template <typename T>
ItemCount_t pnvmValueBlockItemCountAt(
        T const * block,
        int index)
{
    if (pnvmValueIsLastBlockAt(block, index)) {
        return (block->item_count() - (pnvmValueTotalCapacityAt(
                        block, pnvmValueBlockCount(block) - 2)));
    }
    if (index < 1) {
        return T::s_not_last_capacity();
    }
    return T::tail_type_t::s_not_last_capacity();
}

/******************************************************************************
 * Deletion
 */
/*!
 * Delete just one block.
 * This does not care for any items or tail.
 */
template <typename T>
void pnvmBlockDelete(T * block) {
    delete block;
}

/*!
 * Delete all blocks of values together with items
 */
template <typename T, bool Indexable>
struct _ValueIterativeDelete {
    void operator () (T * block) const {
        typename T::tail_type_t * tail, *tmp;
        for (unsigned i=0; i < pnvmValueBlockItemCountAt(block, 0); ++i) {
            pnvm_item_delete((*block)[i]);
        }
        if (!pnvmValueIsLastBlockAt(block, 0)) {
            tail = block->tail();
            for (int bi=1; bi < (int)pnvmValueBlockCount(block); ++bi) {
                for ( unsigned i=0; i < pnvmValueBlockItemCountAt(block, bi)
                    ; ++i)
                {
                    pnvm_item_delete((*tail)[i]);
                }
                tmp = ((typename T::tail_type_t *) tail)->tail();
                delete tail;
                tail = tmp;
            }
        }
        delete block;
    }
};

/**
 * Specialization for not-indexable data types.
 */
template <typename T>
struct _ValueIterativeDelete<T, false> {
    void operator () (T * block) const {
        delete block;
    }
};

template <typename T>
void pnvmValueDelete(T * block) {
    ASSERTM(T::s_is_head, Block_is_not_head);
    #if !defined(TARGET_ARDUINO) && DEBUG
        LOG4CXX_DEBUG(memlog, "deleting " << (*block))
    #endif
    _ValueIterativeDelete<T, T::s_indexable>()(block);
}

template <typename T>
T * pnvmValueUnreference(T * block) {
    ASSERTM(T::s_is_head, Block_is_not_head);
    ASSERTM(T::s_collectable, Block_is_not_head);
    #if !defined(TARGET_ARDUINO) && DEBUG
        LOG4CXX_DEBUG(memlog, "unreferencing " << (*block))
    #endif
    block->dec_ref_count();
    if (block->ref_count() <= 0) {
        pnvmValueDelete(block);
        return NULL;
    }
    return block;
}

template <typename T>
Signature_t signature(T const *) {
    return T::s_signature;
}

#endif /* end of include guard: PNVM_TYPE_INTERFACE_H */
