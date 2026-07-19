#pragma once
#include "common/istring.hpp"
#include "source_location.hpp"

enum class TokenKind {
    INVALID,
    IDENTIFIER,
    NUMBER,
    BASENUMBER,
    ARRAYDIMID,
    SKIP,
    SYM_EOF,
    
    KW_MODULE,
    KW_PARAM,
    KW_LOCAL,
    KW_INPUT,
    KW_OUTPUT,
    KW_WIRE,
    KW_REG,
    KW_ASSIGN,
    KW_ON,
    KW_MATCH,
    KW_DEFAULT,
    KW_NEVER,
    KW_POSEDGE,
    KW_NEGEDGE,
    
    SYM_LCURLYBRACKET,
    SYM_RCURLYBRACKET,
    SYM_LROUNDBRACKET,
    SYM_RROUNDBRACKET,
    SYM_LSQUAREBRACKET,
    SYM_RSQUAREBRACKET,
    SYM_ASSIGN,
    SYM_SEMICOLON,
    SYM_ASK,
    SYM_COLON,
    SYM_COMMA,
    SYM_DARROW,
    SYM_BITNOT,
    SYM_BITAND,
    SYM_BITOR,
    SYM_BITXOR,
    SYM_PLUS,
    SYM_MINUS,
    SYM_STAR,
    SYM_SLASH,
    SYM_PERC,
    SYM_SLL,
    SYM_SLR,
    SYM_SAL,
    SYM_SAR,
    SYM_CONCAT,
    SYM_REPEAT,
    SYM_LT,
    SYM_GT,
    SYM_LTE,
    SYM_GTE,
    SYM_EQ,
    SYM_NEQ,
    SYM_ARROW_IN,
    SYM_ARROW_OUT,
    SYM_DOT,
};

struct Token {
    TokenKind m_kind;
    IString m_lexeme;
    SourceLocation m_location;

    char* to_string() const;
};