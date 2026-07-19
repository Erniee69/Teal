#include "common/error.hpp"
#include <cstdarg>
#include <cstdio>

void Error::report(FILE* p_log) {
    const char* map[] = {
        [(usize) ErrorKind::Info] = "Info",
        [(usize) ErrorKind::Warning] = "Warning",
        [(usize) ErrorKind::Error] = "Error",
        [(usize) ErrorKind::Fatal] = "Fatal",
    };
    fprintf(p_log, "%s: %s\n", map[(usize) m_kind], m_msg);
}

void Error::drop() const {
    free(m_msg);
}

ErrorCtx ErrorCtx::empty() {
    return (ErrorCtx) {
        .m_fatal = false,
        .m_errors = Vec<Error>::empty(),
    };
}

void ErrorCtx::post_info(const char* p_fmt, ...) {
    va_list args;
    va_start(args, p_fmt);
    this->vpost(ErrorKind::Info, p_fmt, args);
    va_end(args);
}

void ErrorCtx::post_warning(const char* p_fmt, ...) {
    va_list args;
    va_start(args, p_fmt);
    this->vpost(ErrorKind::Warning, p_fmt, args);
    va_end(args);
}

void ErrorCtx::post_error(const char* p_fmt, ...) {
    va_list args;
    va_start(args, p_fmt);
    this->vpost(ErrorKind::Error, p_fmt, args);
    va_end(args);
}

void ErrorCtx::post_fatal(const char* p_fmt, ...) {
    va_list args;
    va_start(args, p_fmt);
    this->vpost(ErrorKind::Fatal, p_fmt, args);
    va_end(args);
}

void ErrorCtx::vpost(ErrorKind p_kind, const char* p_fmt, va_list p_args) {
    if (p_kind == ErrorKind::Fatal) {
        m_fatal = true;
    }
    char* msg;
    vasprintf(&msg, p_fmt, p_args);
    assert(msg != nullptr, "Failed to allocate memory, aborting.");
    m_errors.append((Error) {
        .m_kind = p_kind,
        .m_msg = msg,
    });
};

void ErrorCtx::report(FILE* p_log, bool p_print_success) {
    if (m_errors.m_len == 0 && p_print_success) {
        fprintf(p_log, "No errors occured\n");
        return;
    }
    for (usize i = 0; i < m_errors.m_len; i++) {
        m_errors.at(i)->report(p_log);
    }
}

void ErrorCtx::drop() const {
    m_errors.drop();
}