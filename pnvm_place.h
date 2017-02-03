/******************************************************************************
 * Place data type. Not collectable. Internal to pnvm simulator.
 *
 * @author Michal Minar <xminar06@stud.fit.vutbr.cz>
 * @year 2013
 */
#ifndef PNVM_PLACE_H
#define PNVM_PLACE_H

#if !defined(TARGET_ARDUINO) && DEBUG
    #include <ostream>
#endif
#include "pnvm_config.h"
#include "pnvm_token.h"

struct PNVMPlaceItem {
    PNVMToken token;
    unsigned int number;
    PNVMPlaceItem() {};
    PNVMPlaceItem(PNVMToken const & t, unsigned int n):
        token(), number(n)
    {
        set_token(t);
    };

    void set_token(PNVMToken const & t) {
        pnvmTokenAssign(token, t);
    }
};

typedef PNVMBlock<SIGN_PLACE, true, false, HeaderI, PNVMPlaceItem>::Head
    PNVMPlaceHead;
typedef PNVMBlock<SIGN_PLACE, true, false, HeaderI, PNVMPlaceItem>::Tail
    PNVMPlaceTail;
typedef PNVMIterator<PNVMPlaceHead> PNVMPlaceIter;

/******************************************************************************
 * Factories
 */
PNVMPlaceHead * pnvmPlaceNew();

/******************************************************************************
 * Items interface
 */
PNVMPlaceItem pnvmPlaceItemNewof(unsigned int number, PNVMToken const &toke);

PNVMToken const & pnvmPlaceItemToken(PNVMPlaceItem const &item);

PNVMToken & pnvmPlaceItemToken(PNVMPlaceItem &item);

unsigned int pnvmPlaceItemNumber(PNVMPlaceItem const &item);

unsigned int pnvmPlaceItemDecBy(PNVMPlaceItem &item, unsigned int amount);

unsigned int pnvmPlaceItemIncBy(PNVMPlaceItem &item, unsigned int amount);

void pnvmPlaceItemSetNumber(PNVMPlaceItem &item, unsigned int amount);

void pnvmPlaceItemSetNumberof(
        PNVMPlaceItem &item,
        unsigned int amount,
        PNVMToken const & token);

PNVMPlaceItem pnvmPlaceItemClone(PNVMPlaceItem const &pi);

PNVMPlaceItem & pnvmPlaceItemMakeInvalid(PNVMPlaceItem &pi);

template <>
inline void pnvm_item_delete<PNVMPlaceItem>(PNVMPlaceItem & item) {
    item.number = 0;
    pnvm_item_delete<PNVMToken>(item.token);
}

template <>
inline void pnvm_item_reinitialize<PNVMPlaceItem>(PNVMPlaceItem & i) {
    i.number = 0;
    pnvm_item_reinitialize<PNVMToken>(i.token);
}

/******************************************************************************
 * Iterator interface
 */
PNVMPlaceItem & pnvmIterSetItem(PNVMPlaceIter & i, PNVMPlaceItem & value);

PNVMPlaceItem & pnvmIterWrite(PNVMPlaceIter & i, PNVMPlaceItem & value);

#if !defined(TARGET_ARDUINO) && DEBUG
    std::ostream & operator << (std::ostream &s, PNVMPlaceHead const &);
#endif

#endif /* end of include guard: PNVM_PLACE_H */
