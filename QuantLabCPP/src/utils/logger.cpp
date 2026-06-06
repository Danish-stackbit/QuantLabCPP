/**
 * @file logger.cpp
 * @brief Logger implementation using spdlog
 */

#include "logger.h"

#ifdef SPDLOG_AVAILABLE
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#endif

namespace quantlab {

Logger& Logger::instance() {
    static Logger instance;
    return instance;
}

void Logger::init(const std::string& log_file, const std::string& level) {
#ifdef SPDLOG_AVAILABLE
    try {
        // Create multi-sink logger (file + console)
        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(log_file, true);
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        
        logger_ = std::make_shared<spdlog::logger>("quantlab", spdlog::sinks_init_list{file_sink, console_sink});
        
        // Set pattern
        logger_->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");
        
        // Set level
        set_level(level);
        
        // Register as default logger
        spdlog::set_default_logger(logger_);
    } catch (...) {
        // Fallback: no logging
        logger_ = nullptr;
    }
#else
    (void)log_file;
    (void)level;
#endif
}

void Logger::set_level(const std::string& level) {
#ifdef SPDLOG_AVAILABLE
    if (!logger_) return;
    
    if (level == "trace") {
        logger_->set_level(spdlog::level::trace);
    } else if (level == "debug") {
        logger_->set_level(spdlog::level::debug);
    } else if (level == "info") {
        logger_->set_level(spdlog::level::info);
    } else if (level == "warn") {
        logger_->set_level(spdlog::level::warn);
    } else if (level == "error") {
        logger_->set_level(spdlog::level::err);
    } else if (level == "critical") {
        logger_->set_level(spdlog::level::critical);
    }
#endif
}

// Template implementations would be in header for inline expansion
// For now, provide empty implementations to satisfy linker

template<>
void Logger::trace<>(const std::string&, ...) {}

template<>
void Logger::debug<>(const std::string&, ...) {}

template<>
void Logger::info<>(const std::string&, ...) {}

template<>
void Logger::warn<>(const std::string&, ...) {}

template<>
void Logger::error<>(const std::string&, ...) {}

template<>
void Logger::critical<>(const std::string&, ...) {}

} // namespace quantlab
