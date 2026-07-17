#include "common/istring.hpp"
#include <stdlib.h>
#include <string.h>

static IStringArena _global_istring_arena = IStringArena::empty();

IStringArena IStringArena::empty() {
    return (IStringArena) {
        .m_len = 0,
        .m_cap = 0,
        .m_data = nullptr,
    };
}

Option<usize> IStringArena::intern(const char* p_str) {
    for (usize i = 0; i < m_len; i++) {
        if (!strcmp(m_data[i], p_str)) {
            return Option<usize>::some(i);
        }
    }
    usize len = strlen(p_str) + 1;
    char* buf = (char*) malloc(len * sizeof(char));
    if (!buf) return Option<usize>::none();
    if (m_len == m_cap) {
        usize new_cap = m_cap ? 2 * m_cap : 4;
        char** new_data = (char**) realloc((void*) m_data, new_cap * sizeof(char*));
        if (!new_data) {
            free(buf);
            return Option<usize>::none();
        };
        m_cap = new_cap;
        m_data = new_data;
    }
    usize id = m_len++;
    m_data[id] = strcpy(buf, p_str);
    return Option<usize>::some(id);
}

const char* IStringArena::get(usize p_id) const {
    if (p_id >= m_len) return nullptr;
    return m_data[p_id];
}

bool IStringArena::copy(IStringArena* p_dest) const {
    p_dest->m_data = (char**) malloc(m_len * sizeof(char*));
    if (!p_dest->m_data) return false;
    for (usize i = 0; i < m_len; i++) {
        usize len = strlen(m_data[i]) + 1;
        char* buf = (char*) malloc(len * sizeof(char));
        if (!buf) {
            for (usize j = 0; j < i; j++) {
                free(p_dest->m_data[j]);
            }
            free(p_dest->m_data);
            return false;
        }
        p_dest->m_data[i] = strcpy(buf, m_data[i]);
    }
    p_dest->m_len = m_len;
    p_dest->m_cap = m_cap;
    return true;
}

void IStringArena::drop() const {
    for (usize i = 0; i < m_len; i++) {
        free(m_data[i]);
    }
    free(m_data);
}

Option<IString> IString::from(const char* p_str) {
    return IString::from(p_str, &_global_istring_arena);
}

Option<IString> from(const char* p_str, IStringArena* p_arena) {
    Option<usize> opt_id = p_arena->intern(p_str);
    if (!opt_id.m_ok) return Option<IString>::none();
    usize id = opt_id.unwrap();
    return Option<IString>::some((IString) {
        .m_id = id,
        .m_arena = p_arena,
    });
}

usize IString::length() const {
    return strlen(this->get());
}

const char* IString::get() const {
    return m_arena->get(m_id);
}

bool istring_eq(IString p_istr1, IString p_istr2) {
    return (p_istr1.m_arena == p_istr2.m_arena) ? (p_istr1.m_id == p_istr2.m_id) : !strcmp(p_istr1.get(), p_istr2.get());
}