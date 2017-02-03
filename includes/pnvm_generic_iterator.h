/*!****************************************************************************
 * Iterator for generic block.
 *
 * Messages are forwarded to iterators for correct type.
 *
 * @author Michal Minar <xminar06@stud.fit.vutbr.cz>
 * @year 2013
 */
#ifndef PNVM_GENERIC_ITERATOR_H
#define PNVM_GENERIC_ITERATOR_H

#include "pnvm_generic_block.h"

/*!
 * Specialization of block iterator for generic blocks.
 */
template <>
struct PNVMIterator<PNVMHeaderBlock> : PNVMBaseIterator<PNVMHeaderBlock> {
    typedef PNVMBaseIterator<PNVMHeaderBlock> base_iterator_t;

    /**************************************************************************
     * Constructors
     */
    PNVMIterator(): base_iterator_t() {}
    PNVMIterator(PNVMHeaderBlock * hb, void * cb, unsigned int i):
        base_iterator_t(hb, cb, i) {};
    template<typename T>
    PNVMIterator(PNVMBaseIterator<T> const & other):
        base_iterator_t(other.headerBlock, other.currentBlock, other.item) {}
    PNVMIterator(PNVMIterator<PNVMHeaderBlock> const & other):
        base_iterator_t(other) {}

    /**************************************************************************
     * Overloaded operators
     */
    PNVMIterator<PNVMHeaderBlock>& operator ++ ();

    PNVMIterator<PNVMHeaderBlock> operator ++ (int);

    void const * operator * () const;

    void * operator * ();

    /**************************************************************************
     * Methods
     */
    bool at_start() const { return item == 0; }

    bool at_end() const;

    bool at_last_block() const;

    int block_index() const;

    bool at_block_end() const;

    void next_block();

};

typedef PNVMIterator<PNVMHeaderBlock> PNVMItemIterator;

/**
 * Make collection-specific iterator out of general one.
 */
PNVMStringIter & pnvmIterString(PNVMItemIterator &i);
PNVMArrayIter & pnvmIterArray(PNVMItemIterator &i);
#ifdef TARGET_ARDUINO
    #define pnvmIterTuple(i) pnvmIterArray(i)
#else
    PNVMTupleIter & pnvmIterTuple(PNVMItemIterator &i);
#endif
PNVMNetInstIter & pnvmIterNetInst(PNVMItemIterator &i);
PNVMPlaceIter & pnvmIterPlace(PNVMItemIterator &i);

/**
 * Make general iterator out of collection-specific one.
 */
template <typename T>
PNVMItemIterator & pnvmIterAsItemIterator(PNVMIterator<T> & i) {
    return *reinterpret_cast<PNVMItemIterator *>(&i);
}

template <>
inline PNVMHeaderBlock::iterator pnvmValueIterator<PNVMHeaderBlock>(PNVMHeaderBlock * block) {
    return PNVMItemIterator(block, block, 0);
}

/******************************************************************************
 * Iterator interface specializations
 */
inline void const * pnvmIterValue(PNVMItemIterator const &i) {
    return *i;
}
inline void * pnvmIterValue(PNVMItemIterator &i) {
    return *i;
}
inline void * pnvmIterNext(PNVMItemIterator &i) {
    return *(i++);
}

#endif /* end of include guard: PNVM_GENERIC_ITERATOR_H */
