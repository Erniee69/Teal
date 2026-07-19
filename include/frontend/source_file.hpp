#pragma once

struct SourceFile {
    enum class State {
        Start,
        Tokenized,
        Parsed,
        Error,
    };

    const char* m_name;
    const char* m_path;
    State m_state;
};