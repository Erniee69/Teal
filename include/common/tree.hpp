#pragma once
#include "concepts.hpp"
#include "defs.hpp"
#include <stdlib.h>

template <typename T>
struct Tree {
    T m_elem;
    usize m_children_len;
    usize m_children_cap;
    Tree<T>** m_children;

    static Tree<T>* from(T p_elem);

    bool add_child(T p_child);
    bool add_child(Tree<T>* p_child);

    bool copy(Tree<T>* p_dest) const;
    void drop() const;
};

template <typename T>
Tree<T>* Tree<T>::from(T p_elem) {
    Tree<T>* result = (Tree<T>*) malloc(1 * sizeof(Tree<T>));
    if (!result) return nullptr;
    *result = (Tree<T>) {
        .m_elem = p_elem,
        .m_children_len = 0,
        .m_children_cap = 0,
        .m_children = nullptr,
    };
    return result;
}

template <typename T>
bool Tree<T>::add_child(T p_child) {
    Tree<T>* child = Tree<T>::from(p_child);
    if (!child) return false;
    bool succ = this->add_child(child);
    if (!succ) {
        child->drop();
        return false;
    }
    return true;
}

template <typename T>
bool Tree<T>::add_child(Tree<T>* p_child) {
    if (m_children_len == m_children_cap) {
        usize new_cap = m_children_cap ? 2 * m_children_cap : 2;
        Tree<T>** new_children = (Tree<T>**) realloc((void*) m_children, new_cap * sizeof(Tree<T>*));
        if (!new_children) { return false; }
        m_children_cap = new_cap;
        m_children = new_children;
    }
    m_children[m_children_len++] = p_child;
    return true;
}

template <typename T>
bool Tree<T>::copy(Tree<T>* p_dest) const {
    if constexpr (Copy<T>) {
        bool succ = m_elem.copy(&p_dest->m_elem);
        if (!succ) { return false; }
    }
    else {
        p_dest->m_elem = m_elem;
    }
    p_dest->m_children = (Tree<T>**) malloc(m_children_len * sizeof(Tree<T>*));
    if (!p_dest->m_children) {
        if constexpr (Drop<T>) p_dest->m_elem.drop();
        return false;
    }
    for (usize i = 0; i < m_children_len; i++) {
        p_dest->m_children[i] = (Tree<T>*) malloc(1 * sizeof(Tree<T>));
        bool succ = false;
        if (p_dest->m_children[i]) {
            succ = m_children[i]->copy(p_dest->m_children[i]);
        }
        if (!succ) {
            if constexpr (Drop<T>)
            for (usize j = 0; j < i; j++) {
                p_dest->m_children[j]->drop();
                free(p_dest->m_children[j]);
            };
            free(p_dest->m_children);
            if constexpr (Drop<T>) p_dest->m_elem.drop();
            return false;
        }
    }
    p_dest->m_children_len = m_children_len;
    p_dest->m_children_cap = m_children_cap;
    return true;
}

template <typename T>
void Tree<T>::drop() const {
    for (usize i = 0; i < m_children_len; i++) {
        m_children[i]->drop();
    }
    free(m_children);
    if constexpr (Drop<T>) m_elem.drop();
}