#if !defined(TARGET_ARDUINO) && DEBUG
    #include <iostream>
#endif
#include "pnvm_tuple.h"

#ifndef TARGET_ARDUINO
    PNVMTupleHead * pnvmTupleNew() {
        PNVMTupleHead * res = new PNVMTupleHead;
    #if !defined(TARGET_ARDUINO) && DEBUG
        LOG4CXX_DEBUG(memlog, "creating tuple " << (*res))
    #endif
        return res;
    }

    PNVMToken & pnvmIterSetItem(PNVMTupleIter & i, PNVMToken & value) {
        PNVMToken tmp = pnvmTokenClone(value);
        return pnvmIterSetItem<PNVMTupleHead>(i, tmp);
    }

    PNVMToken & pnvmIterWrite(PNVMTupleIter & i, PNVMToken & value) {
        pnvmIterSetItem(i, value);
        return pnvmIterNext(i);
    }

    #if !defined(TARGET_ARDUINO) && DEBUG

    std::ostream & operator << (std::ostream &s, PNVMTupleHead const &hb) {
        s << "PNVMTuple(" << &hb << ", rc=" << hb.ref_count()
          << ", ic=" << hb.item_count()
          << ", bc=" << pnvmValueBlockCount(&hb) << ")";
        return s;
    }

    #endif

#endif
