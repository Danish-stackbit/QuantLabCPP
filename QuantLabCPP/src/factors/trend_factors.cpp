/**
 * @file trend_factors.cpp
 * @brief Implementation of trend factors (SMA, EMA, Momentum)
 */

#include "factor_engine.h"
#include <numeric>

namespace quantlab {

std::vector<double> FactorEngine::calculate_sma(
    const std::vector<double>& prices, int period) {
    
    if (prices.size() < static_cast<size_t>(period)) {
        return {};
    }
    
    std::vector<double> sma;
    sma.reserve(prices.size() - period + 1);
    
    // Calculate first SMA using std::accumulate
    double sum = std::accumulate(prices.begin(), prices.begin() + period, 0.0);
    sma.push_back(sum / period);
    
    // Use sliding window for efficiency
    for (size_t i = period; i < prices.size(); ++i) {
        sum = sum - prices[i - period] + prices[i];
        sma.push_back(sum / period);
    }
    
    return sma;
}

std::vector<double> FactorEngine::calculate_ema(
    const std::vector<double>& prices, int period) {
    
    if (prices.empty()) {
        return {};
    }
    
    std::vector<double> ema;
    ema.reserve(prices.size());
    
    // Smoothing factor α = 2 / (period + 1)
    double alpha = 2.0 / (period + 1);
    
    // Initialize with SMA for first value
    if (prices.size() >= static_cast<size_t>(period)) {
        double init_sum = std::accumulate(prices.begin(), prices.begin() + period, 0.0);
        ema.push_back(init_sum / period);
    } else {
        ema.push_back(prices[0]);
    }
    
    // Calculate EMA recursively: EMA = α × P + (1-α) × EMA_prev
    for (size_t i = ema.size(); i < prices.size(); ++i) {
        double next_ema = alpha * prices[i] + (1 - alpha) * ema.back();
        ema.push_back(next_ema);
    }
    
    return ema;
}

std::vector<double> FactorEngine::calculate_momentum(
    const std::vector<double>& prices, int period) {
    
    if (prices.size() < static_cast<size_t>(period)) {
        return {};
    }
    
    std::vector<double> momentum;
    momentum.reserve(prices.size() - period);
    
    // Momentum = P_today - P_n_periods_ago
    for (size_t i = period; i < prices.size(); ++i) {
        momentum.push_back(prices[i] - prices[i - period]);
    }
    
    return momentum;
}

} // namespace quantlab
