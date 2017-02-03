/******************************************************************************
 * Token used to store user data types. It's also the main manipulator
 * of references.
 *
 * @author Michal Minar <xminar06@stud.fit.vutbr.cz>
 * @year 2013
 */
#ifndef PNVM_TOKEN_H
#define PNVM_TOKEN_H

#include "pnvm_config.h"
#include "pnvm_type_interface.h"
#include "pnvm_block.h"
#if !defined(TARGET_ARDUINO) && DEBUG
    #include <ostream>
#endif

/*!****************************************************************************
 * In single integer stores both the type and value. Type is determined by
 * 2 most significant bits. The rest is used for value.
 * Value can either be integer or address.
 *
 * Integer's maximum value is INTMAX_MAX/2. Minimum is (- INTMAX_MAX/2 - 1).
 *
 * Address is truncated by two least significant bits. Thus blocks or any other
 * address stored must be aligned to the multiples of four.
 */
struct PNVMToken {
    /**************************************************************************
     * Constants
     */
    static TokenValue_t const s_invalid = \
        (TokenValue_t) TOKEN_TYPE_POINTER << (sizeof(IntegerValue_t)*8L - 2L);

    /**************************************************************************
     * Public attributes
     */
    TokenValue_t value;

    /**************************************************************************
     * Class functions
     */
    static TokenValue_t make_integer(IntegerValue_t value);

    static TokenValue_t make_net_ref(void *addr);

    static TokenValue_t make_pointer(void *addr);

    /**************************************************************************
     * Constructors
     */
    PNVMToken(): value(s_invalid) {}

    explicit PNVMToken(TokenValue_t v): value(v) {}

    /**************************************************************************
     * Public methods
     */
    TokenType_t get_type() const;
    void set_type(TokenType_t type);

    IntegerValue_t get_integer() const;

    void set_integer(IntegerValue_t v);

    void * get_address() const;
    void set_address(void * addr);

    bool is_address() const { 
        return get_type() != TOKEN_TYPE_INTEGER;
    }
    bool is_integer() const {
        return get_type() == TOKEN_TYPE_INTEGER;
    }

    bool operator == (PNVMToken const & other) const {
        return value == other.value;
    }

};

inline bool pnvmTokenIsValid(PNVMToken const &t) {
    return t.value != PNVMToken::s_invalid;
}

PNVMToken & pnvmTokenAssign(PNVMToken &dest, PNVMToken const &src);

inline PNVMToken & pnvmTokenMakeInvalid(PNVMToken &t) {
    t.set_type(TOKEN_TYPE_POINTER);
    t.set_address(NULL);
    return t;
}

inline TokenType_t pnvmTokenType(PNVMToken const &t) {
    return t.get_type();
}

inline IntegerValue_t pnvmTokenValue(PNVMToken const &t) {
    return (t.get_type() == TOKEN_TYPE_INTEGER)
            ? t.get_integer()
            : (IntegerValue_t) t.get_address();
}

inline IntegerValue_t pnvmTokenInteger(PNVMToken const &t) {
    return t.get_integer();
}

inline PNVMToken & pnvmTokenSetType(PNVMToken & t, TokenType_t type) {
    t.set_type(type);
    return t;
}

PNVMToken & pnvmTokenSetInteger(PNVMToken &t, IntegerValue_t value);

PNVMToken & pnvmTokenSetNetRef(PNVMToken &t, PNVMHeaderBlock *inst);

PNVMToken & pnvmTokenSetPointer(PNVMToken &t, PNVMHeaderBlock *block);

inline PNVMToken pnvmNewToken() {
    return PNVMToken();
}

inline PNVMToken pnvmNewInteger(IntegerValue_t value) {
    return PNVMToken(PNVMToken::make_integer(value));
}

PNVMToken pnvmNewNetRef(PNVMHeaderBlock * addr);

PNVMToken pnvmNewPointer(PNVMHeaderBlock * addr);

PNVMToken pnvmTokenClone(PNVMToken const &t);

template <>
void pnvm_item_delete<PNVMToken>(PNVMToken & t);

template <>
inline void pnvm_item_reinitialize<PNVMToken>(PNVMToken & t) {
    t.value = PNVMToken::s_invalid;
}

char const * get_token_type_name(TokenType_t type);

#if !defined(TARGET_ARDUINO) && DEBUG
    std::ostream & operator << (std::ostream & s, PNVMToken const &t);
#endif

#endif /* end of include guard: PNVM_TOKEN_H */
