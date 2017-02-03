#ifndef STATIC_ASSERT_H
#define STATIC_ASSERT_H

#define CONCAT_TOKENS(TokenA, TokenB) \
    TokenA ## TokenB
#define EXPAND_THEN_CONCAT(TokenA, TokenB) \
    CONCAT_TOKENS(TokenA, TokenB)
#define ASSERT(Expression) \
    enum{ EXPAND_THEN_CONCAT(ASSERT_line_, __LINE__) = 1 / !!(Expression) }
#define ASSERTM(Expression, Message) \
    enum{ EXPAND_THEN_CONCAT(Message ## _ASSERT_line_, __LINE__) = 1 \
        / !!(Expression) }
#define ASSERT_SAME_SIZE(TokenA, TokenB) \
    ASSERTM(sizeof(TokenA) == sizeof(TokenB), Different_size)

#endif /* end of include guard: STATIC_ASSERT_H */
