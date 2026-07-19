#pragma once
#include "common/vec.hpp"
#include <stdio.h>

enum class ErrorKind {
    Info,
    Warning,
    Error,
    Fatal,
};

struct Error {
    ErrorKind m_kind;
    char* m_msg;

    void report(FILE* p_log);

    void drop() const;
};

struct ErrorCtx {
    bool m_fatal;
    Vec<Error> m_errors;

    static ErrorCtx empty();

    void post_info(const char* p_fmt, ...);
    void post_warning(const char* p_fmt, ...);
    void post_error(const char* p_fmt, ...);
    void post_fatal(const char* p_fmt, ...);

    void report(FILE* p_log, bool p_print_success);

    void drop() const;

private:
    void vpost(ErrorKind p_kind, const char* p_msg, va_list p_args);
};