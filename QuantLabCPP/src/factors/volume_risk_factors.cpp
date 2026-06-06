/**
 * @file volume_risk_factors.cpp
 * @brief Implementation of volume and risk factors
 */

#include "factor_engine.h"
#include <cmath>

namespace quantlab {

std::vector<double> FactorEngine::calculate_obv(const std::vector<Bar>& bars) {
    if (bars.empty()) {
        return {};
    }
    
    std::vector<double> obv;
    obv.reserve(bars.size());
    
    double cumulative_obv = 0.0;
    Price prev_close = bars[0].close;
    
    for (const auto& bar : bars) {
        if (bar.close > prev_close) {
            cumulative_obv += bar.volume;
        } else if (bar.close < prev_close) {
            cumulative_obv -= bar.volume;
        }
        // If equal, OBV unchanged
        
        obv.push_back(cumulative_obv);
        prev_close = bar.close;
    }
    
    return obv;
}

std::vector<bool> FactorEngine::detect_volume_spikes(
    const std::vector<double>& volumes, int period, double threshold) {
    
    if (volumes.size() < static_cast<size_t>(period)) {
        return std::vector<bool>(volumes.size(), false);
    }
    
    std::vector<bool> spikes;
    spikes.reserve(volumes.size());
    
    for (size_t i = 0; i < volumes.size(); ++i) {
        if (i < static_cast<size_t>(period - 1)) {
            spikes.push_back(false);
            continue;
        }
        
        // Calculate average volume over lookback period
        double sum = 0.0;
        for (int j = 0; j < period; ++j) {
            sum += volumes[i - j];
        }
        double avg_volume = sum / period;
        
        // Detect spike if volume exceeds threshold × average
        bool is_spike = volumes[i] > (threshold * avg_volume);
        spikes.push_back(is_spike);
    }
    
    return spikes;
}

double FactorEngine::sharpe_ratio(
    const std::vector<double>& returns,
    double risk_free_rate,
    int periods_per_year) {
    
    if (returns.size() < 2) {
        return 0.0;
    }
    
    // Calculate mean return
    double sum = std::accumulate(returns.begin(), returns.end(), 0.0);
    double mean_return = sum / returns.size();
    
    // Annualize return
    double annual_return = mean_return * periods_per_year;
    
    // Calculate standard deviation
    double volatility = std_dev(returns);
    
    // Annualize volatility
    double annual_volatility = volatility * std::sqrt(periods_per_year);
    
    // Sharpe ratio = (R_p - R_f) / σ_p
    if (annual_volatility == 0.0) {
        return 0.0;
    }
    
    return (annual_return - risk_free_rate) / annual_volatility;
}

double FactorEngine::sortino_ratio(
    const std::vector<double>& returns,
    double risk_free_rate,
    int periods_per_year) {
    
    if (returns.size() < 2) {
        return 0.0;
    }
    
    // Calculate mean return
    double sum = std::accumulate(returns.begin(), returns.end(), 0.0);
    double mean_return = sum / returns.size();
    
    // Annualize return
    double annual_return = mean_return * periods_per_year;
    
    // Calculate downside deviation (only negative returns)
    double downside_sum = 0.0;
    int negative_count = 0;
    
    for (const auto& r : returns) {
        if (r < 0) {
            downside_sum += r * r;
            negative_count++;
        }
    }
    
    double downside_deviation = 0.0;
    if (negative_count > 0) {
        downside_deviation = std::sqrt(downside_sum / returns.size());
        downside_deviation *= std::sqrt(periods_per_year);
    }
    
    if (downside_deviation == 0.0) {
        return 0.0;
    }
    
    return (annual_return - risk_free_rate) / downside_deviation;
}

double FactorEngine::max_drawdown(const std::vector<double>& equity_curve) {
    if (equity_curve.empty()) {
        return 0.0;
    }
    
    double peak = equity_curve[0];
    double max_dd = 0.0;
    
    for (const auto& value : equity_curve) {
        // Update peak if current value is higher
        if (value > peak) {
            peak = value;
        }
        
        // Calculate drawdown from peak
        double drawdown = (peak - value) / peak;
        
        // Track maximum drawdown
        if (drawdown > max_dd) {
            max_dd = drawdown;
        }
    }
    
    return max_dd;
}

std::vector<double> FactorEngine::calculate_returns(
    const std::vector<double>& prices, bool log_returns) {
    
    if (prices.size() < 2) {
        return {};
    }
    
    std::vector<double> returns;
    returns.reserve(prices.size() - 1);
    
    for (size_t i = 1; i < prices.size(); ++i) {
        double ret;
        if (log_returns) {
            // Log return: ln(P_t / P_{t-1})
            ret = std::log(prices[i] / prices[i-1]);
        } else {
            // Simple return: (P_t - P_{t-1}) / P_{t-1}
            ret = (prices[i] - prices[i-1]) / prices[i-1];
        }
        returns.push_back(ret);
    }
    
    return returns;
}

} // namespace quantlab
