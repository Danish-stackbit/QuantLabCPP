/**
 * @file backtester.h
 * @brief Event-driven backtesting engine
 * 
 * TUJUAN FILE:
 * - Simulate trading strategies on historical data
 * - Track portfolio performance over time
 * - Calculate performance metrics
 * 
 * KONSEP QUANT:
 * - Event-driven architecture mirrors real trading
 * - Account for transaction costs (commission, slippage)
 * - Avoid look-ahead bias
 * - Proper handling of corporate actions
 * 
 * KONSEP C++:
 * - Event queue untuk message passing
 * - State machine untuk order lifecycle
 * - Observer pattern for event handling
 * 
 * HUBUNGAN DENGAN HEDGE FUND WORKFLOW:
 * - Strategy testing sebelum live deployment
 * - Performance attribution
 * - Risk analysis
 */

#ifndef QUANTLABCPP_BACKTESTER_H
#define QUANTLABCPP_BACKTESTER_H

#include "types.h"
#include "strategy.h"
#include <vector>
#include <queue>
#include <memory>
#include <map>

namespace quantlab {

// ============================================================================
// EVENTS
// ============================================================================

/**
 * @brief Base event class
 */
struct Event {
    enum class EventType {
        MARKET,
        SIGNAL,
        ORDER,
        FILL
    };
    
    EventType type;
    Timestamp timestamp;
    
    Event(EventType t, Timestamp ts) : type(t), timestamp(ts) {}
    virtual ~Event() = default;
};

/**
 * @brief Market data event
 */
struct MarketEvent : public Event {
    Symbol symbol;
    Bar bar;
    
    MarketEvent(Timestamp ts, const Symbol& sym, const Bar& b)
        : Event(EventType::MARKET, ts), symbol(sym), bar(b) {}
};

/**
 * @brief Signal event dari strategy
 */
struct SignalEvent : public Event {
    Signal signal;
    
    SignalEvent(Timestamp ts, const Signal& sig)
        : Event(EventType::SIGNAL, ts), signal(sig) {}
};

/**
 * @brief Order event ke execution engine
 */
struct OrderEvent : public Event {
    Order order;
    
    OrderEvent(Timestamp ts, const Order& ord)
        : Event(EventType::ORDER, ts), order(ord) {}
};

/**
 * @brief Fill event dari execution
 */
struct FillEvent : public Event {
    Fill fill;
    
    FillEvent(Timestamp ts, const Fill& f)
        : Event(EventType::FILL, ts), fill(f) {}
};

// ============================================================================
// BACKTESTER
// ============================================================================

/**
 * @brief Configuration untuk backtest
 */
struct BacktestConfig {
    double initial_capital = 100000.0;    // Starting capital
    double commission_rate = 0.001;       // Commission per trade (0.1%)
    double slippage_rate = 0.0005;        // Slippage (0.05%)
    bool enable_shorting = false;          // Allow short selling
    double margin_requirement = 1.0;      // Margin requirement (100% = no leverage)
};

/**
 * @brief Performance metrics hasil backtest
 */
struct PerformanceMetrics {
    double total_return;           // Total return (%)
    double cagr;                   // Compound annual growth rate
    double sharpe_ratio;           // Risk-adjusted return
    double sortino_ratio;          // Downside risk-adjusted return
    double max_drawdown;           // Maximum peak-to-trough decline
    double win_rate;               // Percentage of winning trades
    double profit_factor;          // Gross profit / Gross loss
    int total_trades;              // Number of trades
    double avg_trade;              // Average P&L per trade
    double best_trade;             // Best single trade
    double worst_trade;            // Worst single trade
    double avg_win;                // Average winning trade
    double avg_loss;               // Average losing trade
    int consecutive_wins;          // Max consecutive wins
    int consecutive_losses;        // Max consecutive losses
};

/**
 * @brief Event-Driven Backtester
 * 
 * ARCHITECTURE:
 * 1. DataHandler feeds market data
 * 2. Strategy generates signals
 * 3. PortfolioManager manages positions
 * 4. ExecutionHandler simulates fills
 * 5. Events flow through queue
 */
class EventDrivenBacktester {
public:
    /**
     * @brief Constructor
     * @param config Backtest configuration
     */
    explicit EventDrivenBacktester(const BacktestConfig& config = BacktestConfig());
    
    /**
     * @brief Add strategy to backtester
     */
    void add_strategy(const std::string& name, std::unique_ptr<BaseStrategy> strategy);
    
    /**
     * @brief Run backtest on historical data
     * @param bars Historical OHLCV data
     * @return Performance metrics
     */
    PerformanceMetrics run(const std::vector<Bar>& bars);
    
    /**
     * @brief Get equity curve
     */
    const std::vector<double>& equity_curve() const { return equity_curve_; }
    
    /**
     * @brief Get all fills
     */
    const std::vector<Fill>& fills() const { return fills_; }
    
    /**
     * @brief Get final portfolio value
     */
    double final_value() const { return equity_curve_.empty() ? 0 : equity_curve_.back(); }
    
    /**
     * @brief Get initial capital
     */
    double initial_capital() const { return config_.initial_capital; }

private:
    BacktestConfig config_;
    std::map<std::string, std::unique_ptr<BaseStrategy>> strategies_;
    std::vector<double> equity_curve_;
    std::vector<Fill> fills_;
    std::map<Symbol, Position> positions_;
    double cash_;
    double portfolio_value_;
    
    // Event queue
    std::queue<std::shared_ptr<Event>> event_queue_;
    
    /**
     * @brief Process market event
     */
    void handle_market_event(const MarketEvent& event);
    
    /**
     * @brief Process signal event
     */
    void handle_signal_event(const SignalEvent& event);
    
    /**
     * @brief Process order event
     */
    void handle_order_event(const OrderEvent& event);
    
    /**
     * @brief Process fill event
     */
    void handle_fill_event(const FillEvent& event);
    
    /**
     * @brief Execute order (simulate fill)
     */
    Fill execute_order(const Order& order, Price market_price);
    
    /**
     * @brief Update portfolio value
     */
    void update_portfolio_value(const std::map<Symbol, Price>& prices);
    
    /**
     * @brief Calculate performance metrics
     */
    PerformanceMetrics calculate_metrics() const;
};

} // namespace quantlab

#endif // QUANTLABCPP_BACKTESTER_H
