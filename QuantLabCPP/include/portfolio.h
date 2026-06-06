/**
 * @file portfolio.h
 * @brief Portfolio construction and optimization
 * 
 * TUJUAN FILE:
 * - Implement portfolio allocation methods
 * - Mean-Variance Optimization (Markowitz)
 * - Risk Parity allocation
 * - Equal Weight allocation
 * 
 * KONSEP QUANT:
 * - Modern Portfolio Theory (MPT)
 * - Efficient frontier
 * - Diversification benefits
 * - Risk contribution
 * 
 * KONSEP MATEMATIKA:
 * - Portfolio return: R_p = w'R
 * - Portfolio variance: σ²_p = w'Σw
 * - Sharpe ratio maximization
 * - Convex optimization
 * 
 * KONSEP C++:
 * - Eigen untuk linear algebra
 * - Matrix operations
 * - Numerical optimization
 * 
 * HUBUNGAN DENGAN HEDGE FUND WORKFLOW:
 * - Capital allocation across strategies
 * - Multi-asset portfolio management
 * - Risk budgeting
 */

#ifndef QUANTLABCPP_PORTFOLIO_H
#define QUANTLABCPP_PORTFOLIO_H

#include <vector>
#include <Eigen/Dense>

namespace quantlab {

/**
 * @brief Portfolio optimization result
 */
struct PortfolioResult {
    std::vector<double> weights;     // Optimal weights
    double expected_return;          // Expected portfolio return
    double volatility;               // Portfolio volatility
    double sharpe_ratio;             // Portfolio Sharpe ratio
    bool converged;                  // Whether optimization converged
    std::string message;             // Status message
};

/**
 * @brief Portfolio Engine untuk allocation dan optimization
 */
class PortfolioEngine {
public:
    /**
     * @brief Equal Weight Allocation (1/N)
     * @param n_assets Number of assets
     * @return Vector of equal weights
     * 
     * INTUISI:
     * - Simplest diversification
     * - No estimation error
     * - Often outperforms complex methods
     */
    static std::vector<double> equal_weight(int n_assets);
    
    /**
     * @brief Risk Parity Allocation
     * @param cov_matrix Covariance matrix of returns
     * @return Vector of risk-parity weights
     * 
     * RUMUS:
     * Each asset contributes equally to portfolio risk
     * w_i × (Σw)_i / (w'Σw) = 1/n for all i
     * 
     * INTUISI:
     * - Balance risk contributions
     * - Less concentrated than mean-variance
     * - More robust to estimation error
     */
    static PortfolioResult risk_parity(const Eigen::MatrixXd& cov_matrix);
    
    /**
     * @brief Mean-Variance Optimization (Markowitz)
     * @param expected_returns Vector of expected returns
     * @param cov_matrix Covariance matrix
     * @param target_return Target portfolio return (optional)
     * @param risk_free_rate Risk-free rate for Sharpe calculation
     * @return Optimal portfolio weights
     * 
     * OPTIMIZATION PROBLEM:
     * Minimize: w'Σw (variance)
     * Subject to: w'μ = target_return (if specified)
     *             Σw = 1 (fully invested)
     *             w ≥ 0 (no short selling, optional)
     * 
     * INTUISI:
     * - Find efficient frontier
     * - Maximize return for given risk
     * - Or minimize risk for given return
     */
    static PortfolioResult mean_variance_optimization(
        const Eigen::VectorXd& expected_returns,
        const Eigen::MatrixXd& cov_matrix,
        double target_return = -1.0,  // -1 means maximize Sharpe
        double risk_free_rate = 0.02);
    
    /**
     * @brief Maximum Sharpe Ratio Portfolio
     * @param expected_returns Vector of expected returns
     * @param cov_matrix Covariance matrix
     * @param risk_free_rate Risk-free rate
     * @return Optimal portfolio weights
     * 
     * OPTIMIZATION PROBLEM:
     * Maximize: (w'μ - R_f) / sqrt(w'Σw)
     * Subject to: Σw = 1
     */
    static PortfolioResult max_sharpe_portfolio(
        const Eigen::VectorXd& expected_returns,
        const Eigen::MatrixXd& cov_matrix,
        double risk_free_rate = 0.02);
    
    /**
     * @brief Minimum Variance Portfolio
     * @param cov_matrix Covariance matrix
     * @return Minimum variance weights
     * 
     * OPTIMIZATION PROBLEM:
     * Minimize: w'Σw
     * Subject to: Σw = 1
     */
    static PortfolioResult min_variance_portfolio(
        const Eigen::MatrixXd& cov_matrix);
    
    /**
     * @brief Calculate portfolio expected return
     * @param weights Portfolio weights
     * @param expected_returns Asset expected returns
     */
    static double portfolio_return(
        const std::vector<double>& weights,
        const std::vector<double>& expected_returns);
    
    /**
     * @brief Calculate portfolio volatility
     * @param weights Portfolio weights
     * @param cov_matrix Covariance matrix
     */
    static double portfolio_volatility(
        const std::vector<double>& weights,
        const Eigen::MatrixXd& cov_matrix);
    
    /**
     * @brief Calculate portfolio Sharpe ratio
     */
    static double portfolio_sharpe(
        const std::vector<double>& weights,
        const std::vector<double>& expected_returns,
        const Eigen::MatrixXd& cov_matrix,
        double risk_free_rate = 0.02);
    
    /**
     * @brief Calculate risk contributions
     * @param weights Portfolio weights
     * @param cov_matrix Covariance matrix
     * @return Vector of risk contributions (sum to 1)
     */
    static std::vector<double> risk_contributions(
        const std::vector<double>& weights,
        const Eigen::MatrixXd& cov_matrix);
};

} // namespace quantlab

#endif // QUANTLABCPP_PORTFOLIO_H
