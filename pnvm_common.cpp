#ifndef PNVM_COMMON_H
#define PNVM_COMMON_H

#if !defined(TARGET_ARDUINO) && DEBUG
    #include <iostream>
    #include <ostream>
    #include <string>
    using std::string;
#endif
#ifdef USE_MEMBLOCK
    #include "memblock_lib.h"
#endif
#include "pnvm_types.h"
#include "pnvm_plugin.h"
#include "pnvm_utils.h"

/******************************************************************************
 * Constants
 */
char const type_indexable_flags[] = \
    { true , true , true , true , true , false, false};
char const type_collectable_flags[] = \
    { true , true , true , false, true , false, false};

char const * const signature_names[] = \
    {"String",
#ifdef TARGET_ARDUINO
        "Tuple",
#else
       NULL,
#endif
       "Array", "Place", "NetInst", "Template", "Event"};

/******************************************************************************
 * Block Headers
 */
HeaderIC::HeaderIC(Signature_t sig) {
    set_signature(sig);
    set_ref_count(0);
    set_item_count(0);
};

ItemCount_t HeaderIC::item_count() const {
#if BLOCK_SIZE <= 16
    ItemCount_t res = value.item_count_hi << (
            sizeof(RefCount_t)*8 - (SignatureBitSize) - (RefCountBitSize));
    res |= value.item_count_lo;
    return res;
#else
    return value.item_count;
#endif
}

ItemCount_t HeaderIC::set_item_count(ItemCount_t ic) {
    if (ic > s_max_item_count()) {
        dieWithmsgwith(ErrorCollectionFull,
                "collection is full -- signature=", signature());
        return item_count();
    }else {
#if BLOCK_SIZE <= 16
        value.item_count_lo = ic;
        value.item_count_hi = ic >> (
                sizeof(RefCount_t)*8 - (SignatureBitSize + RefCountBitSize));
        return ic;
#else
        return value.item_count = ic;
#endif
    }
}

ItemCount_t HeaderIC::inc_item_count() {
    return set_item_count(item_count() + 1);
}

ItemCount_t HeaderIC::dec_item_count() {
    if (item_count() == 0)
        return 0;
    return set_item_count(item_count() - 1);
}

RefCount_t HeaderIC::inc_ref_count() {
    RefCount_t res = ref_count() + 1;
    set_ref_count(res);
    return res;
}

RefCount_t HeaderIC::dec_ref_count() {
    RefCount_t res = ref_count() - 1;
    set_ref_count(res);
    return res;
}

ItemCount_t HeaderI::set_item_count(ItemCount_t ic) {
    if (ic > s_max_item_count()) {
        dieWithmsgwith(ErrorCollectionFull,
                "collection is full -- signature=", signature());
        return value_ic;
    }else {
        return value_ic = ic;
    }
}

/******************************************************************************
 * Block
 */
#ifdef USE_MEMBLOCK
void * PNVMHeaderBlock::operator new(size_t size) throw() {
    if (MEMBLOCK_GetFreeMem() < AllocationBlockSize) {
        LOG4CXX_ERROR(memlog, "block allocation failed");
        dieWithmsg(ErrorNoMemory, "block allocation failed");
        return NULL;
    }
    return MEMBLOCK_Allocate(size);
}
void PNVMHeaderBlock::operator delete(void *ptr) {
    MEMBLOCK_Free(ptr);
}
#endif

#ifdef USE_MEMBLOCK
void * PNVMTailBlock::operator new(size_t size) throw() {
    if (MEMBLOCK_GetFreeMem() < AllocationBlockSize) {
        LOG4CXX_ERROR(memlog, "block allocation failed");
        dieWithmsg(ErrorNoMemory, "block allocation failed");
        return NULL;
    }
    return MEMBLOCK_Allocate(size);
}
void PNVMTailBlock::operator delete(void *ptr) {
    MEMBLOCK_Free(ptr);
}
#endif

PNVMTailBlock const * PNVMHeaderBlock::tail() const {
    return reinterpret_cast<tail_type_t const *>(this)
        + (AllocationBlockSize/PointerSize) - 1;
}

PNVMTailBlock * PNVMHeaderBlock::tail() {
    return reinterpret_cast<tail_type_t *>(this)
        + (AllocationBlockSize/PointerSize) - 1;
}

PNVMTailBlock const * PNVMTailBlock::tail() const {
    return reinterpret_cast<tail_type_t const *>(this)
        + (AllocationBlockSize/PointerSize) - 1;
}

PNVMTailBlock * PNVMTailBlock::tail() {
    return reinterpret_cast<tail_type_t *>(this)
        + (AllocationBlockSize/PointerSize) - 1;
}

void value_reference(PNVMHeaderBlock * block) {
    switch (block->signature()) {
        case SIGN_STRING:
            pnvmValueReference<PNVMStringHead>((PNVMStringHead *) block);
            break;
#ifndef TARGET_ARDUINO
        case SIGN_TUPLE:
            pnvmValueReference<PNVMTupleHead>((PNVMTupleHead *) block);
            break;
#endif
        case SIGN_ARRAY:
            pnvmValueReference<PNVMArrayHead>((PNVMArrayHead *) block);
            break;
        case SIGN_NET_INSTANCE:
            pnvmValueReference<PNVMNetInstHead>((PNVMNetInstHead *) block);
            break;
        default:
            break;
    }
}

void value_unreference(PNVMHeaderBlock * block) {
    switch (block->signature()) {
        case SIGN_STRING:
            pnvmValueUnreference<PNVMStringHead>((PNVMStringHead *) block);
            break;
#ifndef TARGET_ARDUINO
        case SIGN_TUPLE:
            pnvmValueUnreference<PNVMTupleHead>((PNVMTupleHead *) block);
            break;
#endif
        case SIGN_ARRAY:
            pnvmValueUnreference<PNVMArrayHead>((PNVMArrayHead *) block);
            break;
        case SIGN_NET_INSTANCE:
            pnvmValueUnreference<PNVMNetInstHead>((PNVMNetInstHead *) block);
            break;
        default:
            break;
    }
}

bool block_index_operator_out_of_bounds(int index,
        ItemCount_t last_capacity)
{
    return !unlessdieWithmsg(index >= 0 && index < (int) last_capacity,
            ErrorOutOfRange, "index to block out of bounds");
}

/******************************************************************************
 * Iterator
 */
bool iter_is_out_of_bounds(long item, ItemCount_t total_capacity,
        void * hb) {
    return !unlessdieWithmsgwith(
                total_capacity >= ((ItemCount_t) - 1) ||
                    (item < 0 || (unsigned long) item < total_capacity),
                ErrorCollectionFull,
                "iterating out of collection bounds: ",
                * (PNVMHeaderBlock *) hb);
}

bool iter_access_out_of_bounds(long item, ItemCount_t item_count,
        void * hb) {
    return !unlessdieWithmsgwith(
                item < 0 || (unsigned long) item < item_count,
                ErrorOutOfRange,
                "invalid access to collection: ",
                * (PNVMHeaderBlock *) hb);
}

/******************************************************************************
 * Utilities
 */
/*
void dieWith(int errcode) {
    setError((Error_t) errcode);
#ifdef TARGET_ARDUINO
    Serial.print("fatal: ");
    Serial.println(errcode);
#else
    LOG4CXX_FATAL(exlog, strError((Error_t) errcode))
    exit(EXIT_FAILURE);
#endif
}
*/

#if !defined(TARGET_ARDUINO) && DEBUG

void dieWithmsg(int err, char const * msg) {
    setError((Error_t) err);
    LOG4CXX_FATAL(exlog, strError((Error_t) err) << ": " << msg)
    exit(EXIT_FAILURE);
}

std::ostream & operator << (std::ostream &s, PNVMHeaderBlock const &hb) {
    switch (hb.signature()) {
        case SIGN_STRING:
            return s << (PNVMStringHead const &) hb;
        case SIGN_TUPLE:
            return s << (PNVMTupleHead const &) hb;
        case SIGN_ARRAY:
            return s << (PNVMArrayHead const &) hb;
        case SIGN_NET_INSTANCE:
            return s << (PNVMNetInstHead const &) hb;
        case SIGN_PLACE:
            return s << (PNVMPlaceHead const &) hb;
        case SIGN_EVENT:
            return s << (PNVMEvent const &) hb;
        case SIGN_TEMPLATE:
            return s << (PNVMTemplate const &) hb;
        default:
            return s << "PNVMHeaderBlock(" << ((int) hb.signature()) << ")";
    }
}

string asPlainString(PNVMStringHead const *str) {
    std::ostringstream o;
    PNVMStringIter i = pnvmValueIterator((PNVMStringHead *)str);
    while (!pnvmIterAtEnd(i)) {
        o << pnvmIterNext(i);
    }
    return o.str();
}

#endif

#endif /* end of include guard: PNVM_COMMON_H */
