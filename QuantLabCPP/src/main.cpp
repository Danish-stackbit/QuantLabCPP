/**
 * @file main.cpp
 * @brief Main entry point untuk QuantLabCPP
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <cstdlib>

#include "types.h"
#include "csv_loader.h"
#include "factor_engine.h"
#include "strategy.h"
#include "backtester.h"
#include "portfolio.h"

using namespace quantlab;

void print_welcome() {
    std::cout << R"(
╔═══════════════════════════════════════════════════════════╗
║           QuantLabCPP v1.0.0                              ║
║   Institutional-Grade Quantitative Trading System         ║
║                                                           ║
║   Built with C++20 for learning & research                ║
╚═══════════════════════════════════════════════════════════╝
)" << std::endl;
}

void print_usage() {
    std::cout << R"(
Usage: quantlab [OPTIONS]

Options:
  --help, -h              Show this help message
  --demo                  Run demo backtest
  --test-factors          Test factor calculations
  --test-strategy         Test strategy signals
  --test-portfolio        Test portfolio optimization

Examples:
  ./quantlab --demo
  ./quantlab --test-factors
)" << std::endl;
}

void run_demo() {
    std::cout << "\n[1] Generating sample OHLCV data..." << std::endl;
    std::vector<Bar> sample_bars;
    Price price = 100.0;
    Timestamp ts = 1704067200000000000LL;
    
    srand(42);
    for (int i = 0; i < 252; ++i) {
        double change = (rand() % 201 - 100) / 100.0;
        Price close = price * (1 + change / 100);
        Price high = std::max(price, close) * (1 + (rand() % 51) / 1000.0);
        Price low = std::min(price, close) * (1 - (rand() % 51) / 1000.0);
        
        Bar bar;
        bar.timestamp = ts;
        bar.open = price;
        bar.high = high;
        bar.low = low;
        bar.close = close;
        bar.volume = 1000000 + rand() % 500000;
        
        sample_bars.push_back(bar);
        price = close;
        ts += 86400000000000LL;
    }
    std::cout << "    Generated " << sample_bars.size() << " bars" << std::endl;
    
    std::cout << "\n[2] Calculating factors..." << std::endl;
    std::vector<double> closes;
    for (const auto& bar : sample_bars) {
        closes.push_back(bar.close);
    }
    
    auto sma_20 = FactorEngine::calculate_sma(closes, 20);
    auto sma_50 = FactorEngine::calculate_sma(closes, 50);
    auto momentum = FactorEngine::calculate_momentum(closes, 20);
    auto volatility = FactorEngine::calculate_historical_volatility(closes, 20);
    
    std::cout << "    SMA(20): " << sma_20.back() << std::endl;
    std::cout << "    SMA(50): " << sma_50.back() << std::endl;
    std::cout << "    Momentum(20): " << momentum.back() << std::endl;
    std::cout << "    Volatility(20): " << volatility.back() * 100 << "%" << std::endl;
    
    std::cout << "\n[3] Running MA Crossover Strategy backtest..." << std::endl;
    
    BacktestConfig config;
    config.initial_capital = 100000.0;
    config.commission_rate = 0.001;
    config.slippage_rate = 0.0005;
    
    EventDrivenBacktester backtester(config);
    backtester.add_strategy("MA_Cross", std::make_unique<MACrossoverStrategy>(10, 50));
    
    PerformanceMetrics metrics = backtester.run(sample_bars);
    
    std::cout << "\n[4] Performance Metrics:" << std::endl;
    std::cout << "    Total Return: " << metrics.total_return * 100 << "%" << std::endl;
    std::cout << "    Sharpe Ratio: " << metrics.sharpe_ratio << std::endl;
    std::cout << "    Max Drawdown: " << metrics.max_drawdown * 100 << "%" << std::endl;
    std::cout << "    Win Rate: " << metrics.win_rate * 100 << "%" << std::endl;
    std::cout << "    Total Trades: " << metrics.total_trades << std::endl;
    
    std::cout << "\n[5] Portfolio Optimization Demo..." << std::endl;
    
    Eigen::VectorXd expected_returns(3);
    expected_returns << 0.10, 0.12, 0.08;
    
    Eigen::MatrixXd cov_matrix(3, 3);
    cov_matrix << 0.04, 0.01, 0.02,
                  0.01, 0.09, 0.03,
                  0.02, 0.03, 0.0225;
    
    auto result = PortfolioEngine::max_sharpe_portfolio(expected_returns, cov_matrix);
    
    if (result.converged) {
        std::cout << "    Optimal Weights:" << std::endl;
        for (size_t i = 0; i < result.weights.size(); ++i) {
            std::cout << "      Asset " << (i+1) << ": " << result.weights[i] * 100 << "%" << std::endl;
        }
        std::cout << "    Expected Return: " << result.expected_return * 100 << "%" << std::endl;
        std::cout << "    Volatility: " << result.volatility * 100 << "%" << std::endl;
        std::cout << "    Sharpe Ratio: " << result.sharpe_ratio << std::endl;
    }
    
    std::cout << "\n✓ Demo completed successfully!" << std::endl;
}

void test_factors() {
    std::cout << "\n=== Factor Engine Tests ===" << std::endl;
    
    std::vector<double> prices = {100, 102, 101, 103, 105, 104, 106, 108, 107, 109};
    
    std::cout << "\nTest Prices: ";
    for (auto p : prices) std::cout << p << " ";
    std::cout << std::endl;
    
    auto sma = FactorEngine::calculate_sma(prices, 3);
    std::cout << "\nSMA(3): ";
    for (auto v : sma) std::cout << v << " ";
    std::cout << std::endl;
    
    auto ema = FactorEngine::calculate_ema(prices, 3);
    std::cout << "EMA(3): ";
    for (auto v : ema) std::cout << v << " ";
    std::cout << std::endl;
    
    auto mom = FactorEngine::calculate_momentum(prices, 3);
    std::cout << "Momentum(3): ";
    for (auto v : mom) std::cout << v << " ";
    std::cout << std::endl;
    
    auto returns = FactorEngine::calculate_returns(prices);
    std::cout << "Returns: ";
    for (auto r : returns) std::cout << r << " ";
    std::cout << std::endl;
    
    std::cout << "\nRisk Metrics:" << std::endl;
    std::cout << "  Std Dev: " << FactorEngine::std_dev(prices) << std::endl;
    std::cout << "  Variance: " << FactorEngine::variance(prices) << std::endl;
    std::cout << "  Sharpe: " << FactorEngine::sharpe_ratio(returns) << std::endl;
    
    std::cout << "\n✓ Factor tests completed!" << std::endl;
}

void test_strategies() {
    std::cout << "\n=== Strategy Tests ===" << std::endl;
    
    std::vector<Bar> bars;
    Price price = 100.0;
    srand(42);
    for (int i = 0; i < 100; ++i) {
        double change = (rand() % 201 - 100) / 100.0;
        Price close = price * (1 + change / 100);
        
        Bar bar;
        bar.timestamp = i;
        bar.open = price;
        bar.high = std::max(price, close) * 1.01;
        bar.low = std::min(price, close) * 0.99;
        bar.close = close;
        bar.volume = 1000000;
        
        bars.push_back(bar);
        price = close;
    }
    
    std::cout << "\nTesting MA Crossover Strategy..." << std::endl;
    MACrossoverStrategy ma_strategy(10, 50);
    ma_strategy.init();
    
    int signals = 0;
    for (size_t i = 50; i < bars.size(); ++i) {
        std::vector<Bar> history(bars.begin(), bars.begin() + i + 1);
        Signal sig = ma_strategy.generate_signal(history);
        if (sig.type != SignalType::HOLD) {
            signals++;
            std::cout << "  Bar " << i << ": Signal = ";
            switch(sig.type) {
                case SignalType::LONG: std::cout << "LONG"; break;
                case SignalType::SHORT: std::cout << "SHORT"; break;
                case SignalType::FLAT: std::cout << "FLAT"; break;
                default: std::cout << "HOLD"; break;
            }
            std::cout << std::endl;
        }
    }
    std::cout << "  Total signals: " << signals << std::endl;
    
    std::cout << "\n✓ Strategy tests completed!" << std::endl;
}

void test_portfolio() {
    std::cout << "\n=== Portfolio Optimization Tests ===" << std::endl;
    
    Eigen::VectorXd expected_returns(3);
    expected_returns << 0.10, 0.12, 0.08;
    
    Eigen::MatrixXd cov_matrix(3, 3);
    cov_matrix << 0.04, 0.01, 0.02,
                  0.01, 0.09, 0.03,
                  0.02, 0.03, 0.0225;
    
    std::cout << "\nExpected Returns: 10%, 12%, 8%" << std::endl;
    
    std::cout << "\n--- Equal Weight ---" << std::endl;
    auto ew = PortfolioEngine::equal_weight(3);
    std::cout << "Weights: ";
    for (auto w : ew) std::cout << w * 100 << "% ";
    std::cout << std::endl;
    
    std::cout << "\n--- Minimum Variance ---" << std::endl;
    auto mv = PortfolioEngine::min_variance_portfolio(cov_matrix);
    if (mv.converged) {
        std::cout << "Weights: ";
        for (auto w : mv.weights) std::cout << w * 100 << "% ";
        std::cout << std::endl;
        std::cout << "Volatility: " << mv.volatility * 100 << "%" << std::endl;
    }
    
    std::cout << "\n--- Maximum Sharpe ---" << std::endl;
    auto ms = PortfolioEngine::max_sharpe_portfolio(expected_returns, cov_matrix);
    if (ms.converged) {
        std::cout << "Weights: ";
        for (auto w : ms.weights) std::cout << w * 100 << "% ";
        std::cout << std::endl;
        std::cout << "Expected Return: " << ms.expected_return * 100 << "%" << std::endl;
        std::cout << "Volatility: " << ms.volatility * 100 << "%" << std::endl;
        std::cout << "Sharpe Ratio: " << ms.sharpe_ratio << std::endl;
    }
    
    std::cout << "\n✓ Portfolio tests completed!" << std::endl;
}

int main(int argc, char* argv[]) {
    try {
        print_welcome();
        
        bool run_demo_flag = false;
        bool test_factors_flag = false;
        bool test_strategy_flag = false;
        bool test_portfolio_flag = false;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                print_usage();
                return 0;
            } else if (arg == "--demo") {
                run_demo_flag = true;
            } else if (arg == "--test-factors") {
                test_factors_flag = true;
            } else if (arg == "--test-strategy") {
                test_strategy_flag = true;
            } else if (arg == "--test-portfolio") {
                test_portfolio_flag = true;
            }
        }
        
        if (!run_demo_flag && !test_factors_flag && !test_strategy_flag && !test_portfolio_flag) {
            run_demo_flag = true;
        }
        
        if (run_demo_flag) {
            run_demo();
        }
        
        if (test_factors_flag) {
            test_factors();
        }
        
        if (test_strategy_flag) {
            test_strategies();
        }
        
        if (test_portfolio_flag) {
            test_portfolio();
        }
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
