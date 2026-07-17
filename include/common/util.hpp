#pragma once
#include <stdlib.h>
#include <stdio.h>
#include "defs.hpp"

#ifdef DEBUG
    #define panic(msg) ({\
        fprintf(stderr, "[%s:%d] %s\n", __FILE__, __LINE__, (msg));\
        __builtin_trap();\
    })

    #define assert(cond, msg) ({\
        if (!(cond)) {\
            fprintf(stderr, "[%s:%d] Assertion Failed: %s\n", __FILE__, __LINE__, (msg));\
            __builtin_trap();\
        }\
    })

    #define panic_fmt(fmt, ...) ({\
        fprintf(stderr, fmt __VA_OPT__(,) __VA_ARGS__);\
        __builtin_trap();\
    })

    #define assert_fmt(cond, fmt, ...) ({\
        if (!(cond)) {\
            fprintf(stderr, fmt __VA_OPT__(,) __VA_ARGS__);\
            __builtin_trap();\
        }\
    })
#else
    #define panic(msg) ({\
        fprintf(stderr, "[%s:%d] %s\n", __FILE__, __LINE__, (msg));\
        exit(EXIT_FAILURE);\
    })

    #define assert(cond, msg) ({\
        if (!(cond)) {\
            fprintf(stderr, "[%s:%d] Assertion failed: %s\n", __FILE__, __LINE__, (msg));\
            exit(EXIT_FAILURE);\
        }\
    })

    #define panic_fmt(fmt, ...) ({\
        fprintf(stderr, fmt __VA_OPT__(,) __VA_ARGS__);\
        exit(EXIT_FAILURE);\
    })

    #define assert_fmt(cond, fmt, ...) ({\
        if (!(cond)) {\
            fprintf(stderr, fmt __VA_OPT__(,) __VA_ARGS__);\
            exit(EXIT_FAILURE);\
        }\
    })
#endif

template <typename T>
T* gmalloc(usize p_count) {
    T* buf = (T*) malloc(p_count * sizeof(T));
    assert(buf != nullptr, "Failed to allocate memory, aborting.");
    return buf;
}

template <typename T>
T* grealloc(T* p_ptr, usize p_count) {
    T* buf = (T*) realloc(p_ptr, p_count * sizeof(T));
    assert(buf != nullptr, "Failed to allocate memory, aborting.");
    return buf;
}