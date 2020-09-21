#include "logging.h"

namespace dousi {
static spdlog::level::level_enum GetMappedSeverity(LOG_LEVEL severity) {
    switch (severity) {
    case LOG_LEVEL::DEBUG:
        return spdlog::level::debug;
    case LOG_LEVEL::INFO:
        return spdlog::level::info;
    case LOG_LEVEL::WARNING:
        return spdlog::level::warn;
    case LOG_LEVEL::FATAL:
        return spdlog::level::critical;
    default:
        return spdlog::level::critical;
    }
}

    DousiLog::DousiLog(const char *file_name, int line_number, LOG_LEVEL severity)
    : filename_(file_name),
      line_number_(line_number),
      log_level_(std::move(severity)),
      is_enabled_(severity >= severity_threshold_) {}

    DousiLog::~DousiLog() {
    try {
        if (is_enabled_) {
            logging_provider->log(GetMappedSeverity(log_level_), "in {} line:{} {}",
                                  filename_, line_number_, ss_.str());
        }
    } catch (const spdlog::spdlog_ex &ex) {
        std::cout << "logging_provider->log failed: " << ex.what() << std::endl;
    }
}

void DousiLog::StartDousiLog(const std::string &log_file_name,
                             LOG_LEVEL severity, uint32_t log_file_roll_size_mb,
                                 uint32_t log_file_roll_cout) {
    severity_threshold_ = severity;
    if (logging_provider == nullptr) {
        try {
            logging_provider = ::spdlog::rotating_logger_mt(
                "dousi_log", log_file_name, 1024 * 1024 * 1024 * log_file_roll_size_mb,
                log_file_roll_cout);
            spdlog::set_level(GetMappedSeverity(severity));
            logging_provider->flush_on(spdlog::level::debug);
        } catch (const spdlog::spdlog_ex &ex) {
            std::cout << "DousiLog failed: " << ex.what() << std::endl;
        }
    }
}

bool DousiLog::IsEnabled() const { return is_enabled_; }

bool DousiLog::IsLevelEnabled(LOG_LEVEL log_level) {
    return log_level >= severity_threshold_;
}

void DousiLog::ShutDownDousiLog() { spdlog::shutdown(); }

std::shared_ptr<spdlog::logger> DousiLog::logging_provider = nullptr;
LOG_LEVEL DousiLog::severity_threshold_ = LOG_LEVEL::NOLEVEL;

}  // namespace raftcpp
