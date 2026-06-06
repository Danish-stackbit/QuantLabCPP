/**
 * @file optimization.cpp
 * @brief Portfolio optimization implementations (Risk Parity, Mean-Variance)
 */

#include "portfolio.h"
#include <cmath>
#include <algorithm>

namespace quantlab {

PortfolioResult PortfolioEngine::risk_parity(const Eigen::MatrixXd& cov_matrix) {
    PortfolioResult result;
    int n = cov_matrix.rows();
    
    if (n == 0 || cov_matrix.cols() != n) {
        result.converged = false;
        result.message = "Invalid covariance matrix";
        return result;
    }
    
    // Initialize with equal weights
    Eigen::VectorXd w = Eigen::VectorXd::Constant(n, 1.0 / n);
    
    // Iterative algorithm for risk parity
    const int max_iter = 1000;
    const double tol = 1e-8;
    
    for (int iter = 0; iter < max_iter; ++iter) {
        // Calculate marginal risk contributions
        Eigen::VectorXd sigma = cov_matrix * w;
        double total_risk = std::sqrt(w.dot(sigma));
        
        // Risk contribution of each asset
        Eigen::VectorXd rc = w.cwiseProduct(sigma) / total_risk;
        
        // Target: equal risk contribution = total_risk / n
        double target_rc = total_risk / n;
        
        // Check convergence
        double max_diff = (rc.array() - target_rc).abs().maxCoeff();
        if (max_diff < tol) {
            break;
        }
        
        // Update weights using gradient descent
        for (int i = 0; i < n; ++i) {
            if (rc[i] > target_rc) {
                w[i] *= 0.99;  // Reduce weight if contributing too much risk
            } else {
                w[i] *= 1.01;  // Increase weight if contributing too little risk
            }
        }
        
        // Normalize weights
        w = w.cwiseMax(0.001);  // Ensure positive weights
        w = w / w.sum();
    }
    
    // Store results
    result.weights.resize(n);
    for (int i = 0; i < n; ++i) {
        result.weights[i] = w[i];
    }
    
    // Calculate portfolio statistics
    Eigen::VectorXd sigma = cov_matrix * w;
    result.volatility = std::sqrt(w.dot(sigma));
    result.expected_return = 0;  // Not calculated in risk parity
    result.sharpe_ratio = 0;
    result.converged = true;
    result.message = "Converged";
    
    return result;
}

PortfolioResult PortfolioEngine::mean_variance_optimization(
    const Eigen::VectorXd& expected_returns,
    const Eigen::MatrixXd& cov_matrix,
    double target_return,
    double risk_free_rate) {
    
    PortfolioResult result;
    
    if (target_return < 0) {
        // Maximize Sharpe ratio instead
        return max_sharpe_portfolio(expected_returns, cov_matrix, risk_free_rate);
    }
    
    // Simple analytical solution for minimum variance with target return constraint
    int n = expected_returns.size();
    
    if (n == 0 || cov_matrix.rows() != n || cov_matrix.cols() != n) {
        result.converged = false;
        result.message = "Invalid input dimensions";
        return result;
    }
    
    // For simplicity, use numerical optimization approach
    // In production, would use quadratic programming solver
    
    // Start with equal weights
    Eigen::VectorXd w = Eigen::VectorXd::Constant(n, 1.0 / n);
    
    // Gradient descent to minimize variance subject to constraints
    const double learning_rate = 0.01;
    const int max_iter = 1000;
    const double tol = 1e-6;
    
    for (int iter = 0; iter < max_iter; ++iter) {
        // Gradient of variance: 2 * Σ * w
        Eigen::VectorXd grad = 2 * cov_matrix * w;
        
        // Update weights
        w = w - learning_rate * grad;
        
        // Project onto constraints: sum(w) = 1
        w = w / w.sum();
        
        // Check convergence
        if (grad.norm() < tol) {
            break;
        }
    }
    
    // Store results
    result.weights.resize(n);
    for (int i = 0; i < n; ++i) {
        result.weights[i] = w[i];
    }
    
    result.expected_return = w.dot(expected_returns);
    result.volatility = std::sqrt(w.dot(cov_matrix * w));
    result.sharpe_ratio = (result.expected_return - risk_free_rate) / result.volatility;
    result.converged = true;
    result.message = "Optimization complete";
    
    return result;
}

PortfolioResult PortfolioEngine::max_sharpe_portfolio(
    const Eigen::VectorXd& expected_returns,
    const Eigen::MatrixXd& cov_matrix,
    double risk_free_rate) {
    
    PortfolioResult result;
    int n = expected_returns.size();
    
    if (n == 0 || cov_matrix.rows() != n) {
        result.converged = false;
        result.message = "Invalid input";
        return result;
    }
    
    // Use iterative approach to find maximum Sharpe ratio
    Eigen::VectorXd w = Eigen::VectorXd::Constant(n, 1.0 / n);
    
    const int max_iter = 1000;
    const double tol = 1e-8;
    double prev_sharpe = -1e10;
    
    for (int iter = 0; iter < max_iter; ++iter) {
        double ret = w.dot(expected_returns);
        double vol = std::sqrt(w.dot(cov_matrix * w));
        double sharpe = (ret - risk_free_rate) / vol;
        
        if (std::abs(sharpe - prev_sharpe) < tol) {
            break;
        }
        prev_sharpe = sharpe;
        
        // Gradient of Sharpe ratio (simplified)
        Eigen::VectorXd excess_returns = expected_returns.array() - risk_free_rate;
        Eigen::VectorXd grad = (excess_returns - sharpe * cov_matrix * w / vol) / vol;
        
        // Update weights
        w = w + 0.01 * grad;
        
        // Ensure positive weights and normalize
        w = w.cwiseMax(0.001);
        w = w / w.sum();
    }
    
    // Store results
    result.weights.resize(n);
    for (int i = 0; i < n; ++i) {
        result.weights[i] = w[i];
    }
    
    result.expected_return = w.dot(expected_returns);
    result.volatility = std::sqrt(w.dot(cov_matrix * w));
    result.sharpe_ratio = (result.expected_return - risk_free_rate) / result.volatility;
    result.converged = true;
    result.message = "Maximum Sharpe found";
    
    return result;
}

PortfolioResult PortfolioEngine::min_variance_portfolio(
    const Eigen::MatrixXd& cov_matrix) {
    
    PortfolioResult result;
    int n = cov_matrix.rows();
    
    if (n == 0) {
        result.converged = false;
        result.message = "Empty covariance matrix";
        return result;
    }
    
    // Minimum variance portfolio with sum(w) = 1 constraint
    // Analytical solution: w = Σ^(-1) * 1 / (1' * Σ^(-1) * 1)
    
    Eigen::VectorXd ones = Eigen::VectorXd::Ones(n);
    Eigen::VectorXd w;
    
    try {
        Eigen::VectorXd inv_sigma_ones = cov_matrix.ldlt().solve(ones);
        double denominator = ones.dot(inv_sigma_ones);
        w = inv_sigma_ones / denominator;
    } catch (...) {
        // If inversion fails, fall back to equal weight
        w = Eigen::VectorXd::Constant(n, 1.0 / n);
    }
    
    // Store results
    result.weights.resize(n);
    for (int i = 0; i < n; ++i) {
        result.weights[i] = w[i];
    }
    
    result.expected_return = 0;  // Not calculated
    result.volatility = std::sqrt(w.dot(cov_matrix * w));
    result.sharpe_ratio = 0;
    result.converged = true;
    result.message = "Minimum variance found";
    
    return result;
}

double PortfolioEngine::portfolio_return(
    const std::vector<double>& weights,
    const std::vector<double>& expected_returns) {
    
    if (weights.size() != expected_returns.size()) {
        return 0.0;
    }
    
    double ret = 0.0;
    for (size_t i = 0; i < weights.size(); ++i) {
        ret += weights[i] * expected_returns[i];
    }
    
    return ret;
}

double PortfolioEngine::portfolio_volatility(
    const std::vector<double>& weights,
    const Eigen::MatrixXd& cov_matrix) {
    
    int n = weights.size();
    if (n != cov_matrix.rows()) {
        return 0.0;
    }
    
    Eigen::VectorXd w(n);
    for (int i = 0; i < n; ++i) {
        w[i] = weights[i];
    }
    
    return std::sqrt(w.dot(cov_matrix * w));
}

double PortfolioEngine::portfolio_sharpe(
    const std::vector<double>& weights,
    const std::vector<double>& expected_returns,
    const Eigen::MatrixXd& cov_matrix,
    double risk_free_rate) {
    
    double ret = portfolio_return(weights, expected_returns);
    double vol = portfolio_volatility(weights, cov_matrix);
    
    if (vol == 0) {
        return 0.0;
    }
    
    return (ret - risk_free_rate) / vol;
}

std::vector<double> PortfolioEngine::risk_contributions(
    const std::vector<double>& weights,
    const Eigen::MatrixXd& cov_matrix) {
    
    int n = weights.size();
    Eigen::VectorXd w(n);
    for (int i = 0; i < n; ++i) {
        w[i] = weights[i];
    }
    
    Eigen::VectorXd sigma = cov_matrix * w;
    double total_risk = std::sqrt(w.dot(sigma));
    
    // Risk contribution = w_i * (Σw)_i / total_risk
    Eigen::VectorXd rc = w.cwiseProduct(sigma) / total_risk;
    
    std::vector<double> contributions(n);
    for (int i = 0; i < n; ++i) {
        contributions[i] = rc[i];
    }
    
    return contributions;
}

} // namespace quantlab
