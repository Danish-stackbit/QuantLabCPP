/**
 * @file test_all.cpp
 * @brief Unit tests for QuantLabCPP using Google Test
 */

#include <gtest/gtest.h>
#include "../include/types.h"
#include "../include/factor_engine.h"
#include "../include/portfolio.h"

using namespace quantlab;

// ============================================================================
// FACTOR ENGINE TESTS
// ============================================================================

TEST(FactorEngineTest, SMACalculation) {
    std::vector<double> prices = {1, 2, 3, 4, 5};
    
    // SMA with period 3
    auto sma = FactorEngine::calculate_sma(prices, 3);
    
    ASSERT_EQ(sma.size(), 3);
    EXPECT_DOUBLE_EQ(sma[0], 2.0);   // (1+2+3)/3
    EXPECT_DOUBLE_EQ(sma[1], 3.0);   // (2+3+4)/3
    EXPECT_DOUBLE_EQ(sma[2], 4.0);   // (3+4+5)/3
}

TEST(FactorEngineTest, EMACalculation) {
    std::vector<double> prices = {1, 2, 3, 4, 5};
    
    auto ema = FactorEngine::calculate_ema(prices, 3);
    
    ASSERT_FALSE(ema.empty());
    EXPECT_GT(ema.size(), 0);
}

TEST(FactorEngineTest, MomentumCalculation) {
    std::vector<double> prices = {100, 102, 105, 103, 107};
    
    auto momentum = FactorEngine::calculate_momentum(prices, 2);
    
    ASSERT_EQ(momentum.size(), 3);
    EXPECT_DOUBLE_EQ(momentum[0], 5);   // 105 - 100
    EXPECT_DOUBLE_EQ(momentum[1], 1);   // 103 - 102
    EXPECT_DOUBLE_EQ(momentum[2], 2);   // 107 - 105
}

TEST(FactorEngineTest, ReturnsCalculation) {
    std::vector<double> prices = {100, 101, 102};
    
    auto returns = FactorEngine::calculate_returns(prices, true);
    
    ASSERT_EQ(returns.size(), 2);
    EXPECT_NEAR(returns[0], std::log(101.0/100.0), 1e-6);
    EXPECT_NEAR(returns[1], std::log(102.0/101.0), 1e-6);
}

TEST(FactorEngineTest, StdDevCalculation) {
    std::vector<double> values = {2, 4, 4, 4, 5, 5, 7, 9};
    
    double sd = FactorEngine::std_dev(values);
    
    // Expected std dev ≈ 2.138
    EXPECT_NEAR(sd, 2.138, 0.01);
}

TEST(FactorEngineTest, MaxDrawdownCalculation) {
    std::vector<double> equity = {100, 110, 120, 110, 100, 90, 100, 110};
    
    double max_dd = FactorEngine::max_drawdown(equity);
    
    // Max drawdown from 120 to 90 = 30/120 = 0.25
    EXPECT_NEAR(max_dd, 0.25, 1e-6);
}

// ============================================================================
// PORTFOLIO ENGINE TESTS
// ============================================================================

TEST(PortfolioEngineTest, EqualWeight) {
    auto weights = PortfolioEngine::equal_weight(4);
    
    ASSERT_EQ(weights.size(), 4);
    for (const auto& w : weights) {
        EXPECT_DOUBLE_EQ(w, 0.25);
    }
}

TEST(PortfolioEngineTest, MinVariancePortfolio) {
    Eigen::MatrixXd cov(2, 2);
    cov << 0.04, 0.01,
           0.01, 0.09;
    
    auto result = PortfolioEngine::min_variance_portfolio(cov);
    
    ASSERT_TRUE(result.converged);
    EXPECT_EQ(result.weights.size(), 2);
    
    // Weights should sum to 1
    double sum = 0;
    for (auto w : result.weights) sum += w;
    EXPECT_NEAR(sum, 1.0, 1e-6);
}

TEST(PortfolioEngineTest, RiskContributions) {
    std::vector<double> weights = {0.5, 0.5};
    Eigen::MatrixXd cov(2, 2);
    cov << 0.04, 0.01,
           0.01, 0.09;
    
    auto rc = PortfolioEngine::risk_contributions(weights, cov);
    
    ASSERT_EQ(rc.size(), 2);
    
    // Risk contributions should sum to total risk
    double sum = 0;
    for (auto r : rc) sum += r;
    EXPECT_GT(sum, 0);
}

// ============================================================================
// TYPES TESTS
// ============================================================================

TEST(TypesTest, BarCompleteness) {
    Bar bar;
    bar.open = 100;
    bar.high = 105;
    bar.low = 95;
    bar.close = 102;
    bar.volume = 1000;
    
    EXPECT_TRUE(bar.is_complete());
    
    Bar incomplete;
    incomplete.open = 100;
    EXPECT_FALSE(incomplete.is_complete());
}

TEST(TypesTest, SymbolEquality) {
    Symbol s1("AAPL", "US");
    Symbol s2("AAPL", "US");
    Symbol s3("GOOGL", "US");
    
    EXPECT_EQ(s1, s2);
    EXPECT_NE(s1, s3);
    EXPECT_EQ(s1.to_string(), "US:AAPL");
}

// ============================================================================
// MAIN
// ============================================================================

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
