/******************************************************************************
 * Event data type.
 *
 * This is data type internal to simulation. It's not modifiable and not
 * collectable. Contains links next and prev for use in double linkes list
 * of calendar queue.
 *
 * @author Michal Minar <xminar06@stud.fit.vutbr.cz>
 * @year 2013
 */
#ifndef PNVM_Event_H
#define PNVM_Event_H

#if !defined(TARGET_ARDUINO) && DEBUG
    #include <ostream>
#endif
#include "pnvm_block.h"
#include "pnvm_net_inst.h"

struct HeaderEvent;

typedef PNVMBlock<SIGN_EVENT, false, false, HeaderEvent, void>::Head PNVMEvent;
            
struct HeaderEvent {
#if BLOCK_SIZE <= 16
    unsigned int value_sig   : 4;
    unsigned int place_id    : 12;
#else
    Signature_t value_sig;
    unsigned int place_id;
#endif
    Time_t time;
    unsigned int number;
    PNVMToken token;
    PNVMNetInstHead * instance;
    PNVMEvent * prev;
    PNVMEvent * next;

    explicit HeaderEvent(Signature_t sig=SIGN_EVENT):
        value_sig(sig),
        place_id(0),
        time(0),
        number(0),
        token(PNVMToken::s_invalid),
        instance(NULL),
        prev(NULL),
        next(NULL) {}
    HeaderEvent(
            unsigned int plcid,
            Time_t tim,
            PNVMNetInstHead * inst,
            unsigned int num,
            PNVMToken const &tok,
            PNVMEvent * prev_event=NULL,
            PNVMEvent * next_event=NULL):
        value_sig(SIGN_EVENT),
        place_id(plcid),
        time(tim),
        number(num),
        token(tok),
        instance(inst),
        prev(prev_event),
        next(next_event) {}

    Signature_t signature() const {
        return (Signature_t) value_sig;
    }

    void set_signature(Signature_t sig) {
        value_sig = sig;
    }

};

/******************************************************************************
 * Factories
 */
PNVMEvent * pnvmEventNewtimestoPlaceofInstscheduledAt(
        PNVMToken const &token,
        unsigned int n,
        unsigned int place_id,
        PNVMNetInstHead * instance,
        Time_t time);

/******************************************************************************
 * Interpreter interface
 */
PNVMNetInstHead const * pnvmEventNetInstance(PNVMEvent const * event);

PNVMNetInstHead * pnvmEventNetInstance(PNVMEvent * event);

PNVMEvent const * pnvmEventNext(PNVMEvent const * event);

PNVMEvent * pnvmEventNext(PNVMEvent * event);

unsigned int pnvmEventNumber(PNVMEvent const * event);

unsigned int pnvmEventPlaceId(PNVMEvent const * event);

PNVMEvent const * pnvmEventPrev(PNVMEvent const * event);

PNVMEvent * pnvmEventPrev(PNVMEvent * event);

Time_t pnvmEventTime(PNVMEvent const * event);

PNVMEvent * pnvmEventSetNetInstance(
        PNVMEvent * event,
        PNVMNetInstHead * instanc);

PNVMEvent * pnvmEventSetNext(
        PNVMEvent * event,
        PNVMEvent * nex);

PNVMEvent * pnvmEventSetNumber(
        PNVMEvent * event,
        unsigned int numbe);

PNVMEvent * pnvmEventSetPlaceId(
        PNVMEvent * event,
        unsigned int place_i);

PNVMEvent * pnvmEventSetPrev(
        PNVMEvent * event,
        PNVMEvent * pre);

PNVMEvent * pnvmEventSetTime(
        PNVMEvent * event,
        unsigned long time);

PNVMEvent * pnvmEventSetToken(
        PNVMEvent * event,
        PNVMToken toke);

PNVMToken & pnvmEventToken(PNVMEvent * event);
PNVMToken const & pnvmEventToken(PNVMEvent const * event);

template <>
void pnvmValueDelete<PNVMEvent>(PNVMEvent * event);

#if !defined(TARGET_ARDUINO) && DEBUG
    std::ostream & operator << (std::ostream &s, PNVMEvent const &);
#endif

#endif /* end of include guard: PNVM_Event_H */
