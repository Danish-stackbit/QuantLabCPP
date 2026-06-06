/**
 * @file volatility_factors.cpp
 * @brief Implementation of volatility factors (ATR, Historical Volatility)
 */

#include "factor_engine.h"
#include <cmath>

namespace quantlab {

std::vector<double> FactorEngine::calculate_atr(
    const std::vector<Bar>& bars, int period) {
    
    if (bars.size() < static_cast<size_t>(period + 1)) {
        return {};
    }
    
    std::vector<double> true_ranges;
    true_ranges.reserve(bars.size() - 1);
    
    // Calculate True Range for each bar
    // TR = max(H-L, |H-C_prev|, |L-C_prev|)
    for (size_t i = 1; i < bars.size(); ++i) {
        double high_low = bars[i].high - bars[i].low;
        double high_close_prev = std::abs(bars[i].high - bars[i-1].close);
        double low_close_prev = std::abs(bars[i].low - bars[i-1].close);
        
        double tr = std::max({high_low, high_close_prev, low_close_prev});
        true_ranges.push_back(tr);
    }
    
    // ATR is SMA of true ranges
    return calculate_sma(true_ranges, period);
}

std::vector<double> FactorEngine::calculate_historical_volatility(
    const std::vector<double>& prices, int period, bool annualize) {
    
    if (prices.size() < static_cast<size_t>(period + 1)) {
        return {};
    }
    
    std::vector<double> vol;
    vol.reserve(prices.size() - period);
    
    // Calculate log returns
    std::vector<double> returns;
    for (size_t i = 1; i < prices.size(); ++i) {
        returns.push_back(std::log(prices[i] / prices[i-1]));
    }
    
    // Rolling volatility calculation
    for (size_t i = period - 1; i < returns.size(); ++i) {
        // Calculate mean of returns in window
        double sum = 0.0;
        for (int j = 0; j < period; ++j) {
            sum += returns[i - j];
        }
        double mean = sum / period;
        
        // Calculate variance
        double var_sum = 0.0;
        for (int j = 0; j < period; ++j) {
            double diff = returns[i - j] - mean;
            var_sum += diff * diff;
        }
        double variance = var_sum / (period - 1);  // Sample variance
        
        // Standard deviation (volatility)
        double sigma = std::sqrt(variance);
        
        // Annualize if requested (assuming daily data)
        if (annualize) {
            sigma *= std::sqrt(252);
        }
        
        vol.push_back(sigma);
    }
    
    return vol;
}

double FactorEngine::std_dev(const std::vector<double>& values) {
    if (values.size() < 2) {
        return 0.0;
    }
    
    // Calculate mean
    double sum = std::accumulate(values.begin(), values.end(), 0.0);
    double mean = sum / values.size();
    
    // Calculate variance
    double var_sum = 0.0;
    for (const auto& v : values) {
        double diff = v - mean;
        var_sum += diff * diff;
    }
    
    // Sample standard deviation
    return std::sqrt(var_sum / (values.size() - 1));
}

double FactorEngine::variance(const std::vector<double>& values) {
    double sd = std_dev(values);
    return sd * sd;
}

} // namespace quantlab
