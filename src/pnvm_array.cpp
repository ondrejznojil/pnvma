#if !defined(TARGET_ARDUINO) && DEBUG
    #include <iostream>
#endif
#include "pnvm_array.h"

PNVMArrayHead * pnvmArrayNew() {
    PNVMArrayHead * res = new PNVMArrayHead;
#if !defined(TARGET_ARDUINO) && DEBUG
    LOG4CXX_DEBUG(memlog, "creating array " << *res)
#endif
    return res;
}

PNVMToken & pnvmIterSetItem(PNVMArrayIter & i, PNVMToken & value) {
    PNVMToken tmp = pnvmTokenClone(value);
    return pnvmIterSetItem<PNVMArrayHead>(i, tmp);
}

PNVMToken & pnvmIterWrite(PNVMArrayIter & i, PNVMToken & value) {
    pnvmIterSetItem(i, value);
    return pnvmIterNext(i);
}

#if !defined(TARGET_ARDUINO) && DEBUG

std::ostream & operator << (std::ostream &s, PNVMArrayHead const &hb) {
    s << "PNVMArray(" << &hb << ", rc=" << hb.ref_count()
      << ", ic=" << hb.item_count()
      << ", bc=" << pnvmValueBlockCount(&hb) << ")";
    return s;
}

#endif
