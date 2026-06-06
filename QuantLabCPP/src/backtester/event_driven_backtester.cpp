/**
 * @file event_driven_backtester.cpp
 * @brief Implementation of event-driven backtesting engine
 */

#include "backtester.h"
#include "factor_engine.h"
#include <algorithm>
#include <cmath>

namespace quantlab {

EventDrivenBacktester::EventDrivenBacktester(const BacktestConfig& config)
    : config_(config), cash_(config.initial_capital), 
      portfolio_value_(config.initial_capital) {}

void EventDrivenBacktester::add_strategy(
    const std::string& name, std::unique_ptr<BaseStrategy> strategy) {
    strategies_[name] = std::move(strategy);
}

PerformanceMetrics EventDrivenBacktester::run(const std::vector<Bar>& bars) {
    if (bars.empty()) {
        return PerformanceMetrics{};
    }
    
    // Initialize equity curve
    equity_curve_.clear();
    equity_curve_.push_back(config_.initial_capital);
    fills_.clear();
    
    Symbol symbol("BACKTEST");
    int64_t fill_id = 0;
    
    // Process each bar
    for (size_t i = 0; i < bars.size(); ++i) {
        const Bar& bar = bars[i];
        
        // 1. Generate market event
        MarketEvent mkt_event(bar.timestamp, symbol, bar);
        handle_market_event(mkt_event);
        
        // 2. For each strategy, generate signals
        std::vector<Bar> history(bars.begin(), bars.begin() + i + 1);
        for (auto& [name, strategy] : strategies_) {
            Signal sig = strategy->generate_signal(history);
            
            if (sig.type != SignalType::HOLD) {
                SignalEvent sig_event(bar.timestamp, sig);
                handle_signal_event(sig_event);
                
                // 3. If signal generates order, execute it
                if (sig.type == SignalType::LONG || sig.type == SignalType::SHORT) {
                    Order order;
                    order.order_id = static_cast<int64_t>(i);
                    order.symbol = symbol;
                    order.side = (sig.type == SignalType::LONG) ? 
                        OrderSide::BUY : OrderSide::SELL;
                    order.type = OrderType::MARKET;
                    order.quantity = 100;  // Fixed quantity for simplicity
                    order.price = bar.close;
                    order.timestamp = bar.timestamp;
                    
                    OrderEvent ord_event(bar.timestamp, order);
                    handle_order_event(ord_event);
                    
                    // 4. Execute order and generate fill
                    Fill fill = execute_order(order, bar.close);
                    fill.fill_id = ++fill_id;
                    fills_.push_back(fill);
                    
                    FillEvent fill_event(bar.timestamp, fill);
                    handle_fill_event(fill_event);
                }
            }
        }
        
        // Update portfolio value
        Price current_price = bar.close;
        double position_value = 0;
        for (const auto& [sym, pos] : positions_) {
            position_value += pos.unrealized_pnl();
        }
        portfolio_value_ = cash_ + position_value;
        equity_curve_.push_back(portfolio_value_);
    }
    
    return calculate_metrics();
}

Fill EventDrivenBacktester::execute_order(const Order& order, Price market_price) {
    Fill fill;
    fill.order_id = order.order_id;
    fill.symbol = order.symbol;
    fill.side = order.side;
    fill.fill_quantity = order.quantity;
    fill.timestamp = order.timestamp;
    
    // Apply slippage
    double slippage = market_price * config_.slippage_rate;
    if (order.side == OrderSide::BUY) {
        fill.fill_price = market_price + slippage;
    } else {
        fill.fill_price = market_price - slippage;
    }
    
    // Calculate commission
    fill.commission = fill.fill_price * fill.fill_quantity * config_.commission_rate;
    
    return fill;
}

void EventDrivenBacktester::handle_market_event(const MarketEvent& event) {
    (void)event;
    // Update strategies with new bar data
    for (auto& [name, strategy] : strategies_) {
        strategy->on_bar(event.bar);
    }
}

void EventDrivenBacktester::handle_signal_event(const SignalEvent& event) {
    LOG_DEBUG("Signal: {} - {}", event.signal.reason, 
              event.signal.type == SignalType::LONG ? "BUY" : "SELL");
}

void EventDrivenBacktester::handle_order_event(const OrderEvent& event) {
    (void)event;
    LOG_DEBUG("Order generated");
}

void EventDrivenBacktester::handle_fill_event(const FillEvent& event) {
    const Fill& fill = event.fill;
    
    // Update position
    Position& pos = positions_[fill.symbol];
    
    if (fill.side == OrderSide::BUY) {
        // Buying: add to position
        double total_cost = fill.fill_price * fill.fill_quantity + fill.commission;
        if (pos.quantity >= 0) {
            // Adding to long or opening long
            double total_shares = pos.quantity + fill.fill_quantity;
            if (total_shares > 0) {
                pos.avg_cost = (pos.avg_cost * pos.quantity + total_cost) / total_shares;
            }
            pos.quantity = total_shares;
        } else {
            // Covering short
            pos.quantity += fill.fill_quantity;
            if (pos.quantity > 0) {
                pos.avg_cost = fill.fill_price;
            } else if (pos.quantity == 0) {
                pos.avg_cost = 0;
            }
        }
        cash_ -= total_cost;
    } else {
        // Selling: reduce position
        double proceeds = fill.fill_price * fill.fill_quantity - fill.commission;
        if (pos.quantity <= 0) {
            // Adding to short or opening short
            double total_shares = -pos.quantity + fill.fill_quantity;
            if (total_shares > 0) {
                pos.avg_cost = (pos.avg_cost * (-pos.quantity) + proceeds) / total_shares;
            }
            pos.quantity -= fill.fill_quantity;
        } else {
            // Reducing long
            pos.quantity -= fill.fill_quantity;
            if (pos.quantity < 0) {
                pos.avg_cost = fill.fill_price;
            } else if (pos.quantity == 0) {
                pos.avg_cost = 0;
            }
        }
        cash_ += proceeds;
    }
    
    pos.current_price = fill.fill_price;
    pos.last_update = fill.timestamp;
}

PerformanceMetrics EventDrivenBacktester::calculate_metrics() const {
    PerformanceMetrics metrics;
    
    if (equity_curve_.size() < 2) {
        return metrics;
    }
    
    // Calculate returns
    std::vector<double> returns;
    for (size_t i = 1; i < equity_curve_.size(); ++i) {
        double ret = (equity_curve_[i] - equity_curve_[i-1]) / equity_curve_[i-1];
        returns.push_back(ret);
    }
    
    // Total return
    metrics.total_return = (final_value() - initial_capital()) / initial_capital();
    
    // CAGR (assuming daily data, 252 trading days per year)
    int years = equity_curve_.size() / 252;
    if (years > 0) {
        metrics.cagr = std::pow(final_value() / initial_capital(), 1.0 / years) - 1;
    }
    
    // Risk metrics using FactorEngine
    metrics.sharpe_ratio = FactorEngine::sharpe_ratio(returns);
    metrics.sortino_ratio = FactorEngine::sortino_ratio(returns);
    metrics.max_drawdown = FactorEngine::max_drawdown(equity_curve_);
    
    // Trade statistics
    metrics.total_trades = fills_.size() / 2;  // Each trade has entry and exit
    
    if (!fills_.empty()) {
        // Calculate P&L per round-trip trade
        std::vector<double> trade_pnls;
        double running_pnl = 0;
        Quantity position = 0;
        Price avg_entry = 0;
        
        for (const auto& fill : fills_) {
            if (fill.side == OrderSide::BUY) {
                if (position >= 0) {
                    // Opening or adding to long
                    avg_entry = (avg_entry * position + fill.fill_price * fill.fill_quantity) / 
                               (position + fill.fill_quantity);
                    position += fill.fill_quantity;
                }
            } else {
                if (position > 0) {
                    // Closing long
                    double pnl = (fill.fill_price - avg_entry) * fill.fill_quantity - fill.commission;
                    trade_pnls.push_back(pnl);
                    position -= fill.fill_quantity;
                }
            }
        }
        
        if (!trade_pnls.empty()) {
            double total_pnl = 0;
            double gross_profit = 0;
            double gross_loss = 0;
            int wins = 0;
            int losses = 0;
            int consec_wins = 0;
            int consec_losses = 0;
            int max_consec_wins = 0;
            int max_consec_losses = 0;
            
            metrics.best_trade = trade_pnls[0];
            metrics.worst_trade = trade_pnls[0];
            
            for (double pnl : trade_pnls) {
                total_pnl += pnl;
                
                if (pnl > 0) {
                    gross_profit += pnl;
                    wins++;
                    consec_wins++;
                    consec_losses = 0;
                    max_consec_wins = std::max(max_consec_wins, consec_wins);
                } else {
                    gross_loss += std::abs(pnl);
                    losses++;
                    consec_losses++;
                    consec_wins = 0;
                    max_consec_losses = std::max(max_consec_losses, consec_losses);
                }
                
                metrics.best_trade = std::max(metrics.best_trade, pnl);
                metrics.worst_trade = std::min(metrics.worst_trade, pnl);
            }
            
            metrics.avg_trade = total_pnl / trade_pnls.size();
            metrics.win_rate = static_cast<double>(wins) / trade_pnls.size();
            metrics.profit_factor = (gross_loss > 0) ? gross_profit / gross_loss : gross_profit;
            metrics.avg_win = (wins > 0) ? gross_profit / wins : 0;
            metrics.avg_loss = (losses > 0) ? gross_loss / losses : 0;
            metrics.consecutive_wins = max_consec_wins;
            metrics.consecutive_losses = max_consec_losses;
        }
    }
    
    return metrics;
}

} // namespace quantlab
