#include "pnvm_event.h"

PNVMEvent * pnvmEventNewtimestoPlaceofInstscheduledAt(
        PNVMToken const &token,
        unsigned int n,
        unsigned int place_id,
        PNVMNetInstHead * instance,
        Time_t time)
{
    PNVMEvent * ev = new PNVMEvent;
    ev->place_id = place_id;
    ev->time = time;
    ev->instance = instance;
    ev->number = n;
    ev->token = token;
#if !defined(TARGET_ARDUINO) && DEBUG
    LOG4CXX_DEBUG(memlog, "creating event " << (*ev))
#endif
    return ev;
}

PNVMNetInstHead const * pnvmEventNetInstance(PNVMEvent const * event) {
    return event->instance;
}

PNVMNetInstHead * pnvmEventNetInstance(PNVMEvent * event) {
    return event->instance;
}

PNVMEvent const * pnvmEventNext(PNVMEvent const * event) {
    return event->next;
}

PNVMEvent * pnvmEventNext(PNVMEvent * event) {
    return event->next;
}

unsigned int pnvmEventNumber(PNVMEvent const * event) {
    return event->number;
}

unsigned int pnvmEventPlaceId(PNVMEvent const * event) {
    return event->place_id;
}

PNVMEvent const * pnvmEventPrev(PNVMEvent const * event) {
    return event->prev;
}

PNVMEvent * pnvmEventPrev(PNVMEvent * event) {
    return event->prev;
}

Time_t pnvmEventTime(PNVMEvent const * event) {
    return event->time;
}

PNVMEvent * pnvmEventSetNetInstance(
        PNVMEvent * event,
        PNVMNetInstHead * instance)
{
    event->instance = instance;
    return event;
}

PNVMEvent * pnvmEventSetNext(
        PNVMEvent * event,
        PNVMEvent * next)
{
    event->next = next;
    return event;
}

PNVMEvent * pnvmEventSetNumber(
        PNVMEvent * event,
        unsigned int number)
{
    event->number = number;
    return event;
}

PNVMEvent * pnvmEventSetPlaceId(
        PNVMEvent * event,
        unsigned int place_id)
{
    event->place_id = place_id;
    return event;
}

PNVMEvent * pnvmEventSetPrev(
        PNVMEvent * event,
        PNVMEvent * prev)
{
    event->prev = prev;
    return event;
}

PNVMEvent * pnvmEventSetTime(
        PNVMEvent * event,
        Time_t time)
{
    event->time = time;
    return event;
}

PNVMEvent * pnvmEventSetToken(
        PNVMEvent * event,
        PNVMToken token)
{
    event->token = token;
    return event;
}

PNVMToken & pnvmEventToken(PNVMEvent * event) {
    return event->token;
}
PNVMToken const & pnvmEventToken(PNVMEvent const * event) {
    return event->token;
}

template <>
void pnvmValueDelete<PNVMEvent>(PNVMEvent * event) {
    #if !defined(TARGET_ARDUINO) && DEBUG
        LOG4CXX_DEBUG(memlog, "deleting " << (*event))
    #endif
    pnvmTokenMakeInvalid(event->token);
    pnvmBlockDelete(event);
}

#if !defined(TARGET_ARDUINO) && DEBUG

std::ostream & operator << (std::ostream &s, PNVMEvent const &hb) {
    s << "PNVMEvent(" << &hb << ", n=" << hb.number << ", t=" << hb.token
      << ", pid=" << hb.place_id << ", i=" << (*hb.instance) << ")";
    return s;
}

#endif
