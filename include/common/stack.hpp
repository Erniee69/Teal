#pragma once
#include "defs.hpp"
#include "concepts.hpp"
#include "util.hpp"

template <typename T>
struct Stack {
    usize m_len;
    usize m_cap;
    T* m_data;

    static Stack<T> empty();
    static Stack<T> with_capacity(usize p_cap);

    bool is_empty() const;

    void push(T p_elem);
    bool pop(T* r_elem);

    Stack<T> copy() const;
    void drop() const;
};

template <typename T>
Stack<T> Stack<T>::empty() {
    return (Stack<T>) {
        .m_len = 0,
        .m_cap = 0,
        .m_data = nullptr,
    };
}

template <typename T>
Stack<T> Stack<T>::with_capacity(usize p_cap) {
    T* data = gmalloc<T>(p_cap);
    return (Stack<T>) {
        .m_len = 0,
        .m_cap = p_cap,
        .m_data = data,
    };
}

template <typename T>
bool Stack<T>::is_empty() const {
    return m_len == 0;
}

template <typename T>
void Stack<T>::push(T p_elem) {
    if (m_len == m_cap) {
        m_cap = m_cap ? 2 * m_cap : 2;
        m_data = grealloc<T>(m_data, m_cap);
    }
    m_data[m_len++] = p_elem;
}

template <typename T>
bool Stack<T>::pop(T* r_elem) {
    if (m_len == 0) return false;
    *r_elem = m_data[--m_len];
    return true;
}

template <typename T>
Stack<T> Stack<T>::copy() const {
    if (m_len == 0) {
        return Stack<T>::empty();
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
    return (Stack<T>) {
        .m_len = m_len,
        .m_cap = m_len,
        .m_data = new_data,
    };
}

template <typename T>
void Stack<T>::drop() const {
    if constexpr (Drop<T>)
    for (usize i = 0; i < m_len; i++) {
        m_data[i].drop();
    };
    free(m_data);
}