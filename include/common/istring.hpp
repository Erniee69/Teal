#pragma once
#include "defs.hpp"
#include "option.hpp"

struct IStringArena {
    usize  m_len;
    usize  m_cap;
    char** m_data;

    static IStringArena empty();

    Option<usize> intern(const char* p_str);
    const char* get(usize p_id) const;

    bool copy(IStringArena* p_dest) const;
    void drop() const;
};

struct IString {
    usize m_id;
    IStringArena* m_arena;

    static Option<IString> from(const char* p_str);
    static Option<IString> from(const char* p_str, IStringArena* p_arena);

    usize length() const;
    const char* get() const;
};

bool istring_eq(IString p_istr1, IString p_istr2);