/**
 * @file logger.h
 * @brief Logging utility using spdlog
 * 
 * TUJUAN FILE:
 * - Menyediakan logging infrastructure untuk debugging dan monitoring
 * - Mendukung multiple log levels (TRACE, DEBUG, INFO, WARN, ERROR, CRITICAL)
 * - Thread-safe logging dengan rotation dan formatting
 * 
 * KONSEP C++:
 * - Singleton pattern untuk global logger instance
 * - Variadic templates untuk type-safe formatting
 * - RAII untuk resource management
 * 
 * HUBUNGAN DENGAN HEDGE FUND WORKFLOW:
 * - Audit trail untuk compliance
 * - Debugging production issues
 * - Performance monitoring
 * - Risk management alerts
 */

#ifndef QUANTLABCPP_LOGGER_H
#define QUANTLABCPP_LOGGER_H

#include <string>
#include <memory>

// Forward declare spdlog logger
namespace spdlog {
    class logger;
}

namespace quantlab {

/**
 * @brief Logger singleton class
 */
class Logger {
public:
    /**
     * @brief Get singleton instance
     */
    static Logger& instance();
    
    /**
     * @brief Initialize logger with file and console output
     * @param log_file Path to log file
     * @param level Minimum log level
     */
    void init(const std::string& log_file = "quantlab.log", 
              const std::string& level = "info");
    
    /**
     * @brief Set log level
     */
    void set_level(const std::string& level);
    
    /**
     * @brief Log trace message
     */
    template<typename... Args>
    void trace(const std::string& fmt, Args&&... args);
    
    /**
     * @brief Log debug message
     */
    template<typename... Args>
    void debug(const std::string& fmt, Args&&... args);
    
    /**
     * @brief Log info message
     */
    template<typename... Args>
    void info(const std::string& fmt, Args&&... args);
    
    /**
     * @brief Log warning message
     */
    template<typename... Args>
    void warn(const std::string& fmt, Args&&... args);
    
    /**
     * @brief Log error message
     */
    template<typename... Args>
    void error(const std::string& fmt, Args&&... args);
    
    /**
     * @brief Log critical message
     */
    template<typename... Args>
    void critical(const std::string& fmt, Args&&... args);

private:
    Logger() = default;
    ~Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    std::shared_ptr<spdlog::logger> logger_;
};

// Global logger macros
#define LOG_TRACE(...) quantlab::Logger::instance().trace(__VA_ARGS__)
#define LOG_DEBUG(...) quantlab::Logger::instance().debug(__VA_ARGS__)
#define LOG_INFO(...) quantlab::Logger::instance().info(__VA_ARGS__)
#define LOG_WARN(...) quantlab::Logger::instance().warn(__VA_ARGS__)
#define LOG_ERROR(...) quantlab::Logger::instance().error(__VA_ARGS__)
#define LOG_CRITICAL(...) quantlab::Logger::instance().critical(__VA_ARGS__)

} // namespace quantlab

#endif // QUANTLABCPP_LOGGER_H
