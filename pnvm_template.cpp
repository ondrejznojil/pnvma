#if !defined(TARGET_ARDUINO) && DEBUG
    #include <iostream>
#endif
#include "pnvm_template.h"
#include "pnvm_utils.h"

PNVMTemplate * pnvmTemplateNew() {
    PNVMTemplate * res = new PNVMTemplate;
#if !defined(TARGET_ARDUINO) && DEBUG
    LOG4CXX_DEBUG(memlog, "creating template " << res)
#endif
    return res;
}

CodeLength_t pnvmTemplateCodeLength(PNVMTemplate const *tmpl) {
    return tmpl->code_length;
}

CodeOffset_t pnvmTemplateCodeOffset(PNVMTemplate const *tmpl) {
    return tmpl->code_offset;
}

PNVMArrayHead const * pnvmTemplateInstances(PNVMTemplate const *tmpl) {
    return tmpl->instances;
}

PNVMArrayHead * pnvmTemplateInstances(PNVMTemplate *tmpl) {
    return tmpl->instances;
}

PNVMStringHead const * pnvmTemplateName(PNVMTemplate const *tmpl) {
    return tmpl->name;
}

PNVMStringHead * pnvmTemplateName(PNVMTemplate *tmpl) {
    return tmpl->name;
}

PNVMTupleHead const * pnvmTemplateSymbols(PNVMTemplate const *tmpl) {
    return tmpl->symbols;
}

PNVMTupleHead * pnvmTemplateSymbols(PNVMTemplate *tmpl) {
    return tmpl->symbols;
}

CodeOffset_t pnvmTemplateTransitions(PNVMTemplate *const tmpl) {
    return tmpl->transitions;
}

ItemCount_t pnvmTemplatePlaceCount(PNVMTemplate const *tmpl) {
    return tmpl->place_count;
}

PNVMTupleHead const * pnvmTemplatePlaces(PNVMTemplate const *tmpl) {
    return tmpl->places;
}

PNVMTupleHead * pnvmTemplatePlaces(PNVMTemplate *tmpl) {
    return tmpl->places;
}

PNVMTemplate * pnvmTemplateSetCodeLength(
        PNVMTemplate *tmpl,
        CodeLength_t code_length)
{
    tmpl->code_length = code_length;
    return tmpl;
}

PNVMTemplate * pnvmTemplateSetCodeOffset(
        PNVMTemplate *tmpl,
        CodeOffset_t code_offset)
{
    tmpl->code_offset = code_offset;
    return tmpl;
}

PNVMTemplate * pnvmTemplateSetInstances(
        PNVMTemplate *tmpl,
        PNVMArrayHead *instances)
{
    tmpl->instances = instances;
    return tmpl;
}

PNVMTemplate * pnvmTemplateSetName(
        PNVMTemplate *tmpl,
        PNVMStringHead * name)
{
    tmpl->name = name;
    return tmpl;
}

PNVMTemplate * pnvmTemplateSetPlaceCount(
        PNVMTemplate *tmpl,
        unsigned int place_count)
{
    tmpl->place_count = place_count;
    return tmpl;
}

PNVMTemplate * pnvmTemplateSetPlaces(
        PNVMTemplate *tmpl,
        PNVMTupleHead *places)
{
    tmpl->places = places;
    return tmpl;
}

PNVMTemplate * pnvmTemplateSetSymbols(
        PNVMTemplate *tmpl,
        PNVMTupleHead *symbols)
{
    tmpl->symbols = symbols;
    return tmpl;
}

PNVMTemplate * pnvmTemplateSetTransitions(
        PNVMTemplate *tmpl,
        unsigned int transitions)
{
    tmpl->transitions = transitions;
    return tmpl;
}

#if !defined(TARGET_ARDUINO) && DEBUG

std::ostream & operator << (std::ostream &s, PNVMTemplate const &hb) {
    s << "PNVMTemplate(" << &hb << ", n=\"";
    if (hb.name)
        s << (asPlainString(hb.name));
    s << "\", pc=" << hb.place_count << ", co=" << hb.code_offset
      << ", cl=" << hb.code_length << ", ct=" << hb.transitions << ", smbs="
      << (hb.symbols   ? pnvmValueItemCount(hb.symbols) : 0) << ", plcs="
      << (hb.places    ? pnvmValueItemCount(hb.places) : 0)  << ", itms="
      << (hb.instances ? pnvmValueItemCount(hb.instances) : 0)
      << ")";
    return s;
}

#endif
