#pragma once
#include "common/defs.hpp"
#include "frontend/source_file.hpp"

struct SourceLocation {
    SourceFile* m_file;
    usize m_row;
    usize m_col;

    static SourceLocation start(SourceFile* p_file);
};