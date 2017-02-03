/******************************************************************************
 * Net instance data type. Collectable and indexable. It contains places.
 *
 * @author Michal Minar <xminar06@stud.fit.vutbr.cz>
 * @year 2013
 */
#ifndef PNVM_NET_INST_H
#define PNVM_NET_INST_H

#if !defined(TARGET_ARDUINO) && DEBUG
    #include <ostream>
#endif
#include "pnvm_config.h"
#include "pnvm_place.h"
#include "pnvm_block.h"
#include "pnvm_token.h"
#include "pnvm_template.h"

/******************************************************************************
 * Header block embedded data types.
 */
struct HeaderNetInst : HeaderIC {
    PNVMTemplate * tmpl;

    explicit HeaderNetInst(Signature_t sig=SIGN_NET_INSTANCE):
        HeaderIC(sig),
        tmpl(NULL) {}
};

typedef PNVMBlock<SIGN_NET_INSTANCE, true, true,
        HeaderNetInst, PNVMPlaceHead *>::Head
    PNVMNetInstHead;
typedef PNVMBlock<SIGN_NET_INSTANCE, true, true,
        HeaderNetInst, PNVMPlaceHead *>::Tail
    PNVMNetInstTail;
typedef PNVMIterator<PNVMNetInstHead> PNVMNetInstIter;

/******************************************************************************
 * Forward declarations
 */
void unmanageNetInstance(PNVMNetInstHead *);

/******************************************************************************
 * Factories
 */
PNVMNetInstHead * pnvmNetInstNew(PNVMTemplate * tmpl);

PNVMNetInstHead * pnvmNetInstNewwithPlace(
        PNVMTemplate tmpl,
        unsigned int place_count);

/******************************************************************************
 * Value interface
 */
PNVMTemplate * pnvmNetInstTemplate(PNVMNetInstHead const *inst);

template <>
void pnvmValueDelete<PNVMNetInstHead>(PNVMNetInstHead * block);

/******************************************************************************
 * Items interface
 */
template <>
inline void pnvm_item_delete<PNVMPlaceHead *>(PNVMPlaceHead * & item) {
    pnvmValueDelete(item);
}
template <>
inline void pnvm_item_reinitialize<PNVMPlaceHead *>(PNVMPlaceHead * & p) {
    p = NULL;
}

#if !defined(TARGET_ARDUINO) && DEBUG
    std::ostream & operator << (std::ostream &s, PNVMNetInstHead const &);
#endif

#endif /* end of include guard: PNVM_NET_INST_H */
