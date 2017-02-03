#if !defined(TARGET_ARDUINO) && DEBUG
    #include <iostream>
#endif
#include "pnvm_plugin.h"
#include "pnvm_string.h"

ItemCount_t s_max_item_count() {
    return -1;
}

StringHeader::StringHeader(Signature_t sig) {
    set_signature(sig);
    set_ref_count(0);
    set_item_count(0);
};

ItemCount_t StringHeader::set_item_count(ItemCount_t ic) {
    if (ic > s_max_item_count()) {
        dieWithmsg(ErrorCollectionFull,
                "can not set item count higher, than max capacity for string");
        return value.item_count;
    }else {
        return value.item_count = ic;
    }
}

ItemCount_t StringHeader::inc_item_count() {
    ItemCount_t res = item_count() + 1;
    set_item_count(res);
    return item_count();
}

ItemCount_t StringHeader::dec_item_count() {
    ItemCount_t res = item_count() - 1;
    set_item_count(res);
    return item_count();
}

RefCount_t StringHeader::inc_ref_count() {
    RefCount_t res = ref_count() + 1;
    set_ref_count(res);
    return res;
}

RefCount_t StringHeader::dec_ref_count() {
    RefCount_t res = ref_count() - 1;
    set_ref_count(res);
    return res;
}


PNVMStringHead * pnvmStringNew() {
    PNVMStringHead * res = new PNVMStringHead;
#if !defined(TARGET_ARDUINO) && DEBUG
    LOG4CXX_DEBUG(memlog, "creating string " << (*res))
#endif
    return res;
}

char & pnvmIterSetItem(PNVMStringIter & i, char c) {
    return pnvmIterSetItem<PNVMStringHead>(i, c);
}

char & pnvmIterWrite(PNVMStringIter & i, char c) {
    pnvmIterSetItem<PNVMStringHead>(i, c);
    return pnvmIterNext(i);
}

#if !defined(TARGET_ARDUINO) && DEBUG

std::ostream & operator << (std::ostream &s, PNVMStringHead const &hb) {
    char * value = primitiveStringToCStr(&hb);
    s << "PNVMString(" << &hb << ", rc=" << hb.ref_count()
      << ", len=" << hb.item_count()
      << ", bc=" << pnvmValueBlockCount(&hb)
      << ", \"" << value << "\")";
    free(value);
    return s;
}

#endif
