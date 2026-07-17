#pragma once
#include "common/error.hpp"
#include "common/istring.hpp"
#include "common/vec.hpp"
#include "token.hpp"
#include "source_file.hpp"

Vec<Token> tokenize(ErrorCtx* p_ctx, SourceFile* p_source, IStringArena* p_arena);