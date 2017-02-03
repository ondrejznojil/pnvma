#include "pnvm_generic_block.h"

#ifdef TARGET_ARDUINO
    #define CASE_TUPLE(body) \
        case SIGN_TUPLE: body;
#else
    #define CASE_TUPLE(body) \
        case SIGN_TUPLE: body; \
        case SIGN_ARRAY: body;
#endif
    
#define _get_capacity(prefix) \
    template <> \
    unsigned int prefix ## Capacity <PNVMHeaderBlock>( \
            PNVMHeaderBlock const *block) { \
        switch (block->signature()) { \
            case SIGN_STRING: \
                return prefix ## Capacity <PNVMStringHead>( \
                    reinterpret_cast<PNVMStringHead const *>(block)); \
            CASE_TUPLE(return prefix ## Capacity <PNVMTupleHead>( \
                    reinterpret_cast<PNVMTupleHead const *>(block))) \
            case SIGN_NET_INSTANCE: \
                return prefix ## Capacity <PNVMNetInstHead>( \
                    reinterpret_cast<PNVMNetInstHead const *>(block)); \
            case SIGN_PLACE: \
                return prefix ## Capacity <PNVMPlaceHead>( \
                    reinterpret_cast<PNVMPlaceHead const *>(block)); \
            default: \
                return 0; \
        } \
        return 0; \
    }

_get_capacity(pnvmTypeLast);
_get_capacity(pnvmTypeNotLast);
_get_capacity(pnvmTypeTailLast);
_get_capacity(pnvmTypeTailNotLast);
_get_capacity(pnvmTypeTotal);

#define _value_unary_op_const(name, return_type) \
    template <> \
    return_type name <PNVMHeaderBlock>(PNVMHeaderBlock const *block) { \
        switch (block->signature()) { \
            case SIGN_STRING: \
                return name <PNVMStringHead>( \
                    reinterpret_cast<PNVMStringHead const *>(block)); \
            CASE_TUPLE(return name <PNVMTupleHead>( \
                    reinterpret_cast<PNVMTupleHead const *>(block))) \
            case SIGN_NET_INSTANCE: \
                return name <PNVMNetInstHead>( \
                    reinterpret_cast<PNVMNetInstHead const *>(block)); \
            case SIGN_PLACE: \
                return name <PNVMPlaceHead>( \
                    reinterpret_cast<PNVMPlaceHead const *>(block)); \
            default: \
                return 0; \
        } \
        return 0; \
    }

_value_unary_op_const(pnvmValueItemCount, ItemCount_t);
_value_unary_op_const(pnvmValueBlockCount, unsigned int);

template <>
PNVMHeaderBlock * pnvmValueSetItemCount<PNVMHeaderBlock>(
        PNVMHeaderBlock *block,
        unsigned int count)
{
    switch (block->signature()) {
        case SIGN_STRING:
            return pnvmValueSetItemCount<PNVMStringHead>(
                    reinterpret_cast<PNVMStringHead *>(block), count);
#ifndef TARGET_ARDUINO
        case SIGN_TUPLE:
            return pnvmValueSetItemCount<PNVMTupleHead>(
                    reinterpret_cast<PNVMTupleHead *>(block), count);
#endif
        case SIGN_ARRAY:
            return pnvmValueSetItemCount<PNVMArrayHead>(
                    reinterpret_cast<PNVMArrayHead *>(block), count);
        case SIGN_NET_INSTANCE:
            return pnvmValueSetItemCount<PNVMNetInstHead>(
                    reinterpret_cast<PNVMNetInstHead *>(block), count);
        case SIGN_PLACE:
            return pnvmValueSetItemCount<PNVMPlaceHead>(
                    reinterpret_cast<PNVMPlaceHead *>(block), count);
        default:
            return NULL;
    }
    return NULL;
}

template <>
unsigned int pnvmValueTotalCapacityAt<PNVMHeaderBlock>(
        PNVMHeaderBlock const *block, int index)
{
    switch (block->signature()) {
        case SIGN_STRING:
            return pnvmValueTotalCapacityAt<PNVMStringHead>(
                    reinterpret_cast<PNVMStringHead const *>(block), index);
#ifndef TARGET_ARDUINO
        case SIGN_TUPLE:
            return pnvmValueTotalCapacityAt<PNVMTupleHead>(
                    reinterpret_cast<PNVMTupleHead const *>(block), index);
#endif
        case SIGN_ARRAY:
            return pnvmValueTotalCapacityAt<PNVMArrayHead>(
                    reinterpret_cast<PNVMArrayHead const *>(block), index);
        case SIGN_NET_INSTANCE:
            return pnvmValueTotalCapacityAt<PNVMNetInstHead>(
                    reinterpret_cast<PNVMNetInstHead const *>(block), index);
        case SIGN_PLACE:
            return pnvmValueTotalCapacityAt<PNVMPlaceHead>(
                    reinterpret_cast<PNVMPlaceHead const *>(block), index);
        default:
            return 0;
    }
    return 0;
}

template <>
unsigned int pnvmValueIndexOfBlockAtItem<PNVMHeaderBlock>(
        PNVMHeaderBlock const *block, unsigned int index)
{
    switch (block->signature()) {
        case SIGN_STRING:
            return pnvmValueIndexOfBlockAtItem<PNVMStringHead>(
                    reinterpret_cast<PNVMStringHead const *>(block), index);
#ifndef TARGET_ARDUINO
        case SIGN_TUPLE:
            return pnvmValueIndexOfBlockAtItem<PNVMTupleHead>(
                    reinterpret_cast<PNVMTupleHead const *>(block), index);
#endif
        case SIGN_ARRAY:
            return pnvmValueIndexOfBlockAtItem<PNVMArrayHead>(
                    reinterpret_cast<PNVMArrayHead const *>(block), index);
        case SIGN_NET_INSTANCE:
            return pnvmValueIndexOfBlockAtItem<PNVMNetInstHead>(
                    reinterpret_cast<PNVMNetInstHead const *>(block), index);
        case SIGN_PLACE:
            return pnvmValueIndexOfBlockAtItem<PNVMPlaceHead>(
                    reinterpret_cast<PNVMPlaceHead const *>(block), index);
        default:
            return 0;
    }
    return 0;
}

