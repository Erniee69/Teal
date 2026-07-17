#pragma once
#include "concepts.hpp"

template <typename T>
struct Option {
    bool m_ok;
    T m_data;

    static Option<T> some(T p_data);
    static Option<T> none();

    T unwrap();

    bool copy(Option<T>* p_dest) const;
    void drop() const;
};

template <typename T>
Option<T> Option<T>::some(T p_data) {
    return (Option<T>) {
        .m_ok = true,
        .m_data = p_data,
    };
}

template <typename T>
Option<T> Option<T>::none() {
    return (Option<T>) {
        .m_ok = false,
    };
}

template <typename T>
T Option<T>::unwrap() {
    m_ok = false;
    return m_data;
}

template <typename T>
bool Option<T>::copy(Option<T>* p_dest) const {
    if (m_ok) {
        if constexpr (Copy<T>) {
            bool succ = m_data.copy(p_dest->m_data);
            if (!succ) return false;
        }
        else {
            p_dest->m_data = m_data;
        }
    }
    p_dest->m_ok = m_ok;
    return true;
}

template <typename T>
void Option<T>::drop() const {
    if constexpr (Drop<T>) {
        if (m_ok) {
            m_data.drop();
        }
    }
}