#include "common/istring.hpp"
#include "common/util.hpp"
#include <string.h>

static IStringArena _global_istring_arena = IStringArena::empty();

IStringArena IStringArena::empty() {
    return (IStringArena) {
        .m_len = 0,
        .m_cap = 0,
        .m_data = nullptr,
    };
}

usize IStringArena::intern(const char* p_str) {
    for (usize i = 0; i < m_len; i++) {
        if (!strcmp(m_data[i], p_str)) {
            return i;
        }
    }
    usize len = strlen(p_str) + 1;
    char* buf = gmalloc<char>(len);
    if (m_len == m_cap) {
        m_cap = m_cap ? 2 * m_cap : 4;
        m_data = grealloc<char*>(m_data, m_cap);
    }
    usize id = m_len++;
    m_data[id] = strcpy(buf, p_str);
    return id;
}

const char* IStringArena::get(usize p_id) const {
    if (p_id >= m_len) return nullptr;
    return m_data[p_id];
}

IStringArena IStringArena::copy() const {
    IStringArena result;
    result.m_data = gmalloc<char*>(m_len);
    for (usize i = 0; i < m_len; i++) {
        usize len = strlen(m_data[i]) + 1;
        char* buf = gmalloc<char>(len);
        result.m_data[i] = strcpy(buf, m_data[i]);
    }
    result.m_len = m_len;
    result.m_cap = m_cap;
    return result;
}

void IStringArena::drop() const {
    for (usize i = 0; i < m_len; i++) {
        free(m_data[i]);
    }
    free(m_data);
}

IString IString::from(const char* p_str) {
    return IString::from(p_str, &_global_istring_arena);
}

IString IString::from(const char* p_str, IStringArena* p_arena) {
    usize id = p_arena->intern(p_str);
    return (IString) {
        .m_id = id,
        .m_arena = p_arena,
    };
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