/**
 * @file strategy.h
 * @brief Base strategy class and concrete implementations
 * 
 * TUJUAN FILE:
 * - Define interface untuk trading strategies
 * - Implementasi berbagai strategy types
 * - Support modular dan extensible design
 * 
 * KONSEP QUANT:
 * - Strategy adalah rule set untuk entry/exit
 * - Trend following: buy high, sell higher
 * - Mean reversion: buy low, sell high
 * - Breakout: trade momentum dari consolidation
 * 
 * KONSEP C++:
 * - Abstract base class dengan pure virtual functions
 * - Polymorphism untuk runtime strategy selection
 * - Factory pattern untuk strategy creation
 * 
 * HUBUNGAN DENGAN HEDGE FUND WORKFLOW:
 * - Multiple strategies run simultaneously
 * - Strategy research dan iteration
 * - Performance attribution per strategy
 */

#ifndef QUANTLABCPP_STRATEGY_H
#define QUANTLABCPP_STRATEGY_H

#include "types.h"
#include <vector>
#include <string>
#include <memory>

namespace quantlab {

/**
 * @brief Signal type dari strategy
 */
enum class SignalType : uint8_t {
    LONG = 0,      // Buy signal
    SHORT = 1,     // Sell signal
    FLAT = 2,      // Exit/close position
    HOLD = 3       // No action
};

/**
 * @brief Trading signal dari strategy
 */
struct Signal {
    Timestamp timestamp;
    Symbol symbol;
    SignalType type;
    double strength;        // Signal strength (-1 to 1)
    std::string reason;     // Why this signal was generated
    
    Signal() : timestamp(0), type(SignalType::HOLD), strength(0) {}
};

/**
 * @brief Base class untuk semua strategies
 * 
 * DESIGN PATTERN: Strategy Pattern
 * - Encapsulates different algorithms
 * - Makes them interchangeable
 * - Lets algorithm vary independently from clients
 */
class BaseStrategy {
public:
    /**
     * @brief Constructor
     * @param name Strategy name
     * @param params Strategy parameters
     */
    BaseStrategy(const std::string& name, const std::string& params = "");
    
    virtual ~BaseStrategy() = default;
    
    /**
     * @brief Generate signal based on market data
     * @param bars Historical bars up to current time
     * @return Trading signal
     * 
     * PURE VIRTUAL - must be implemented by derived classes
     */
    virtual Signal generate_signal(const std::vector<Bar>& bars) = 0;
    
    /**
     * @brief Called on each new bar (convenience method)
     * @param bar New bar data
     */
    virtual void on_bar(const Bar& bar);
    
    /**
     * @brief Initialize strategy with parameters
     */
    virtual void init();
    
    /**
     * @brief Reset strategy state
     */
    virtual void reset();
    
    /**
     * @brief Get strategy name
     */
    const std::string& name() const { return name_; }
    
    /**
     * @brief Get strategy parameters
     */
    const std::string& params() const { return params_; }
    
    /**
     * @brief Check if strategy is ready (warmup complete)
     */
    bool is_ready() const { return is_ready_; }

protected:
    std::string name_;
    std::string params_;
    bool is_ready_;
    size_t bar_count_;
};

// ============================================================================
// CONCRETE STRATEGIES
// ============================================================================

/**
 * @brief Moving Average Crossover Strategy
 * 
 * LOGIC:
 * - Buy when fast MA crosses above slow MA (golden cross)
 * - Sell when fast MA crosses below slow MA (death cross)
 * 
 * PARAMETERS:
 * - fast_period: Fast MA period (e.g., 10)
 * - slow_period: Slow MA period (e.g., 50)
 */
class MACrossoverStrategy : public BaseStrategy {
public:
    MACrossoverStrategy(int fast_period = 10, int slow_period = 50);
    
    Signal generate_signal(const std::vector<Bar>& bars) override;
    
private:
    int fast_period_;
    int slow_period_;
    bool prev_above_;  // Was fast above slow in previous bar?
};

/**
 * @brief Momentum Strategy
 * 
 * LOGIC:
 * - Buy stocks with positive momentum
 * - Sell stocks with negative momentum
 * - Based on rate of change over N periods
 * 
 * PARAMETERS:
 * - lookback: Momentum lookback period (e.g., 20)
 * - threshold: Minimum momentum to trigger signal
 */
class MomentumStrategy : public BaseStrategy {
public:
    MomentumStrategy(int lookback = 20, double threshold = 0.0);
    
    Signal generate_signal(const std::vector<Bar>& bars) override;
    
private:
    int lookback_;
    double threshold_;
};

/**
 * @brief Mean Reversion Strategy
 * 
 * LOGIC:
 * - Buy when price is significantly below moving average
 * - Sell when price is significantly above moving average
 * - Assumes price will revert to mean
 * 
 * PARAMETERS:
 * - ma_period: Moving average period
 * - z_threshold: Number of standard deviations for entry
 */
class MeanReversionStrategy : public BaseStrategy {
public:
    MeanReversionStrategy(int ma_period = 20, double z_threshold = 2.0);
    
    Signal generate_signal(const std::vector<Bar>& bars) override;
    
private:
    int ma_period_;
    double z_threshold_;
};

/**
 * @brief Breakout Strategy
 * 
 * LOGIC:
 * - Buy when price breaks above resistance (N-period high)
 * - Sell when price breaks below support (N-period low)
 * - Trade momentum from consolidation
 * 
 * PARAMETERS:
 * - lookback: Lookback period for high/low (e.g., 20)
 */
class BreakoutStrategy : public BaseStrategy {
public:
    BreakoutStrategy(int lookback = 20);
    
    Signal generate_signal(const std::vector<Bar>& bars) override;
    
private:
    int lookback_;
    Price prev_high_;
    Price prev_low_;
};

} // namespace quantlab

#endif // QUANTLABCPP_STRATEGY_H
