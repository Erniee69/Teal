#include "frontend/lexer.hpp"
#include "frontend/source_location.hpp"

enum class State {
    Start,
    Temp,
    Fatal,
    Accept,
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
    IStringArena* m_arena;

    static Option<Lexer> from(ErrorCtx* p_ctx, SourceFile* p_file, IStringArena* p_arena);

    void shift(State p_state);
    void discard();
    Option<Token> emit(TokenKind p_kind);

    bool step(Token* r_token);
    Token next();
};

Option<Lexer> Lexer::from(ErrorCtx* p_ctx, SourceFile* p_file, IStringArena *p_arena) {
    FILE* handle = fopen(p_file->m_path, "rb");
    if (!handle) {
        p_ctx->post_error("Could'nt open file %s", p_file->m_path);
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
        .m_token_location = SourceLocation::start(p_file),
        .m_current_location = SourceLocation::start(p_file),
        .m_current = current,
        .m_lexeme_buffer = Vec<char>::empty(),
        .m_arena = p_arena,
    });
}

void Lexer::shift(State p_state) {
    m_state = p_state;
    bool succ = m_lexeme_buffer.append(m_current);
    if (!succ) {
        m_ctx->post_fatal("Failed to allocate buffer, out of memory");
        return;
    };
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

Option<Token> Lexer::emit(TokenKind p_kind) {
    m_state = State::Start;
    bool succ = m_lexeme_buffer.append('\0');
    if (!succ) {
        m_ctx->post_fatal("Failed to allocate buffer, out of memory");
        return Option<Token>::none();
    };
    Option<IString> opt_lexeme = IString::from(m_lexeme_buffer.m_data, m_arena);
    m_lexeme_buffer.clear();
    if (!opt_lexeme.m_ok) {
        m_state = State::Fatal;
        return Option<Token>::none();
    }
    return Option<Token>::some((Token) {
        .m_kind = p_kind,
        .m_lexeme = opt_lexeme.unwrap(),
        .m_location = m_token_location,
    });
}

bool Lexer::step(Token* r_token) {
    // lexer logic
    return true;
}

Vec<Token> tokenize(ErrorCtx* p_ctx, SourceFile* p_source, IStringArena* p_arena) {
    if (p_ctx->m_fatal) {
        return Vec<Token>::empty();
    }
    Vec<typename T>
    return Vec<Token>::empty();
}