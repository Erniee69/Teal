#pragma once
#include "defs.hpp"

struct IStringArena {
    usize  m_len;
    usize  m_cap;
    char** m_data;

    static IStringArena empty();

    usize intern(const char* p_str);
    const char* get(usize p_id) const;

    IStringArena copy() const;
    void drop() const;
};

struct IString {
    usize m_id;
    IStringArena* m_arena;

    static IString from(const char* p_str);
    static IString from(const char* p_str, IStringArena* p_arena);

    usize length() const;
    const char* get() const;
};

bool istring_eq(IString p_istr1, IString p_istr2);