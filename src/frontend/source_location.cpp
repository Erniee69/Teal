#include "frontend/source_location.hpp"

SourceLocation SourceLocation::start(SourceFile* p_file) {
    return (SourceLocation) {
        .m_file = p_file,
        .m_row = 1,
        .m_col = 1,
    };
}