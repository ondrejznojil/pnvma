#include "pnvm_generic_iterator.h"

PNVMItemIterator & PNVMItemIterator::operator ++ () {
    switch (base_iterator_t::headerBlock->signature()) {
        case SIGN_STRING:
            reinterpret_cast<PNVMStringIter *>(this)->operator++();
            break;
#ifndef TARGET_ARDUINO
        case SIGN_TUPLE:
            reinterpret_cast<PNVMTupleIter *>(this)->operator++();
            break;
#endif
        case SIGN_ARRAY:
            reinterpret_cast<PNVMArrayIter *>(this)->operator++();
            break;
        case SIGN_NET_INSTANCE:
            reinterpret_cast<PNVMNetInstIter *>(this)->operator++();
            break;
        case SIGN_PLACE:
            reinterpret_cast<PNVMPlaceIter *>(this)->operator++();
            break;
        default:
            assert(false);
    }
    return *this;
}

PNVMItemIterator PNVMItemIterator::operator ++ (int) {
    switch (base_iterator_t::headerBlock->signature()) {
        case SIGN_STRING:
            return reinterpret_cast<PNVMStringIter *>(this)->operator++(1);
#ifndef TARGET_ARDUINO
        case SIGN_TUPLE:
            return reinterpret_cast<PNVMTupleIter *>(this)->operator++(1);
#endif
        case SIGN_ARRAY:
            return reinterpret_cast<PNVMArrayIter *>(this)->operator++(1);
        case SIGN_NET_INSTANCE:
            return reinterpret_cast<PNVMNetInstIter *>(this)->operator++(1);
        case SIGN_PLACE:
            return reinterpret_cast<PNVMPlaceIter *>(this)->operator++(1);
        default:
            assert(false);
    }
    return PNVMItemIterator(NULL, NULL, 0);
}

void const * PNVMItemIterator::operator * () const {
    switch (base_iterator_t::headerBlock->signature()) {
        case SIGN_STRING:
            return (void *) &reinterpret_cast<PNVMStringIter const *>
                (this)->operator *();
#ifndef TARGET_ARDUINO
        case SIGN_TUPLE:
            return (void *) &reinterpret_cast<PNVMTupleIter const *>
                (this)->operator *();
#endif
        case SIGN_ARRAY:
            return (void *) &reinterpret_cast<PNVMArrayIter const *>
                (this)->operator *();
        case SIGN_NET_INSTANCE:
            return (void *) reinterpret_cast<PNVMNetInstIter const *>
                (this)->operator *();
            break;
        case SIGN_PLACE:
            return (void *) &reinterpret_cast<PNVMPlaceIter const *>
                (this)->operator *();
            break;
        default:
            assert(false);
    }
    return NULL;
}

void * PNVMItemIterator::operator * () {
    switch (base_iterator_t::headerBlock->signature()) {
        case SIGN_STRING:
            return (void *) &reinterpret_cast<PNVMStringIter *>
                (this)->operator *();
#ifndef TARGET_ARDUINO
        case SIGN_TUPLE:
            return (void *) &reinterpret_cast<PNVMTupleIter *>
                (this)->operator *();
#endif
        case SIGN_ARRAY:
            return (void *) &reinterpret_cast<PNVMArrayIter *>
                (this)->operator *();
        case SIGN_NET_INSTANCE:
            return (void *) reinterpret_cast<PNVMNetInstIter *>
                (this)->operator *();
        case SIGN_PLACE:
            return (void *) &reinterpret_cast<PNVMPlaceIter *>
                (this)->operator *();
        default:
            assert(false);
    }
    return NULL;
}

bool PNVMItemIterator::at_end() const { 
    switch (base_iterator_t::headerBlock->signature()) {
        case SIGN_STRING:
            return reinterpret_cast<PNVMStringIter const *>(this)->at_end();
#ifndef TARGET_ARDUINO
        case SIGN_TUPLE:
            return reinterpret_cast<PNVMTupleIter const *>(this)->at_end();
#endif
        case SIGN_ARRAY:
            return reinterpret_cast<PNVMArrayIter const *>(this)->at_end();
        case SIGN_NET_INSTANCE:
            return reinterpret_cast<PNVMNetInstIter const *>
                (this)->at_end();
        case SIGN_PLACE:
            return reinterpret_cast<PNVMPlaceIter const *>
                (this)->at_end();
        default:
            assert(false);
    }
    return true;
}

bool PNVMItemIterator::at_last_block() const {
    switch (base_iterator_t::headerBlock->signature()) {
        case SIGN_STRING:
            return reinterpret_cast<PNVMStringIter const *>
                (this)->at_last_block();
#ifndef TARGET_ARDUINO
        case SIGN_TUPLE:
            return reinterpret_cast<PNVMTupleIter const *>
                (this)->at_last_block();
#endif
        case SIGN_ARRAY:
            return reinterpret_cast<PNVMArrayIter const *>
                (this)->at_last_block();
        case SIGN_NET_INSTANCE:
            return reinterpret_cast<PNVMNetInstIter const *>
                (this)->at_last_block();
         case SIGN_PLACE:
            return reinterpret_cast<PNVMPlaceIter const *>
                (this)->at_last_block();
        default:
            assert(false);
    }
    return true;
}

int PNVMItemIterator::block_index() const {
    switch (base_iterator_t::headerBlock->signature()) {
        case SIGN_STRING:
            return reinterpret_cast<PNVMStringIter const *>
                (this)->block_index();
#ifndef TARGET_ARDUINO
        case SIGN_TUPLE:
            return reinterpret_cast<PNVMTupleIter const *>
                (this)->block_index();
#endif
        case SIGN_ARRAY:
            return reinterpret_cast<PNVMArrayIter const *>
                (this)->block_index();
        case SIGN_NET_INSTANCE:
            return reinterpret_cast<PNVMNetInstIter const *>
                (this)->block_index();
        case SIGN_PLACE:
            return reinterpret_cast<PNVMPlaceIter const *>
                (this)->block_index();
        default:
            assert(false);
    }
    return 0;
}

bool PNVMItemIterator::at_block_end() const {
    switch (base_iterator_t::headerBlock->signature()) {
        case SIGN_STRING:
            return reinterpret_cast<PNVMStringIter const *>
                (this)->at_block_end();
#ifndef TARGET_ARDUINO
        case SIGN_TUPLE:
            return reinterpret_cast<PNVMTupleIter const *>
                (this)->at_block_end();
#endif
        case SIGN_ARRAY:
            return reinterpret_cast<PNVMArrayIter const *>
                (this)->at_block_end();
        case SIGN_NET_INSTANCE:
            return reinterpret_cast<PNVMNetInstIter const *>
                (this)->at_block_end();
        case SIGN_PLACE:
            return reinterpret_cast<PNVMPlaceIter const *>
                (this)->at_block_end();
        default:
            assert(false);
    }
    return true;
}

void PNVMItemIterator::next_block() {
    switch (base_iterator_t::headerBlock->signature()) {
        case SIGN_STRING:
            reinterpret_cast<PNVMStringIter *>(this)->next_block();
            break;
#ifndef TARGET_ARDUINO
        case SIGN_TUPLE:
            reinterpret_cast<PNVMTupleIter *>(this)->next_block();
            break;
#endif
        case SIGN_ARRAY:
            reinterpret_cast<PNVMArrayIter *>(this)->next_block();
            break;
        case SIGN_NET_INSTANCE:
            reinterpret_cast<PNVMNetInstIter *>(this)->next_block();
            break;
        case SIGN_PLACE:
            reinterpret_cast<PNVMPlaceIter *>(this)->next_block();
            break;
        default:
            assert(false);
    }
}

PNVMStringIter & pnvmIterString(PNVMItemIterator &i) {
    return *reinterpret_cast<PNVMStringIter *>(&i);
}
PNVMArrayIter & pnvmIterArray(PNVMItemIterator &i) {
    return *reinterpret_cast<PNVMArrayIter *>(&i);
}
#ifdef TARGET_ARDUINO
    #define pnvmIterTuple(i) pnvmIterArray(i)
#else
    PNVMTupleIter & pnvmIterTuple(PNVMItemIterator &i) {
        return *reinterpret_cast<PNVMTupleIter *>(&i);
    }
#endif

PNVMNetInstIter & pnvmIterNetInst(PNVMItemIterator &i) {
    return *reinterpret_cast<PNVMNetInstIter *>(&i);
}
PNVMPlaceIter & pnvmIterPlace(PNVMItemIterator &i) {
    return *reinterpret_cast<PNVMPlaceIter *>(&i);
}

