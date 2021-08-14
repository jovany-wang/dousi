#pragma once

#include <iostream>
#include <sstream>
#include <string>

#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/spdlog.h"

namespace dousi {
    enum LOG_LEVEL {
        DEBUG,
        INFO,
        WARNING,
        FATAL,
        NOLEVEL
    };

class DousiLogBase {
public:
    virtual ~DousiLogBase(){};

    virtual bool IsEnabled() const { return false; };

    template <typename T>
    DousiLogBase &operator<<(const T &t) {
        if (IsEnabled()) {
            ss_ << t;
        }
        return *this;
    }

protected:
    std::stringstream ss_;
};

class DousiLog : public DousiLogBase {
public:
    DousiLog(const char *file_name, int line_number, LOG_LEVEL severity);

    ~DousiLog();

    static void StartDousiLog(const std::string &log_file_name,
                                LOG_LEVEL severity, uint32_t log_file_roll_size_mb,
                                uint32_t log_file_roll_cout);

    bool IsEnabled() const;

    static bool IsLevelEnabled(LOG_LEVEL log_level);

    static void ShutDownDousiLog();

private:
    bool is_enabled_;
    LOG_LEVEL log_level_;
    std::string filename_;
    int line_number_;
    static std::shared_ptr<spdlog::logger> logging_provider;
    static LOG_LEVEL severity_threshold_;

protected:
};

class Voidify {
public:
    Voidify() {}

    void operator&(DousiLogBase &) {}
};

#define DOUSI_LOG_INTERNAL(level) ::dousi::DousiLog(__FILE__, __LINE__, level)
#define DOUSI_LOG(level)                                                    \
    if (dousi::DousiLog::IsLevelEnabled(dousi::LOG_LEVEL::level)) \
    DOUSI_LOG_INTERNAL(dousi::LOG_LEVEL::level)

#define DOUSI_LOG_ENABLED(level) \
    dousi::DousiLog::IsLevelEnabled(dousi::LOG_LEVEL::level)
#define DOUSI_IGNORE_EXPR(expr) ((void)(expr))
#define DOUSI_CHECK(condition)                                                 \
    (condition) ? DOUSI_IGNORE_EXPR(0)                                         \
                : ::dousi::Voidify() &                                         \
                      ::dousi::DousiLog(__FILE__, __LINE__,                  \
                                            dousi::LOG_LEVEL::FATAL) \
                          << " Check failed: " #condition " "
}  // namespace dousi
