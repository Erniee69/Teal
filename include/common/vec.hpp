#pragma once
#include "defs.hpp"
#include "concepts.hpp"
#include "util.hpp"

template <typename T>
struct Vec {
    usize m_len;
    usize m_cap;
    T* m_data;

    static Vec<T> empty();
    static Vec<T> with_capacity(usize p_cap);

    void append(T p_elem);
    T* at(usize p_idx);

    void clear();

    Vec<T> copy() const;
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
    T* data = gmalloc<T>(p_cap);
    return (Vec<T>) {
        .m_len = 0,
        .m_cap = p_cap,
        .m_data = data,
    };
}

template <typename T>
void Vec<T>::append(T p_elem) {
    if (m_len == m_cap) {
        m_cap = m_cap ? 2 * m_cap : 2;
        m_data = grealloc<T>(m_data, m_cap);
    }
    m_data[m_len++] = p_elem;
}

template <typename T>
T* Vec<T>::at(usize p_idx) {
    return &m_data[p_idx];
}

template <typename T>
void Vec<T>::clear() {
    if constexpr (Drop<T>)
    for (usize i = 0; i < m_len; i++) {
        m_data[i].drop();
    };
    m_len = 0;
}

template <typename T>
Vec<T> Vec<T>::copy() const {
    if (m_len == 0) {
        return Vec<T>::empty();
    }
    T* new_data = gmalloc<T>(m_len);
    for (usize i = 0; i < m_len; i++) {
        if constexpr (Copy<T>) {
            new_data[i] = m_data[i].copy();
        }
        else {
            new_data[i] = m_data[i];
        }
    }
    return (Vec<T>) {
        .m_len = m_len,
        .m_cap = m_len,
        .m_data = new_data,
    };
}

template <typename T>
void Vec<T>::drop() const {
    if constexpr (Drop<T>)
    for (usize i = 0; i < m_len; i++) {
        m_data[i].drop();
    };
    free(m_data);
}