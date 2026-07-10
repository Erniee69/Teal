#pragma once

enum class TokenKind {
    INVALID,
    EOF,

    KW_PARAM,
    KW_LOCAL,
    KW_MODULE,
    KW_INPUT,
    KW_OUTPUT,
    KW_REG,
    KW_WIRE,
    KW_ASSIGN,
    KW_ON,
    KW_POSEDGE,
    KW_NEGEDGE,
    KW_MATCH,
    
};