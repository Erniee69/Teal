#pragma once
#include "defs.hpp"
#include <stdlib.h>
#include "concepts.hpp"

template <typename T>
struct Vec {
    usize m_len;
    usize m_cap;
    T* m_data;

    static Vec<T> empty();
    static Vec<T> with_capacity(usize p_cap);

    bool append(T p_elem);

    bool copy(Vec<T>* r_dest) const;

    void drop() const;
};

template <typename T>
Vec<T> Vec<T>::empty() {
    return (Vec<T>) {
        .m_len = 0,
        .m_cap = 0,
        .m_data = nullptr,
    };
}

template <typename T>
Vec<T> Vec<T>::with_capacity(usize p_cap) {
    T* data = (T*) malloc(p_cap * sizeof(T));
    return (Vec<T>) {
        .m_len = 0,
        .m_cap = data ? p_cap : 0,
        .m_data = data,
    };
}

template <typename T>
bool Vec<T>::append(T p_elem) {
    if (m_len == m_cap) {
        usize new_cap = m_cap ? 2 * m_cap : 2;
        T* new_data = (T*) realloc((void*) m_data, new_cap * sizeof(T));
        if (!new_data) return false;
        m_data = new_data;
        m_cap = new_cap;
    }
    m_data[m_len++] = p_elem;
    return true;
}

template <typename T>
bool Vec<T>::copy(Vec<T>* r_dest) const {
    if (m_len == 0) {
        *r_dest = Vec<T>::empty();
        return true;
    }
    T* new_data = (T*) malloc(m_len * sizeof(T));
    if (!new_data) return false;
    if constexpr (Copy<T>) {
        for (usize i = 0; i < m_len; i++) {
            bool success = m_data[i].copy(&new_data[i]);
            if (!success) {
                if constexpr (Drop<T>)
                for (usize j = 0; j < i; j++) {
                    new_data[j].drop();
                };
                free(new_data);
                return false;
            }
        }
    } else {
        for (usize i = 0; i < m_len; i++) {
            new_data[i] = m_data[i];
        }
    }
    *r_dest = (Vec<T>) {
        .m_len = m_len,
        .m_cap = m_len,
        .m_data = new_data,
    };
    return true;
}

template <typename T>
void Vec<T>::drop() const {
    if constexpr (Drop<T>)
    for (usize i = 0; i < m_len; i++) {
        m_data[i].drop();
    };
    free(m_data);
}