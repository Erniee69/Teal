#include "common/error.hpp"
#include "common/istring.hpp"
#include "frontend/lexer.hpp"
#include "frontend/source_file.hpp"
#include <stdlib.h>

int main() {
    const char* file_name = "scoreboard.teal";
    const char* file_path = "assets/sources/scoreboard.teal";
    SourceFile src_file = (SourceFile) {
        .m_name = file_name,
        .m_path = file_path,
        .m_state = SourceFile::State::Start,
    };
    ErrorCtx err_ctx = ErrorCtx::empty();
    IStringArena arena = IStringArena::empty();
    Vec<Token> tokens = tokenize(&err_ctx, &src_file, &arena);
    err_ctx.report(stderr, true);
    printf("=== Token dump of file %s ===\n\n", file_name);
    for (usize i = 0; i < tokens.m_len; i++) {
        char* buf = tokens.at(i)->to_string();
        printf("%s\n", buf);
        free(buf);
    }
    u32 error_count = err_ctx.m_errors.m_len;
    tokens.drop();
    arena.drop();
    err_ctx.drop();
    return error_count != 0;
}