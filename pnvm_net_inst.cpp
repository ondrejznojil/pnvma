#if !defined(TARGET_ARDUINO) && DEBUG
    #include <iostream>
#endif
#include "pnvm_net_inst.h"
#include "pnvm_plugin.h"
#include "pnvm_utils.h"

PNVMNetInstHead * pnvmNetInstNew(PNVMTemplate * tmpl) {
    PNVMNetInstHead * inst = new PNVMNetInstHead;
    inst->tmpl = tmpl;
#if !defined(TARGET_ARDUINO) && DEBUG
    LOG4CXX_DEBUG(memlog, "creating net instance from template "
            << (*tmpl) << ": " << (*inst))
#endif
    return inst;
}

PNVMNetInstHead * pnvmNetInstNewwithPlace(
        PNVMTemplate * tmpl,
        unsigned int place_count)
{
    PNVMNetInstHead * inst = new PNVMNetInstHead;
    inst->tmpl = tmpl;
    inst->set_item_count(place_count);
    return inst;
}

PNVMTemplate * pnvmNetInstTemplate(PNVMNetInstHead const *inst) {
    return inst->tmpl;
}

template <>
void pnvmValueDelete<PNVMNetInstHead>(PNVMNetInstHead * block) {
    #if !defined(TARGET_ARDUINO) && DEBUG
        LOG4CXX_DEBUG(memlog, "deleting " << (*block))
    #endif
    unmanageNetInstance(block);
    _ValueIterativeDelete<PNVMNetInstHead, true>()(block);
}

#if !defined(TARGET_ARDUINO) && DEBUG

std::ostream & operator << (std::ostream &s, PNVMNetInstHead const &hb) {
    s << "PNVMNetInst(" << &hb << ", rc=" << hb.ref_count()
      << ", ic=" << hb.item_count()
      << ", bc=" << pnvmValueBlockCount(&hb)
      << ", tmpl=" << asPlainString(hb.tmpl->name) << ")";
    return s;
}

#endif
