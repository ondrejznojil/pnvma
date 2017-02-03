/******************************************************************************
 * String data type. Indexable and collectable. Contains characters.
 *
 * @author Michal Minar <xminar06@stud.fit.vutbr.cz>
 * @year 2013
 */
#ifndef PNVM_STRING_H
#define PNVM_STRING_H

#if !defined(TARGET_ARDUINO) && DEBUG
    #include <ostream>
#endif
#include "pnvm_block.h"

struct StringHeader  {
    struct {
#if BLOCK_SIZE <= 16
        RefCount_t signature : (SignatureBitSize);
        RefCount_t ref_count : (RefCountBitSize);
#else
        Signature_t signature;
        RefCount_t  ref_count;
#endif
        ItemCount_t item_count;
    } value;

    static ItemCount_t s_max_item_count() {
        return -1;
    }

    explicit StringHeader(Signature_t sig);

    Signature_t signature() const;

    void set_signature(Signature_t sig) {
        value.signature = sig;
    }

    RefCount_t ref_count() const {
        return value.ref_count;
    }
    
    void set_ref_count(RefCount_t rc) {
        value.ref_count = rc;
    }

    ItemCount_t item_count() const {
        return value.item_count;
    }

    ItemCount_t set_item_count(ItemCount_t item_count);

    ItemCount_t inc_item_count();

    ItemCount_t dec_item_count();

    RefCount_t inc_ref_count();

    RefCount_t dec_ref_count();

};

typedef PNVMBlock<SIGN_STRING, true, true, StringHeader, char>::Head PNVMStringHead;
typedef PNVMBlock<SIGN_STRING, true, true, StringHeader, char>::Tail PNVMStringTail;
typedef PNVMIterator<PNVMStringHead> PNVMStringIter;

/******************************************************************************
 * Factories
 */
PNVMStringHead * pnvmStringNew();

/******************************************************************************
 * Items interface
 */
template<>
inline void pnvm_item_reinitialize<char>(char &i) {
    i = 0;
}

/******************************************************************************
 * Iterator interface
 */
char & pnvmIterSetItem(PNVMStringIter & i, char c);

char & pnvmIterWrite(PNVMStringIter & i, char c);

#if !defined(TARGET_ARDUINO) && DEBUG
    std::ostream & operator << (std::ostream &s, PNVMStringHead const &);
#endif

#endif /* end of include guard: PNVM_STRING_H */
