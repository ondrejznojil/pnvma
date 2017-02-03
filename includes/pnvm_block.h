/******************************************************************************
 * Base classes for block values.
 *
 * Each dynamically allocated value inherits from PNVMHeaderBlock declared
 * here. Value is composed of one header block and zero or more tail blocks.
 * Each header block contains a metadata at the beginning in Header structrure.
 *
 * Indexable data types can have tail blocks and must provide operator[] for
 * access to their items. Not indexable data types have only one header block.
 *
 * Collectable data types contain reference counter.
 *
 * @author Michal Minar <xminar06@stud.fit.vutbr.cz>
 * @year 2013
 */
#ifndef PNVM_BLOCK_H
#define PNVM_BLOCK_H

#include "pnvm_config.h"
#include "pnvm_iterator.h"
#include "pnvm_type_interface.h"

/******************************************************************************
 * Common block headers
 */
/*!
 * This is a generic metadata header for indexable and collectable data types.
 */
struct HeaderIC  {
    struct {
#if BLOCK_SIZE <= 16
        /*  |0  3|4   13|14  23| for arduino
         *  +----+------+------+
         *  |SIG |REFCNT|ITMCNT|
         *  +----+------+------+
         */
        RefCount_t signature     : (SignatureBitSize);
        RefCount_t ref_count     : (RefCountBitSize);
        RefCount_t item_count_lo : sizeof(RefCount_t)*8 - (
                (SignatureBitSize + RefCountBitSize));
        unsigned char item_count_hi;
#else
        Signature_t signature;
        RefCount_t  ref_count;
        ItemCount_t item_count;
#endif
    } value;

    static ItemCount_t s_max_item_count() {
        return
#if BLOCK_SIZE <= 16
            (1 << (
                (sizeof(RefCount_t)*8 - (
                    SignatureBitSize + RefCountBitSize))
               + sizeof(char)*8)) - 1;
#else
            -1;
#endif
    }

    explicit HeaderIC(Signature_t sig);

    Signature_t signature() const {
        return (Signature_t) value.signature;
    }

    void set_signature(Signature_t sig) {
        value.signature = sig;
    }

    RefCount_t ref_count() const {
        return value.ref_count;
    }
    
    void set_ref_count(RefCount_t rc) {
        value.ref_count = rc;
    }

    ItemCount_t item_count() const;

    ItemCount_t set_item_count(ItemCount_t item_count);

    ItemCount_t inc_item_count();

    ItemCount_t dec_item_count();

    RefCount_t inc_ref_count();

    RefCount_t dec_ref_count();
};

/******************************************************************************
 * Generic metadata header for indexable data types.
 */
struct HeaderI {
#if BLOCK_SIZE <= 16
    ItemCount_t value_sig : SignatureBitSize;
    ItemCount_t value_ic  : (sizeof(ItemCount_t)*8 - (SignatureBitSize));
#else
    Signature_t value_sig;
    ItemCount_t value_ic;
#endif

    static unsigned int s_max_item_count() {
        return
#if BLOCK_SIZE <= 16
            (1 << (sizeof(ItemCount_t)*8 - SignatureBitSize)) - 1;
#else
            -1;
#endif
    }

    explicit HeaderI(Signature_t sig): value_sig(sig), value_ic(0) {};

    Signature_t signature() const {
        return (Signature_t) value_sig;
    }

    void set_signature(Signature_t sig) {
        value_sig = sig;
    }

    ItemCount_t item_count() const {
        return value_ic;
    }

    ItemCount_t set_item_count(ItemCount_t item_count);

    ItemCount_t inc_item_count() {
        return set_item_count(item_count() + 1);
    }

    ItemCount_t dec_item_count() {
        return set_item_count(item_count() - 1);
    }

};

/******************************************************************************
 * Main classes
 */
struct PNVMTailBlock;

/*!
 * The main interface of Header block.
 *
 * Each header block must contain at least signature() for introspection.
 */
struct PNVMHeaderBlock {
    typedef PNVMTailBlock tail_type_t;
    typedef void * item_type_t;
    typedef PNVMIterator<PNVMHeaderBlock> iterator;

    static bool const s_is_head = true;

    Signature_t signature() const {
        return (Signature_t) reinterpret_cast<HeaderI const *>
            (this)->value_sig;
    }

    tail_type_t const * tail() const;
    tail_type_t * tail();

#ifdef USE_MEMBLOCK
    /**************************************************************************
     * Use memblock allocator.
     */
    void *operator new(size_t size) throw();
    void operator delete(void *ptr);
#endif

};


/*!
 * The main interface of Tail block.
 */
struct PNVMTailBlock {
    typedef PNVMTailBlock tail_type_t;
    typedef void * item_type_t;

    static bool const s_is_head = false;

#ifdef USE_MEMBLOCK
    /**************************************************************************
     * Use memblock allocator.
     */
    void *operator new(size_t size) throw();
    void operator delete(void *ptr);
#endif

    tail_type_t const * tail() const;
    tail_type_t * tail();
};

/******************************************************************************
 * Forward declarations of non-template utilities
 */
bool block_index_operator_out_of_bounds(int index,
        ItemCount_t last_capacity);

/*!
 * This is a generic template for Header block that should not be instantiated.
 * It should be specialized for indexable and not indexable data types.
 */
template <
    Signature_t Signature,      //!< Data type signature.
    bool Indexable,             //!< Whether the data type is indexable.
    bool Collectable,           //!< Whether the data type is collectable.
    typename Header,            //!< Metadata header.
    typename Item               //!< Item type for indexable data types.
>
struct _Head : Header {
    //ASSERTM(false, Indexable_not_0_or_1);
};

template <
    Signature_t Signature,
    bool Collectable,
    typename Header,
    typename Item
>
struct _Tail : PNVMTailBlock {
    typedef _Head<Signature, true, Collectable, Header, Item> head_type_t;
    typedef _Tail<Signature, Collectable, Header, Item> tail_type_t;
    typedef Item item_type_t;

    /**************************************************************************
     * Class constants
     */
    static bool const  s_collectable = Collectable;
    static bool const  s_indexable = true;
    static bool const  s_is_head = false;
    static Signature_t const s_signature = Signature;
    static unsigned const s_item_size = sizeof(Item);

    /**************************************************************************
     * Class functions
     */
    static unsigned s_not_last_capacity() {
        return (AllocationBlockSize - PointerSize)/s_item_size;
    }
    static unsigned s_last_capacity() {
        return AllocationBlockSize/s_item_size;
    }

    /**************************************************************************
     * Attributes
     */
    union {
        struct {
            unsigned char items[AllocationBlockSize];
        }last;
        struct {
            unsigned char items[AllocationBlockSize - PointerSize];
            tail_type_t * tail;
        }not_last;
    }data;

    /**************************************************************************
     * Interface
     */
    tail_type_t const * tail() const {
        return data.not_last.tail;
    }
    tail_type_t * tail() {
        return data.not_last.tail;
    }
    tail_type_t * set_tail(tail_type_t *tail) {
        return data.not_last.tail = tail;
    }

    Item const & operator[] (int index) const {
        Item const *ptr = reinterpret_cast<Item const *>(
                    data.last.items);
        if (!block_index_operator_out_of_bounds(index, s_last_capacity())) {
            return *(ptr + index);
        }else {
            return *ptr;
        }
    }
    Item & operator[] (int index) {
        Item *ptr = reinterpret_cast<Item *>(data.last.items);
        if (!block_index_operator_out_of_bounds(index, s_last_capacity())) {
            return *(ptr + index);
        }else {
            return *ptr;
        }
    }

};

/*!
 * This is utility template class declaring both Head and Tail blocks for
 * particular data type at once.
 */
template <
    Signature_t Signature,
    bool Indexable,
    bool Collectable,
    typename Header,
    typename Item
>
struct PNVMBlock {

    typedef _Head<Signature, Indexable, Collectable, Header, Item> Head;
    typedef _Tail<Signature, Collectable, Header, Item> Tail;

};

/**
 * Header block for indexable types
 */
template <
    Signature_t Signature,
    bool Collectable,
    typename Header,
    typename Item
>
struct _Head<Signature, true, Collectable, Header, Item> :
        PNVMHeaderBlock, Header 
{
    /**********************************************************************
     * Typedefs
     */
    typedef _Head<Signature, true, Collectable, Header, Item> head_type_t;
    typedef typename PNVMBlock<
        Signature,
        true,
        Collectable,
        Header,
        Item>::Tail tail_type_t;
    typedef Item item_type_t;
    typedef PNVMIterator<
        _Head<Signature, true, Collectable, Header, Item>
    > iterator;

    /**********************************************************************
     * Class constants
     */
    static bool const s_indexable = true;
    static bool const s_collectable = Collectable;
    static bool const s_is_head = true;
    static Signature_t const s_signature = (Signature_t) Signature;
    static unsigned const s_header_size = sizeof(Header);
    static unsigned const s_item_size = sizeof(Item);

    /**************************************************************************
     * Class functions
     */
    static unsigned s_not_last_capacity() {
        return (AllocationBlockSize - s_header_size
#ifndef TARGET_ARDUINO
                // on arduino data won't be aligned to pointer size
                - ((AllocationBlockSize - s_header_size)%PointerSize)
#endif
                - PointerSize)/s_item_size;
    }
    static unsigned s_last_capacity() {
        return (AllocationBlockSize
#ifndef TARGET_ARDUINO
                - ((AllocationBlockSize - s_header_size)%PointerSize)
#endif
                - s_header_size)/s_item_size;
    }

    /**********************************************************************
     * Attributes
     */
    union {
        struct {
            unsigned char items[AllocationBlockSize
#ifndef TARGET_ARDUINO
                // on arduino data won't be aligned to pointer size
                - ((AllocationBlockSize - s_header_size)%PointerSize)
#endif
                - s_header_size];
        }last;
        struct {
            unsigned char items[
                (AllocationBlockSize - s_header_size)
#ifndef TARGET_ARDUINO
                // pointer is aligned at the multiples of its size
                // TODO: should use alignof here
                - ((AllocationBlockSize - s_header_size)%PointerSize)
#endif
                - PointerSize];
            tail_type_t * tail;
        }not_last;
    }data;

    /**********************************************************************
     * Constructors
     */
    explicit _Head(): Header(s_signature) {}

    /**********************************************************************
     * Methods
     */
    tail_type_t const * tail() const {
        return data.not_last.tail;
    }
    tail_type_t * tail() {
        return data.not_last.tail;
    }
    tail_type_t * set_tail(tail_type_t *tail) {
        return data.not_last.tail = tail;
    }

    Item const & operator[] (int index) const {
        Item const *ptr = reinterpret_cast<Item const *>(
                    data.last.items);
        if (!block_index_operator_out_of_bounds(index, s_last_capacity())) {
            return *(ptr + index);
        }else {
            return *ptr;
        }
    }
    Item & operator[] (int index) {
        Item *ptr = reinterpret_cast<Item *>(data.last.items);
        if (!block_index_operator_out_of_bounds(index, s_last_capacity())) {
            return *(ptr + index);
        }else {
            return *ptr;
        }
    }

};

/**
 * Header block for non-indexable types
 */
template <
    Signature_t Signature,
    bool Collectable,
    typename Header,
    typename Item
>
struct _Head<Signature, false, Collectable, Header, Item> :
        Header, PNVMHeaderBlock
{
    typedef _Head<Signature, false, Collectable, Header, Item> head_type_t;

    static bool const  s_indexable = false;
    static bool const  s_collectable = Collectable;
    static Signature_t const s_signature = Signature;
    static unsigned const s_header_size = sizeof(Header);

    unsigned char padding[AllocationBlockSize
        // padding will be aligned to the sizeof Pointer
        - ((AllocationBlockSize - sizeof(Header))%PointerSize)
        - sizeof(Header)];
};

#endif /* end of include guard: PNVM_BLOCK_H */
