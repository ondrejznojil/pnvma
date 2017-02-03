/******************************************************************************
 * Iterator for indexable data types.
 *
 * @author Michal Minar <xminar06@stud.fit.vutbr.cz>
 * @year 2013
 */
#ifndef PNVM_ITERATOR_H
#define PNVM_ITERATOR_H

#include "pnvm_config.h"
#include "pnvm_type_interface.h"

/******************************************************************************
 * Non-template iterator utilities
 */
/*!
 * Check, whether iterator is out of collection boundaries (maximum number
 * of items the collection can store) and terminate the program if true.
 * On Arduino just set the error.
 * 
 * :param hb header block of iterated value
 *
 * Return true if out of boundaries.
 */
bool iter_is_out_of_bounds(
        long item,
        ItemCount_t total_capacity,
        void * hb);

/*!
 * Check, whether iterator if item points behind the last stored item
 * of collection. Terminate the program if true.
 * On Arduino just set the error.
 *
 * :param hb header block of iterated value
 *
 * Return true if out of bounds.
 */
bool iter_access_out_of_bounds(
        long item,
        ItemCount_t item_count,
        void * hb);

/******************************************************************************
 * Main iterator class
 */
template <typename Block>
struct PNVMBaseIterator {
    typedef Block head_type_t;
    typedef typename Block::item_type_t item_type_t;
    typedef typename Block::tail_type_t tail_type_t;
    /**************************************************************************
     * Public attributes
     */
    Block * headerBlock;
    void * currentBlock;
    unsigned int item;

    /**************************************************************************
     * Constructors
     */
    PNVMBaseIterator(): headerBlock(NULL), currentBlock(NULL), item(0) {};
    PNVMBaseIterator(Block * hb, void * cb, unsigned int i):
        headerBlock(hb), currentBlock(cb), item(i) {};
    PNVMBaseIterator(PNVMBaseIterator<Block> const &other):
        headerBlock(other.headerBlock), currentBlock(other.currentBlock),
        item(other.item) {};

    /**************************************************************************
     * Overloaded operators
     */
    PNVMBaseIterator<Block> & operator = (
            PNVMBaseIterator<Block> const & other)
    {
        headerBlock = other.headerBlock;
        currentBlock = other.currentBlock;
        item = other.item;
        return *this;
    }

    /**************************************************************************
     * Methods
     */
    bool at_start() const { return item == 0; }

    bool at_end() const { return item >= pnvmValueItemCount(headerBlock); }

    PNVMBaseIterator<Block> & operator ++ () {
        if (at_block_end() && !at_last_block()) {
            currentBlock = (void *) reinterpret_cast<tail_type_t *>(
                    currentBlock)->tail();
        }
        item += 1;
        return *this;
    }

    PNVMBaseIterator<Block> operator ++ (int) {
        PNVMBaseIterator<Block> tmp = *this;
        operator++();
        return tmp;
    }

    bool operator == (PNVMBaseIterator<Block> const & other) const
    {
        return (  headerBlock  == other.headerBlock
               && currentBlock == other.currentBlock
               && item         == other.item);
    }

    bool operator != (PNVMBaseIterator<Block> const & other) const
    {
        return (  headerBlock  != other.headerBlock
               || currentBlock != other.currentBlock
               || item         != other.item);
    }

    item_type_t const & operator * () const {
        if (headerBlock == currentBlock) {
            return ((Block *) currentBlock)->operator[](block_index());
        }else {
            return ((tail_type_t *) currentBlock)->operator[](block_index());
        }
    }

    item_type_t & operator * () {
        if (headerBlock == currentBlock) {
            return (*((Block *) currentBlock))[block_index()];
        }else {
            return (*((tail_type_t *) currentBlock))[block_index()];
        }
    }

    bool at_block_start() const {
        return block_index() == 0;
    }

    bool at_block_end() const {
        return pnvmValueTotalCapacityAt(headerBlock, index_of_block()) - 1
            == item;
    }

    bool at_last_block() const {
        return pnvmValueIsLastBlockAt(headerBlock, index_of_block());
    }

    unsigned int block_index() const {
        if (currentBlock == headerBlock)
            return item;
        return item - pnvmValueTotalCapacityAt(
                headerBlock, index_of_block() - 1);
    }

    unsigned int index_of_block() const {
        return pnvmValueIndexOfBlockAtItem(headerBlock, item);
    }

    void next_block() {
        item = pnvmValueTotalCapacityAt(headerBlock, index_of_block());
        currentBlock = reinterpret_cast<tail_type_t *>(
                currentBlock)->tail();
    }

};

template <typename B>
struct PNVMIterator : PNVMBaseIterator<B> {
    typedef PNVMBaseIterator<B> base_iterator_t;

    PNVMIterator(): base_iterator_t() {};
    PNVMIterator(B * hb, void * cb, unsigned int i):
        base_iterator_t(hb, cb, i) {};

};

/******************************************************************************
 * interpreter iterator interface
 */
template <typename T>
PNVMIterator<T> & pnvmIterAssign(PNVMIterator<T> & dest,
        PNVMIterator<T> const & src)
{
    return dest = src;
}

template <typename T>
bool pnvmIterAtStart(PNVMIterator<T> const & i) {
    return i.at_start();
}

template <typename T>
bool pnvmIterAtEnd(PNVMIterator<T> const & i) {
    return i.at_end();
}

template <typename T>
bool pnvmIterAtBlockStart(PNVMIterator<T> const & i) {
    return i.at_block_start();
}

template <typename T>
bool pnvmIterAtBlockEnd(PNVMIterator<T> const & i) {
    return i.at_block_end();
}

template <typename T>
bool pnvmIterAtLastBlock(PNVMIterator<T> const & i) {
    return i.at_last_block();
}

template <typename T>
int pnvmIterBlockIndex(PNVMIterator<T> const & i) {
    return i.block_index();
}

template <typename T>
bool pnvmIterCanAppend(PNVMIterator<T> const & i) {
    return i.item < pnvmTypeTotalCapacity(i.headerBlock);
}

template <typename T>
void * pnvmIterCurrentBlock(PNVMIterator<T> const & i) {
    return i.currentBlock;
}

template <typename T>
T * pnvmIterHeaderBlock(PNVMIterator<T> const & i) {
    return i.headerBlock;
}

template <typename T>
int pnvmIterIndex(PNVMIterator<T> const & i) {
    return i.item;
}

template <typename T>
typename T::item_type_t & pnvmIterNext(PNVMIterator<T> & i) {
    if (!iter_is_out_of_bounds(i.item,
                pnvmTypeTotalCapacity(i.headerBlock),
                i.headerBlock)) {
        return *i++;
    }else {
        return *i;
    }
}

template <typename T>
PNVMIterator<T> & pnvmIterNextBlock(PNVMIterator<T> & i) {
    if (!iter_access_out_of_bounds(
                pnvmValueTotalCapacityAt(i.headerBlock, i.index_of_block()),
                pnvmValueItemCount(i.headerBlock),
                i.headerBlock))
    {
        i.next_block();
    }
    return i;
}

template <typename T>
PNVMIterator<T> & pnvmIterSeekEnd(PNVMIterator<T> & i) {
    while (!pnvmIterAtLastBlock(i)) {
        pnvmIterNextBlock(i);
    }
    i.item = pnvmValueItemCount(i.headerBlock);
    return i;
}

template <typename T>
PNVMIterator<T> & pnvmIterSeekStart(PNVMIterator<T> & i) {
    i.item = 0;
    if (  pnvmValueBlockCount(i.headerBlock) > 1
       && pnvmValueTotalCapacityAt(i.headerBlock, 0) == 0) {
        i.currentBlock = i.headerBlock->tail();
    }else {
        i.currentBlock = i.headerBlock;
    }
    return i;
}

template <typename T>
PNVMIterator<T> & pnvmIterSeek(PNVMIterator<T> & i, int index) {
    if (!iter_is_out_of_bounds((long) index - 1,
                pnvmTypeTotalCapacity(i.headerBlock),
                i.headerBlock))
    {
        int bi = pnvmValueIndexOfBlockAtItem(i.headerBlock, i.item);
        if (index <= (int) pnvmValueItemCount(i.headerBlock)) {
            pnvmIterSeekStart(i);
            bi = 0;
        }
        while (  !pnvmIterAtLastBlock(i)
              && (index >= (int) pnvmValueTotalCapacityAt(i.headerBlock, bi)))
        {
            pnvmIterNextBlock(i);
            ++bi;
        }
        i.item = index;
    }
    return i;
}

template <typename T>
typename T::item_type_t & pnvmIterSetItem(
        PNVMIterator<T> & i, typename T::item_type_t & value)
{
    if (iter_is_out_of_bounds(i.item,
                pnvmTypeTotalCapacity(i.headerBlock),
                i.headerBlock))
        return value;
    if (iter_access_out_of_bounds(i.item,
                pnvmValueItemCount(i.headerBlock) + 1,
                i.headerBlock))
        return value;

    unsigned int bc;
    if (i.at_end()) {
        bc = pnvmValueBlockCount(i.headerBlock);
        pnvmValueIncItemCount(i.headerBlock);
        if (bc < pnvmValueBlockCount(i.headerBlock)) {
            if (i.headerBlock == i.currentBlock) {
                i.currentBlock = pnvmBlockAppendBlock(i.headerBlock);
            }else {
                i.currentBlock = pnvmBlockAppendBlock(
                        (typename T::tail_type_t *)i.currentBlock);
            }
        }
    }
    return *i = value;
}

template <typename T>
typename T::item_type_t const & pnvmIterValue(PNVMIterator<T> const & i)
{
    iter_access_out_of_bounds(i.item,
            pnvmValueItemCount(i.headerBlock),
            i.headerBlock);
    return *i;
}

template <typename T>
typename T::item_type_t & pnvmIterValue(PNVMIterator<T> & i)
{
    iter_access_out_of_bounds(i.item,
            pnvmValueItemCount(i.headerBlock),
            i.headerBlock);
    return *i;
}

template <typename T>
typename T::item_type_t & pnvmIterWrite(
        PNVMIterator<T> & i,
        typename T::item_type_t & item)
{
    pnvmIterSetItem(i, item);
    return pnvmIterNext(i);
}

template <typename T>
PNVMIterator<T> pnvmIterNew(T * headerBlock) {
    return PNVMIterator<T>(headerBlock, headerBlock, 0);
}

#endif /* end of include guard: PNVM_ITERATOR_H */
