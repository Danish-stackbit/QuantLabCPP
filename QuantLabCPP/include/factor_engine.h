/**
 * @file factor_engine.h
 * @brief Factor calculation engine for quantitative analysis
 * 
 * TUJUAN FILE:
 * - Implementasi mathematical factors untuk signal generation
 * - Trend factors (SMA, EMA, Momentum)
 * - Volatility factors (ATR, Historical Volatility)
 * - Volume factors (OBV, Volume Spike)
 * - Risk factors (Sharpe, Sortino, Drawdown)
 * 
 * KONSEP QUANT:
 * - Factors adalah building blocks dari trading signals
 * - Trend following vs mean reversion
 * - Volatility sebagai risk measure
 * - Volume confirmation untuk price moves
 * 
 * KONSEP MATEMATIKA:
 * - Moving averages: SMA = ΣP/n, EMA = αP + (1-α)EMA_prev
 * - Volatility: σ = sqrt(Σ(r-μ)²/(n-1))
 * - Sharpe: (R_p - R_f) / σ_p
 * 
 * KONSEP C++:
 * - Static methods untuk stateless calculations
 * - Template functions untuk generic types
 * - Eigen untuk vectorized operations
 * 
 * HUBUNGAN DENGAN HEDGE FUND WORKFLOW:
 * - Factor research dan testing
 * - Signal generation untuk strategies
 * - Risk monitoring
 */

#ifndef QUANTLABCPP_FACTOR_ENGINE_H
#define QUANTLABCPP_FACTOR_ENGINE_H

#include "types.h"
#include <vector>
#include <cmath>
#include <numeric>
#include <algorithm>

namespace quantlab {

/**
 * @brief Factor Engine untuk menghitung berbagai factors
 */
class FactorEngine {
public:
    // ========================================================================
    // TREND FACTORS
    // ========================================================================
    
    /**
     * @brief Simple Moving Average
     * @param prices Vector of prices
     * @param period Lookback period
     * @return SMA values (size = prices.size() - period + 1)
     * 
     * RUMUS:
     * SMA = (P₁ + P₂ + ... + Pₙ) / n
     * 
     * INTUISI:
     * - Smoothing dari price data
     * - Mengidentifikasi trend direction
     * - Support/resistance dynamic
     */
    static std::vector<double> calculate_sma(
        const std::vector<double>& prices, int period);
    
    /**
     * @brief Exponential Moving Average
     * @param prices Vector of prices
     * @param period Lookback period
     * @return EMA values
     * 
     * RUMUS:
     * EMA_today = α × P_today + (1-α) × EMA_yesterday
     * α = 2 / (period + 1)
     * 
     * INTUISI:
     * - Bobot lebih pada data recent
     * - Lebih responsive daripada SMA
     * - Less lag
     */
    static std::vector<double> calculate_ema(
        const std::vector<double>& prices, int period);
    
    /**
     * @brief Momentum (Rate of Change)
     * @param prices Vector of prices
     * @param period Lookback period
     * @return Momentum values
     * 
     * RUMUS:
     * Momentum = P_today - P_n_days_ago
     * atau
     * ROC = (P_today / P_n_days_ago - 1) × 100
     * 
     * INTUISI:
     * - Measure of velocity of price movement
     * - Positive momentum = uptrend
     * - Negative momentum = downtrend
     */
    static std::vector<double> calculate_momentum(
        const std::vector<double>& prices, int period);
    
    // ========================================================================
    // VOLATILITY FACTORS
    // ========================================================================
    
    /**
     * @brief Average True Range (ATR)
     * @param bars Vector of OHLCV bars
     * @param period Lookback period (default 14)
     * @return ATR values
     * 
     * RUMUS:
     * TR = max(H-L, |H-C_prev|, |L-C_prev|)
     * ATR = SMA(TR, period)
     * 
     * INTUISI:
     * - Measure of volatility
     * - Higher ATR = more volatile
     * - Used for position sizing dan stop loss
     */
    static std::vector<double> calculate_atr(
        const std::vector<Bar>& bars, int period = 14);
    
    /**
     * @brief Historical Volatility
     * @param prices Vector of prices
     * @param period Lookback period (in days)
     * @param annualize Whether to annualize (default true)
     * @return Historical volatility (as decimal)
     * 
     * RUMUS:
     * Returns: r_t = ln(P_t / P_{t-1})
     * Volatility: σ = sqrt(Σ(r-μ)²/(n-1))
     * Annualized: σ_annual = σ × sqrt(252)
     * 
     * INTUISI:
     * - Standard deviation of returns
     * - Measure of risk
     * - Input untuk option pricing
     */
    static std::vector<double> calculate_historical_volatility(
        const std::vector<double>& prices, int period = 20, bool annualize = true);
    
    /**
     * @brief Standard Deviation
     * @param values Vector of values
     * @return Standard deviation
     */
    static double std_dev(const std::vector<double>& values);
    
    /**
     * @brief Variance
     * @param values Vector of values
     * @return Variance
     */
    static double variance(const std::vector<double>& values);
    
    // ========================================================================
    // VOLUME FACTORS
    // ========================================================================
    
    /**
     * @brief On-Balance Volume (OBV)
     * @param bars Vector of OHLCV bars
     * @return OBV cumulative series
     * 
     * RUMUS:
     * If close > prev_close: OBV += volume
     * If close < prev_close: OBV -= volume
     * If close == prev_close: OBV unchanged
     * 
     * INTUISI:
     * - Cumulative volume flow
     * - Volume precedes price
     * - Divergence dapat signal reversal
     */
    static std::vector<double> calculate_obv(const std::vector<Bar>& bars);
    
    /**
     * @brief Volume Spike Detection
     * @param volumes Vector of volumes
     * @param period Lookback period for average
     * @param threshold Multiple of average (default 2.0)
     * @return Boolean flags for spike detection
     * 
     * INTUISI:
     * - Unusual volume indicates institutional activity
     * - Breakout confirmation
     * - Reversal signal
     */
    static std::vector<bool> detect_volume_spikes(
        const std::vector<double>& volumes, int period = 20, double threshold = 2.0);
    
    // ========================================================================
    // RISK FACTORS
    // ========================================================================
    
    /**
     * @brief Sharpe Ratio
     * @param returns Vector of period returns
     * @param risk_free_rate Annual risk-free rate (default 0.02 = 2%)
     * @param periods_per_year Number of periods per year (default 252 for daily)
     * @return Annualized Sharpe ratio
     * 
     * RUMUS:
     * Sharpe = (R_p - R_f) / σ_p
     * Where:
     * - R_p = portfolio return
     * - R_f = risk-free rate
     * - σ_p = portfolio standard deviation
     * 
     * INTUISI:
     * - Risk-adjusted return measure
     * - Higher is better
     * - >1 considered good, >2 excellent
     */
    static double sharpe_ratio(
        const std::vector<double>& returns,
        double risk_free_rate = 0.02,
        int periods_per_year = 252);
    
    /**
     * @brief Sortino Ratio
     * @param returns Vector of period returns
     * @param risk_free_rate Annual risk-free rate
     * @param periods_per_year Number of periods per year
     * @return Annualized Sortino ratio
     * 
     * RUMUS:
     * Sortino = (R_p - R_f) / σ_downside
     * Where downside deviation only considers negative returns
     * 
     * INTUISI:
     * - Like Sharpe but only penalizes downside volatility
     * - More appropriate for asymmetric return distributions
     * - Higher is better
     */
    static double sortino_ratio(
        const std::vector<double>& returns,
        double risk_free_rate = 0.02,
        int periods_per_year = 252);
    
    /**
     * @brief Maximum Drawdown
     * @param equity_curve Vector of portfolio values
     * @return Maximum drawdown as positive decimal (e.g., 0.20 = 20% drawdown)
     * 
     * RUMUS:
     * Peak_t = max(Equity_0, ..., Equity_t)
     * Drawdown_t = (Peak_t - Equity_t) / Peak_t
     * MaxDD = max(Drawdown_t)
     * 
     * INTUISI:
     * - Largest peak-to-trough decline
     * - Measure of downside risk
     * - Important for investor psychology
     */
    static double max_drawdown(const std::vector<double>& equity_curve);
    
    /**
     * @brief Calculate returns from prices
     * @param prices Vector of prices
     * @param log_returns Use logarithmic returns (default true)
     * @return Vector of returns
     */
    static std::vector<double> calculate_returns(
        const std::vector<double>& prices, bool log_returns = true);
};

} // namespace quantlab

#endif // QUANTLABCPP_FACTOR_ENGINE_H
