#include "pnvm_token.h"
#include "pnvm_generic_block.h"


TokenValue_t PNVMToken::make_integer(IntegerValue_t value) {
    return ((TokenValue_t) value) & (((TokenValue_t) (~0L)) >> 1L);
}

TokenValue_t PNVMToken::make_net_ref(void *addr) {
    return ( (  (TokenValue_t) TOKEN_TYPE_NET_REF
             << (sizeof(IntegerValue_t)*8L - 2L))
           | ((TokenValue_t) addr >> 2L));
}

TokenValue_t PNVMToken::make_pointer(void *addr) {
    return ( (  (TokenValue_t) TOKEN_TYPE_POINTER
             << (sizeof(IntegerValue_t)*8L - 2L))
           | ((TokenValue_t) addr >> 2L));
}

TokenType_t PNVMToken::get_type() const {
    return (TokenType_t) ((value & ((~0L) << (sizeof(IntegerValue_t)*8L - 1L)))
        ? (value >> (sizeof(IntegerValue_t)*8L - 2L))
        : ((TokenValue_t) TOKEN_TYPE_INTEGER));
}

IntegerValue_t PNVMToken::get_integer() const {
    return ( (value & (((TokenValue_t) (~0L)) >> 1L))
           | ((value & (1L << (sizeof(IntegerValue_t)*8L - 2L))) << 1L));
}

void PNVMToken::set_integer(IntegerValue_t v) {
    value = ((TokenValue_t) v) & (((TokenValue_t) (~0L)) >> 1L);
}

void PNVMToken::set_type(TokenType_t type) {
    if (get_type() != type) {
        if (get_type() != TOKEN_TYPE_INTEGER && get_address() != NULL) {
            value_unreference((PNVMHeaderBlock *) get_address());
        }
        value = ((TokenValue_t) type) << (sizeof(TokenValue_t)*8L - 2L);
        if (type == TOKEN_TYPE_INTEGER) {
            set_integer(0L);
        }else {
            set_address(NULL);
        }
    }
}

void * PNVMToken::get_address() const {
    return (void *) ((value & (((TokenValue_t) (~0L)) >> 2L)) << 2L);
}

void PNVMToken::set_address(void * addr) {
    if (get_address() != addr) {
        if (get_address() != NULL) {
            value_unreference((PNVMHeaderBlock *) get_address());
        }else if (addr != NULL) {
            value_reference((PNVMHeaderBlock *) addr);
        }
        value = ( (value & (~((TokenValue_t) (~0L) >> 2L)))
                | ((TokenValue_t) addr >> 2L));
    }
}


PNVMToken & pnvmTokenAssign(PNVMToken &dest, PNVMToken const &src) {
    dest.set_type(src.get_type());
    if (src.get_type() == TOKEN_TYPE_INTEGER) {
        dest.set_integer(src.get_integer());
    }else {
        dest.set_address(src.get_address());
    }
    return dest;
}

PNVMToken & pnvmTokenSetInteger(PNVMToken &t, IntegerValue_t value) {
    t.set_type(TOKEN_TYPE_INTEGER);
    t.set_integer(value);
    return t;
}

PNVMToken & pnvmTokenSetNetRef(PNVMToken &t, PNVMHeaderBlock *inst) {
    t.set_type(TOKEN_TYPE_NET_REF);
    t.set_address(inst);
    return t;
}

PNVMToken & pnvmTokenSetPointer(PNVMToken &t, PNVMHeaderBlock *block) {
    t.set_type(TOKEN_TYPE_POINTER);
    t.set_address(block);
    return t;
}

PNVMToken pnvmNewNetRef(PNVMHeaderBlock * addr) {
    assert(addr->signature() == SIGN_NET_INSTANCE);
    pnvmValueReference((PNVMNetInstHead *) addr);
    return PNVMToken(PNVMToken::make_net_ref((void *) addr));
}

PNVMToken pnvmNewPointer(PNVMHeaderBlock * addr) {
    value_reference(addr);
    return PNVMToken(PNVMToken::make_pointer(addr));
}

PNVMToken pnvmTokenClone(PNVMToken const &t) {
    PNVMToken token;
    pnvmTokenAssign(token, t);
    return token;
}

template <>
void pnvm_item_delete<PNVMToken>(PNVMToken & t) {
    if (t.value == PNVMToken::s_invalid) {
        return;
    }
    if (t.get_type() == TOKEN_TYPE_NET_REF) {
        pnvmValueUnreference((PNVMNetInstHead *) t.get_address());
    }else if (t.is_address()) {
        switch (((PNVMHeaderBlock *) t.get_address())->signature()) {
            case SIGN_STRING:
                pnvmValueUnreference((PNVMStringHead *) t.get_address());
                break;
#ifndef TARGET_ARDUINO
            case SIGN_TUPLE:
                pnvmValueUnreference((PNVMTupleHead *) t.get_address());
                break;
#endif
            case SIGN_ARRAY:
                pnvmValueUnreference((PNVMArrayHead *) t.get_address());
                break;
            case SIGN_PLACE:
                pnvmValueDelete((PNVMPlaceHead *) t.get_address());
                break;
            default:
                break;
        }
    }
    t.value = PNVMToken::s_invalid;
}

char const * get_token_type_name(TokenType_t type) {
    switch (type) {
        case TOKEN_TYPE_INTEGER:
            return "Integer";
        case TOKEN_TYPE_NET_REF:
            return "NetRef";
        case TOKEN_TYPE_POINTER:
            return "Pointer";
    }
    return "unknown";
}

#if !defined(TARGET_ARDUINO) && DEBUG

std::ostream & operator << (std::ostream & s, PNVMToken const &t) {
    s << "PNVMToken(";
    if (!pnvmTokenIsValid(t)) {
        s << "invalid";
    }else {
        s << get_token_type_name(t.get_type()) << ", ";
        switch (t.get_type()) {
            case TOKEN_TYPE_INTEGER:
                s << t.get_integer();
                break;
            case TOKEN_TYPE_NET_REF:
                s << *((PNVMNetInstHead *) t.get_address());
                break;
            case TOKEN_TYPE_POINTER:
                s << *((PNVMHeaderBlock *) t.get_address());
                break;
        }
    }
    s << ")";
    return s;
}

#endif
