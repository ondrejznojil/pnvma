#if !defined(TARGET_ARDUINO) && DEBUG
    #include <iostream>
#endif
#include "pnvm_place.h"

PNVMPlaceHead * pnvmPlaceNew() {
    PNVMPlaceHead * res = new PNVMPlaceHead;
#if !defined(TARGET_ARDUINO) && DEBUG
    LOG4CXX_DEBUG(memlog, "creating place " << *res)
#endif
    return res;
}

PNVMPlaceItem pnvmPlaceItemNewof(unsigned int number, PNVMToken const &token)
{
    return PNVMPlaceItem(token, number);
}

PNVMToken const & pnvmPlaceItemToken(PNVMPlaceItem const &item) {
    return item.token;
}

PNVMToken & pnvmPlaceItemToken(PNVMPlaceItem &item) {
    return item.token;
}

unsigned int pnvmPlaceItemNumber(PNVMPlaceItem const &item) {
    return item.number;
}

unsigned int pnvmPlaceItemDecBy(PNVMPlaceItem &item, unsigned int amount) {
    item.number -= amount;
    return item.number;
}

unsigned int pnvmPlaceItemIncBy(PNVMPlaceItem &item, unsigned int amount) {
    item.number += amount;
    return item.number;
}

void pnvmPlaceItemSetNumber(PNVMPlaceItem &item, unsigned int amount) {
    item.number = amount;
}

void pnvmPlaceItemSetNumberof(
        PNVMPlaceItem &item,
        unsigned int amount,
        PNVMToken const & token)
{
    item.number = amount;
    item.set_token(token);
}

PNVMPlaceItem pnvmPlaceItemClone(PNVMPlaceItem const &pi) {
    return PNVMPlaceItem(pi.token, pi.number);
}

PNVMPlaceItem & pnvmPlaceItemMakeInvalid(PNVMPlaceItem &pi) {
    pi.number = 0;
    pnvmTokenMakeInvalid(pi.token);
    return pi;
}

PNVMPlaceItem & pnvmIterSetItem(PNVMPlaceIter & i, PNVMPlaceItem & value) {
    PNVMPlaceItem pi = pnvmPlaceItemClone(value);
    return pnvmIterSetItem<PNVMPlaceHead>(i, pi);
}

PNVMPlaceItem & pnvmIterWrite(PNVMPlaceIter & i, PNVMPlaceItem & value) {
    pnvmIterSetItem(i, value);
    return pnvmIterNext(i);
}

#if !defined(TARGET_ARDUINO) && DEBUG

std::ostream & operator << (std::ostream &s, PNVMPlaceHead const &hb) {
    s << "PNVMPlace(" << &hb
      << ", ic=" << hb.item_count()
      << ", bc=" << pnvmValueBlockCount(&hb) << ")";
    return s;
}

#endif
