/**
 * @file base_strategy.cpp
 * @brief Implementation of base strategy and concrete strategies
 */

#include "strategy.h"
#include "factor_engine.h"

namespace quantlab {

// ============================================================================
// BASE STRATEGY
// ============================================================================

BaseStrategy::BaseStrategy(const std::string& name, const std::string& params)
    : name_(name), params_(params), is_ready_(false), bar_count_(0) {}

void BaseStrategy::init() {
    is_ready_ = true;
}

void BaseStrategy::reset() {
    bar_count_ = 0;
    is_ready_ = false;
}

void BaseStrategy::on_bar(const Bar& bar) {
    (void)bar;  // Suppress unused warning
    bar_count_++;
}

// ============================================================================
// MA CROSSOVER STRATEGY
// ============================================================================

MACrossoverStrategy::MACrossoverStrategy(int fast_period, int slow_period)
    : BaseStrategy("MA_Crossover", "fast=" + std::to_string(fast_period) + 
                   ",slow=" + std::to_string(slow_period)),
      fast_period_(fast_period), slow_period_(slow_period), prev_above_(false) {}

Signal MACrossoverStrategy::generate_signal(const std::vector<Bar>& bars) {
    Signal sig;
    sig.timestamp = bars.back().timestamp;
    
    if (bars.size() < static_cast<size_t>(slow_period_)) {
        return sig;  // Not enough data
    }
    
    // Extract closing prices
    std::vector<double> closes;
    for (const auto& bar : bars) {
        closes.push_back(bar.close);
    }
    
    // Calculate SMAs
    auto fast_sma = FactorEngine::calculate_sma(closes, fast_period_);
    auto slow_sma = FactorEngine::calculate_sma(closes, slow_period_);
    
    if (fast_sma.empty() || slow_sma.empty()) {
        return sig;
    }
    
    double current_fast = fast_sma.back();
    double current_slow = slow_sma.back();
    bool currently_above = current_fast > current_slow;
    
    // Check for crossover
    if (currently_above && !prev_above_) {
        // Golden cross: fast crosses above slow
        sig.type = SignalType::LONG;
        sig.strength = (current_fast - current_slow) / current_slow;
        sig.reason = "Golden Cross";
    } else if (!currently_above && prev_above_) {
        // Death cross: fast crosses below slow
        sig.type = SignalType::SHORT;
        sig.strength = (current_slow - current_fast) / current_slow;
        sig.reason = "Death Cross";
    } else {
        sig.type = SignalType::HOLD;
    }
    
    prev_above_ = currently_above;
    return sig;
}

// ============================================================================
// MOMENTUM STRATEGY
// ============================================================================

MomentumStrategy::MomentumStrategy(int lookback, double threshold)
    : BaseStrategy("Momentum", "lookback=" + std::to_string(lookback) + 
                   ",threshold=" + std::to_string(threshold)),
      lookback_(lookback), threshold_(threshold) {}

Signal MomentumStrategy::generate_signal(const std::vector<Bar>& bars) {
    Signal sig;
    sig.timestamp = bars.back().timestamp;
    
    if (bars.size() < static_cast<size_t>(lookback_ + 1)) {
        return sig;
    }
    
    // Calculate momentum
    std::vector<double> closes;
    for (const auto& bar : bars) {
        closes.push_back(bar.close);
    }
    
    auto momentum = FactorEngine::calculate_momentum(closes, lookback_);
    
    if (momentum.empty()) {
        return sig;
    }
    
    double mom = momentum.back();
    double prev_price = closes[closes.size() - lookback_ - 1];
    double mom_pct = mom / prev_price;
    
    if (mom_pct > threshold_) {
        sig.type = SignalType::LONG;
        sig.strength = mom_pct;
        sig.reason = "Positive momentum";
    } else if (mom_pct < -threshold_) {
        sig.type = SignalType::SHORT;
        sig.strength = -mom_pct;
        sig.reason = "Negative momentum";
    } else {
        sig.type = SignalType::HOLD;
    }
    
    return sig;
}

// ============================================================================
// MEAN REVERSION STRATEGY
// ============================================================================

MeanReversionStrategy::MeanReversionStrategy(int ma_period, double z_threshold)
    : BaseStrategy("Mean_Reversion", "ma=" + std::to_string(ma_period) + 
                   ",z=" + std::to_string(z_threshold)),
      ma_period_(ma_period), z_threshold_(z_threshold) {}

Signal MeanReversionStrategy::generate_signal(const std::vector<Bar>& bars) {
    Signal sig;
    sig.timestamp = bars.back().timestamp;
    
    if (bars.size() < static_cast<size_t>(ma_period_ + 1)) {
        return sig;
    }
    
    // Get recent prices
    std::vector<double> closes;
    size_t start = std::max(static_cast<int>(bars.size()) - ma_period_ - 1, 0);
    for (size_t i = start; i < bars.size(); ++i) {
        closes.push_back(bars[i].close);
    }
    
    // Calculate mean and std dev
    double sum = 0.0;
    for (auto p : closes) sum += p;
    double mean = sum / closes.size();
    
    double var_sum = 0.0;
    for (auto p : closes) {
        double diff = p - mean;
        var_sum += diff * diff;
    }
    double std_dev = std::sqrt(var_sum / (closes.size() - 1));
    
    // Calculate z-score of current price
    double current_price = bars.back().close;
    double z_score = (current_price - mean) / std_dev;
    
    if (z_score < -z_threshold_) {
        // Price significantly below mean - buy
        sig.type = SignalType::LONG;
        sig.strength = -z_score / z_threshold_;
        sig.reason = "Price below mean (oversold)";
    } else if (z_score > z_threshold_) {
        // Price significantly above mean - sell
        sig.type = SignalType::SHORT;
        sig.strength = z_score / z_threshold_;
        sig.reason = "Price above mean (overbought)";
    } else {
        sig.type = SignalType::HOLD;
    }
    
    return sig;
}

// ============================================================================
// BREAKOUT STRATEGY
// ============================================================================

BreakoutStrategy::BreakoutStrategy(int lookback)
    : BaseStrategy("Breakout", "lookback=" + std::to_string(lookback)),
      lookback_(lookback), prev_high_(0), prev_low_(0) {}

Signal BreakoutStrategy::generate_signal(const std::vector<Bar>& bars) {
    Signal sig;
    sig.timestamp = bars.back().timestamp;
    
    if (bars.size() < static_cast<size_t>(lookback_ + 1)) {
        return sig;
    }
    
    // Find N-period high and low (excluding current bar)
    Price highest = 0;
    Price lowest = std::numeric_limits<Price>::max();
    
    for (int i = 1; i <= lookback_; ++i) {
        size_t idx = bars.size() - i - 1;
        if (bars[idx].high > highest) highest = bars[idx].high;
        if (bars[idx].low < lowest) lowest = bars[idx].low;
    }
    
    Price current_price = bars.back().close;
    
    if (current_price > highest) {
        // Breakout above resistance
        sig.type = SignalType::LONG;
        sig.strength = (current_price - highest) / highest;
        sig.reason = "Breakout above resistance";
    } else if (current_price < lowest) {
        // Breakdown below support
        sig.type = SignalType::SHORT;
        sig.strength = (lowest - current_price) / lowest;
        sig.reason = "Breakdown below support";
    } else {
        sig.type = SignalType::HOLD;
    }
    
    prev_high_ = highest;
    prev_low_ = lowest;
    
    return sig;
}

} // namespace quantlab
