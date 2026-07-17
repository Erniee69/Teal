#pragma once
#include "concepts.hpp"
#include "defs.hpp"
#include "util.hpp"

template <typename T>
struct Tree {
    T m_elem;
    usize m_children_len;
    usize m_children_cap;
    Tree<T>** m_children;

    static Tree<T>* from(T p_elem);

    void add_child(T p_child);
    void add_child(Tree<T>* p_child);

    Tree<T>* copy() const;
    void drop() const;
};

template <typename T>
Tree<T>* Tree<T>::from(T p_elem) {
    Tree<T>* result = gmalloc<Tree<T>>(1);
    *result = (Tree<T>) {
        .m_elem = p_elem,
        .m_children_len = 0,
        .m_children_cap = 0,
        .m_children = nullptr,
    };
    return result;
}

template <typename T>
void Tree<T>::add_child(T p_child) {
    this->add_child(Tree<T>::from(p_child));
}

template <typename T>
void Tree<T>::add_child(Tree<T>* p_child) {
    if (m_children_len == m_children_cap) {
        m_children_cap = m_children_cap ? 2 * m_children_cap : 2;
        m_children = grealloc(m_children, m_children_cap);
    }
    m_children[m_children_len++] = p_child;
}

template <typename T>
Tree<T>* Tree<T>::copy() const {
    Tree<T>* result = gmalloc<Tree<T>>(1);
    if constexpr (Copy<T>) {
        result->m_elem = m_elem.copy();
    }
    else {
        result->m_elem = m_elem;
    }
    result->m_children = gmalloc<Tree<T>*>(m_children_len);
    for (usize i = 0; i < m_children_len; i++) {
        result->m_children[i] = m_children[i]->copy();
    }
    result->m_children_len = m_children_len;
    result->m_children_cap = m_children_cap;
    return result;
}

template <typename T>
void Tree<T>::drop() const {
    for (usize i = 0; i < m_children_len; i++) {
        m_children[i]->drop();
    }
    free(m_children);
    if constexpr (Drop<T>) m_elem.drop();
}