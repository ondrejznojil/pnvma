/******************************************************************************
 * Template data type. Not indexable and not collectable.
 *
 * @author Michal Minar <xminar06@stud.fit.vutbr.cz>
 * @year 2013
 */
#ifndef PNVM_TEMPLATE_H
#define PNVM_TEMPLATE_H

#if !defined(TARGET_ARDUINO) && DEBUG
    #include <ostream>
#endif
#include "pnvm_config.h"
#include "pnvm_string.h"
#include "pnvm_tuple.h"
#include "pnvm_array.h"

struct HeaderTemplate {
#if BLOCK_SIZE <= 16
    ItemCount_t value_sig   : SignatureBitSize;
    ItemCount_t place_count : sizeof(ItemCount_t)*8 - (SignatureBitSize);
#else
    Signature_t value_sig;
    ItemCount_t place_count;
#endif
    CodeOffset_t code_offset;   //<! offest to netTemplateCode
    CodeLength_t code_length;   //<! length of code in netTemplateCode
    CodeOffset_t transitions;   //<! beginning of transitions code
    PNVMStringHead * name;      //<! name of template
    PNVMTupleHead * places;     //<! place names as Strings in tuple
    PNVMTupleHead * symbols;    //<! template symbols stored in tuple
    PNVMArrayHead * instances;  //<! all existing instances of template

    explicit HeaderTemplate(Signature_t sig=SIGN_TEMPLATE):
        value_sig(sig), place_count(0),
        code_offset(0),
        code_length(0),
        transitions(0),
        name(NULL),
        places(NULL),
        symbols(NULL),
        instances(NULL) {}

    Signature_t signature() const {
        return (Signature_t) value_sig;
    }

    void set_signature(Signature_t sig) {
        value_sig = sig;
    }
};

typedef PNVMBlock<SIGN_TEMPLATE, false, false,
        HeaderTemplate, void>::Head PNVMTemplate;

PNVMTemplate * pnvmTemplateNew();

CodeLength_t pnvmTemplateCodeLength(PNVMTemplate const *tmpl);

CodeOffset_t pnvmTemplateCodeOffset(PNVMTemplate const *tmpl);

PNVMArrayHead const * pnvmTemplateInstances(PNVMTemplate const *tmpl);

PNVMArrayHead * pnvmTemplateInstances(PNVMTemplate *tmpl);

PNVMStringHead const * pnvmTemplateName(PNVMTemplate const *tmpl);

PNVMStringHead * pnvmTemplateName(PNVMTemplate *tmpl);

PNVMTupleHead const * pnvmTemplateSymbols(PNVMTemplate const *tmpl);

PNVMTupleHead * pnvmTemplateSymbols(PNVMTemplate *tmpl);

CodeOffset_t pnvmTemplateTransitions(PNVMTemplate *const tmpl);

ItemCount_t pnvmTemplatePlaceCount(PNVMTemplate const *tmpl);

PNVMTupleHead const * pnvmTemplatePlaces(PNVMTemplate const *tmpl);

PNVMTupleHead * pnvmTemplatePlaces(PNVMTemplate *tmpl);

PNVMTemplate * pnvmTemplateSetCodeLength(
        PNVMTemplate *tmpl,
        CodeLength_t code_lengt);


PNVMTemplate * pnvmTemplateSetCodeOffset(
        PNVMTemplate *tmpl,
        CodeOffset_t code_offse);


PNVMTemplate * pnvmTemplateSetInstances(
        PNVMTemplate *tmpl,
        PNVMArrayHead *instance);


PNVMTemplate * pnvmTemplateSetName(
        PNVMTemplate *tmpl,
        PNVMStringHead * nam);


PNVMTemplate * pnvmTemplateSetPlaceCount(
        PNVMTemplate *tmpl,
        unsigned int place_coun);


PNVMTemplate * pnvmTemplateSetPlaces(
        PNVMTemplate *tmpl,
        PNVMTupleHead *place);


PNVMTemplate * pnvmTemplateSetSymbols(
        PNVMTemplate *tmpl,
        PNVMTupleHead *symbol);


PNVMTemplate * pnvmTemplateSetTransitions(
        PNVMTemplate *tmpl,
        unsigned int transition);

#if !defined(TARGET_ARDUINO) && DEBUG
    std::ostream & operator << (std::ostream &s, PNVMTemplate const &);
#endif

#endif /* end of include guard: PNVM_TEMPLATE_H */
