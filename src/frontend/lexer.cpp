#include "frontend/lexer.hpp"
#include "frontend/source_file.hpp"
#include "frontend/source_location.hpp"
#include "common/option.hpp"
#include "frontend/token.hpp"
#include <string.h>

enum class State {
    Start,
    Temp,
    Fatal,
    Accept,

    Identifier,
    Number,
    Whitespace,
    Apostrophe,
    Percent,
    Assign,
    Minus,
    Slash,
    Less,
    Greater,
    Dollar,
    Bang,
    HexNumber,
    SingleComment,
    DoubleLess,
    DoubleGreater,
    ArrayDimensionID,
};

struct Lexer {
    State m_state;
    SourceFile* m_file;
    FILE* m_file_handle;
    ErrorCtx* m_ctx;

    SourceLocation m_token_location;
    SourceLocation m_current_location;

    char m_current;
    Vec<char> m_lexeme_buffer;
    bool m_emit;
    Token m_token;
    IStringArena* m_arena;

    static Option<Lexer> from(ErrorCtx* p_ctx, SourceFile* p_file, IStringArena* p_arena);

    void shift(State p_state);
    void discard();
    void emit(TokenKind p_kind);
    void accept();

    bool step(Token* r_token);
    Token next();

    void unexpected_token(const char* p_state);
};

Option<Lexer> Lexer::from(ErrorCtx* p_ctx, SourceFile* p_file, IStringArena *p_arena) {
    FILE* handle = fopen(p_file->m_path, "rb");
    if (!handle) {
        p_ctx->post_error("Could'nt open source file %s", p_file->m_path);
        return Option<Lexer>::none();
    }
    char current;
    int fread_count = fread(&current, sizeof(char), 1, handle);
    if (fread_count != 1) {
        current = '\0';
    }
    return Option<Lexer>::some((Lexer) {
        .m_state = State::Start,
        .m_file = p_file,
        .m_file_handle = handle,
        .m_ctx = p_ctx,
        .m_token_location = SourceLocation::start(p_file),
        .m_current_location = SourceLocation::start(p_file),
        .m_current = current,
        .m_lexeme_buffer = Vec<char>::empty(),
        .m_emit = false,
        .m_arena = p_arena,
    });
}

void Lexer::shift(State p_state) {
    m_state = p_state;
    m_lexeme_buffer.append(m_current);
    if (m_current == '\n') {
        m_current_location.m_col = 1;
        m_current_location.m_row++;
    }
    else {
        m_current_location.m_col++;
    }
    int fread_count = fread(&m_current, sizeof(char), 1, m_file_handle);
    if (fread_count != 1) {
        m_current = '\0';
    }
}

void Lexer::discard() {
    m_state = State::Start;
    m_lexeme_buffer.clear();
    m_token_location = m_current_location;
}

void Lexer::emit(TokenKind p_kind) {
    m_emit = true;
    m_state = State::Start;
    m_lexeme_buffer.append('\0');
    IString lexeme = IString::from(m_lexeme_buffer.m_data, m_arena);
    m_lexeme_buffer.clear();
    m_token = (Token) {
        .m_kind = p_kind,
        .m_lexeme = lexeme,
        .m_location = m_token_location,
    };
}

void Lexer::accept() {
    this->emit(TokenKind::SYM_EOF);
    m_state = State::Accept;
}

static bool is_letter(char p_char) {
    return (('a' <= p_char) && (p_char <= 'z')) || (('A' <= p_char) && (p_char <= 'Z'));
}

static bool is_number(char p_char) {
    return ('0' <= p_char) && (p_char <= '9');
}

static bool is_whitespace(char p_char) {
    return (p_char == ' ') || (p_char == '\n') || (p_char == '\t') || (p_char == '\r');
}

static bool is_hexadecimal_letter(char p_char) {
    return (('a' <= p_char) && (p_char <= 'f')) || (('A' <= p_char) && (p_char <= 'F')) || is_number(p_char); 
}

bool Lexer::step(Token* r_token) {
    m_emit = false;
    if (m_state == State::Start) {
        if (is_letter(m_current) || m_current == '_') {
            this->shift(State::Identifier);
        }
        else if (is_number(m_current)) {
            this->shift(State::Number);
        }
        else if (is_whitespace(m_current)) {
            this->shift(State::Whitespace);
        }
        else if (m_current == '\'') {
            this->shift(State::Apostrophe);
        }
        else if (m_current == '%') {
            this->shift(State::Percent);
        }
        else if (m_current == '\0') {
            this->accept();
        }
        else if (m_current == '{') {
            this->shift(State::Temp);
            this->emit(TokenKind::SYM_LCURLYBRACKET);
        }
        else if (m_current == '}') {
            this->shift(State::Temp);
            this->emit(TokenKind::SYM_RCURLYBRACKET);
        }
        else if (m_current == '(') {
            this->shift(State::Temp);
            this->emit(TokenKind::SYM_LROUNDBRACKET);
        }
        else if (m_current == ')') {
            this->shift(State::Temp);
            this->emit(TokenKind::SYM_RROUNDBRACKET);
        }
        else if (m_current == '[') {
            this->shift(State::Temp);
            this->emit(TokenKind::SYM_LSQUAREBRACKET);
        }
        else if (m_current == ']') {
            this->shift(State::Temp);
            this->emit(TokenKind::SYM_RSQUAREBRACKET);
        }
        else if (m_current == '=') {
            this->shift(State::Assign);
        }
        else if (m_current == ';') {
            this->shift(State::Temp);
            this->emit(TokenKind::SYM_SEMICOLON);
        }
        else if (m_current == '?') {
            this->shift(State::Temp);
            this->emit(TokenKind::SYM_ASK);
        }
        else if (m_current == ':') {
            this->shift(State::Temp);
            this->emit(TokenKind::SYM_COLON);
        }
        else if (m_current == ',') {
            this->shift(State::Temp);
            this->emit(TokenKind::SYM_COMMA);
        }
        else if (m_current == '~') {
            this->shift(State::Temp);
            this->emit(TokenKind::SYM_BITNOT);
        }
        else if (m_current == '&') {
            this->shift(State::Temp);
            this->emit(TokenKind::SYM_BITAND);
        }
        else if (m_current == '|') {
            this->shift(State::Temp);
            this->emit(TokenKind::SYM_BITOR);
        }
        else if (m_current == '^') {
            this->shift(State::Temp);
            this->emit(TokenKind::SYM_BITXOR);
        }
        else if (m_current == '+') {
            this->shift(State::Temp);
            this->emit(TokenKind::SYM_PLUS);
        }
        else if (m_current == '-') {
            this->shift(State::Minus);
        }
        else if (m_current == '*') {
            this->shift(State::Temp);
            this->emit(TokenKind::SYM_STAR);
        }
        else if (m_current == '/') {
            this->shift(State::Slash);
        }
        else if (m_current == '<') {
            this->shift(State::Less);
        }
        else if (m_current == '>') {
            this->shift(State::Greater);
        }
        else if (m_current == '$') {
            this->shift(State::Dollar);
        }
        else if (m_current == '!') {
            this->shift(State::Bang);
        }
        else if (m_current == '.') {
            this->shift(State::Temp);
            this->emit(TokenKind::SYM_DOT);
        }
        else {
            this->unexpected_token("Start");
        }
    }
    else if (m_state == State::Identifier) {
        if (is_letter(m_current) || is_number(m_current) || m_current == '_') {
            this->shift(State::Identifier);
        }
        else {
            const usize map_len = 14;
            const struct { const char* key; TokenKind val; } map[map_len] = {
                {"module",  TokenKind::KW_MODULE },
                {"param",   TokenKind::KW_PARAM  },
                {"local",   TokenKind::KW_LOCAL  },
                {"input",   TokenKind::KW_INPUT  },
                {"output",  TokenKind::KW_OUTPUT },
                {"wire",    TokenKind::KW_WIRE   },
                {"reg",     TokenKind::KW_REG    },
                {"assign",  TokenKind::KW_ASSIGN },
                {"on",      TokenKind::KW_ON     },
                {"match",   TokenKind::KW_MATCH  },
                {"default", TokenKind::KW_DEFAULT},
                {"never",   TokenKind::KW_NEVER  },
                {"posedge", TokenKind::KW_POSEDGE},
                {"negedge", TokenKind::KW_NEGEDGE},
            };
            bool match = false;
            for (usize i = 0; i < map_len; i++) {
                if (strlen(map[i].key) == m_lexeme_buffer.m_len && !strncmp(map[i].key, m_lexeme_buffer.m_data, m_lexeme_buffer.m_len)) {
                    match = true;
                    this->emit(map[i].val);
                }
            }
            if (!match) {
                this->emit(TokenKind::IDENTIFIER);
            }
        }
    }
    else if (m_state == State::Number) {
        if (is_number(m_current)) {
            this->shift(State::Number);
        }
        else {
            this->emit(TokenKind::NUMBER);
        }
    }
    else if (m_state == State::Whitespace) {
        if (is_whitespace(m_current)) {
            this->shift(State::Whitespace);
        }
        else {
            this->discard();
        }
    }
    else if (m_state == State::Apostrophe) {
        if (m_current == 'b' || m_current == 'o' || m_current == 'd' || m_current == 'h') {
            this->shift(State::HexNumber);
        }
        else {
            this->unexpected_token("Apostrophe");
        }
    }
    else if (m_state == State::Percent) {
        if (is_number(m_current)) {
            this->shift(State::ArrayDimensionID);
        }
        else {
            this->emit(TokenKind::SYM_PERC);
        }
    }
    else if (m_state == State::Assign) {
        if (m_current == '>') {
            this->shift(State::Temp);
            this->emit(TokenKind::SYM_DARROW);
        }
        else if (m_current == '=') {
            this->shift(State::Temp);
            this->emit(TokenKind::SYM_EQ);
        }
        else {
            this->emit(TokenKind::SYM_ASSIGN);
        }
    }
    else if (m_state == State::Minus) {
        if (m_current == '>') {
            this->shift(State::Temp);
            this->emit(TokenKind::SYM_ARROW_OUT);
        }
        else {
            this->emit(TokenKind::SYM_MINUS);
        }
    }
    else if (m_state == State::Slash) {
        if (m_current == '/') {
            this->shift(State::SingleComment);
        }
        else {
            this->shift(State::Temp);
            this->emit(TokenKind::SYM_SLASH);
        }
    }
    else if (m_state == State::Less) {
        if (m_current == '<') {
            this->shift(State::DoubleLess);
        }
        else if (m_current == '=') {
            this->shift(State::Temp);
            this->emit(TokenKind::SYM_LTE);
        }
        else if (m_current == '-') {
            this->shift(State::Temp);
            this->emit(TokenKind::SYM_ARROW_IN);
        }
        else {
            this->emit(TokenKind::SYM_LT);
        }
    }
    else if (m_state == State::Greater) {
        if (m_current == '>') {
            this->shift(State::DoubleGreater);
        }
        else if (m_current == '=') {
            this->shift(State::Temp);
            this->emit(TokenKind::SYM_GTE);
        }
        else {
            this->emit(TokenKind::SYM_GT);
        }
    }
    else if (m_state == State::Dollar) {
        if (m_current == '$') {
            this->shift(State::Temp);
            this->emit(TokenKind::SYM_REPEAT);
        }
        else {
            this->emit(TokenKind::SYM_CONCAT);
        }
    }
    else if (m_state == State::Bang) {
        if (m_current == '=') {
            this->shift(State::Temp);
            this->emit(TokenKind::SYM_NEQ);
        }
        else {
            this->unexpected_token("Bang");
        }
    }
    else if (m_state == State::HexNumber) {
        if (is_hexadecimal_letter(m_current)) {
            this->shift(State::HexNumber);
        }
        else {
            this->emit(TokenKind::BASENUMBER);
        }
    }
    else if (m_state == State::SingleComment) {
        if (m_current != '\n') {
            this->shift(State::SingleComment);
        }
        else {
            this->discard();
        }
    }
    else if (m_state == State::DoubleLess) {
        if (m_current == '<') {
            this->shift(State::Temp);
            this->emit(TokenKind::SYM_SAL);
        }
        else {
            this->emit(TokenKind::SYM_SLL);
        }
    }
    else if (m_state == State::DoubleGreater) {
        if (m_current == '>') {
            this->shift(State::Temp);
            this->emit(TokenKind::SYM_SAR);
        }
        else {
            this->emit(TokenKind::SYM_SLR);
        }
    }
    else if (m_state == State::ArrayDimensionID) {
        if (is_number(m_current)) {
            this->shift(State::ArrayDimensionID);
        }
        else {
            this->emit(TokenKind::ARRAYDIMID);
        }
    }
    if (m_emit) {
        *r_token = m_token;
    }
    return m_emit;
}

Token Lexer::next() {
    Token result;
    bool emitted = false;
    while (!emitted) {
        emitted = this->step(&result);
    }
    return result;
}

void Lexer::unexpected_token(const char* p_state) {
    m_ctx->post_error(
        "Error in line %u, col %u while tokenizing %s. Unexpected token %c in state '%s'.",
        m_current_location.m_row,
        m_current_location.m_col,
        m_current_location.m_file->m_name,
        m_current,
        p_state
    );
    this->shift(State::Temp);
    this->emit(TokenKind::INVALID);
}

Vec<Token> tokenize(ErrorCtx* p_ctx, SourceFile* p_source, IStringArena* p_arena) {
    if (p_ctx->m_fatal) {
        return Vec<Token>::empty();
    }
    Vec<Token> tokens = Vec<Token>::empty();
    Option<Lexer> opt_lexer = Lexer::from(p_ctx, p_source, p_arena);
    if (!opt_lexer.m_ok) {
        p_source->m_state = SourceFile::State::Error;
        return Vec<Token>::empty();
    }
    Lexer lexer = opt_lexer.unwrap();
    while (lexer.m_state != State::Fatal && lexer.m_state != State::Accept) {
        tokens.append(lexer.next());
    }
    if (lexer.m_state == State::Fatal) {
        p_source->m_state = SourceFile::State::Error;
    }
    return tokens;
}