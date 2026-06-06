/**
 * @file types.h
 * @brief Core type definitions for QuantLabCPP
 * 
 * TUJUAN FILE:
 * - Mendefinisikan tipe data dasar yang digunakan di seluruh sistem
 * - Menyediakan type aliases untuk clarity dan maintainability
 * - Menetapkan enum untuk state management
 * 
 * KONSEP QUANT:
 * - Price, volume, timestamp adalah primitive data dalam trading
 * - Order side (BUY/SELL) dan order type menentukan execution logic
 * - Timeframe menentukan granularity analisis
 * 
 * KONSEP C++:
 * - Strong typing untuk type safety
 * - Enum class untuk scoped enumerations
 * - Using declarations untuk type aliases
 * - Structs untuk plain old data (POD)
 * 
 * HUBUNGAN DENGAN HEDGE FUND WORKFLOW:
 * - Standardisasi tipe data memastikan konsistensi across systems
 * - Type safety mencegah bugs dari type mismatches
 * - Clear semantics memudahkan kolaborasi tim
 */

#ifndef QUANTLABCPP_TYPES_H
#define QUANTLABCPP_TYPES_H

#include <string>
#include <vector>
#include <chrono>
#include <cstdint>

namespace quantlab {

// ============================================================================
// TIME TYPES
// ============================================================================

/**
 * @brief Timestamp dalam nanoseconds sejak epoch
 * 
 * Menggunakan int64_t untuk presisi tinggi required dalam HFT
 */
using Timestamp = int64_t;

/**
 * @brief Durasi dalam nanoseconds
 */
using Duration = int64_t;

// ============================================================================
// PRICE AND VOLUME TYPES
// ============================================================================

/**
 * @brief Harga sekuritas
 * 
 * Menggunakan double untuk floating-point precision
 * Dalam production, pertimbangkan fixed-point arithmetic
 */
using Price = double;

/**
 * @brief Volume perdagangan
 */
using Volume = double;

/**
 * @brief Jumlah saham/quantity
 */
using Quantity = double;

// ============================================================================
// ENUMERATIONS
// ============================================================================

/**
 * @brief Sisi order (buy atau sell)
 */
enum class OrderSide : uint8_t {
    BUY = 0,
    SELL = 1,
    UNKNOWN = 2
};

/**
 * @brief Tipe order
 */
enum class OrderType : uint8_t {
    MARKET = 0,      // Execute immediately at best available price
    LIMIT = 1,       // Execute at specified price or better
    STOP = 2,        // Become market order when stop price hit
    STOP_LIMIT = 3   // Become limit order when stop price hit
};

/**
 * @brief Status order
 */
enum class OrderStatus : uint8_t {
    PENDING = 0,     // Order created but not sent
    SUBMITTED = 1,   // Order sent to market
    PARTIALLY_FILLED = 2,
    FILLED = 3,
    CANCELLED = 4,
    REJECTED = 5,
    EXPIRED = 6
};

/**
 * @brief Timeframe untuk OHLCV data
 */
enum class Timeframe : uint8_t {
    TICK = 0,        // Every trade
    SECOND_1 = 1,    // 1 second
    MINUTE_1 = 2,    // 1 minute
    MINUTE_5 = 3,    // 5 minutes
    MINUTE_15 = 4,   // 15 minutes
    MINUTE_30 = 5,   // 30 minutes
    HOUR_1 = 6,      // 1 hour
    DAY = 7,         // Daily
    WEEK = 8,        // Weekly
    MONTH = 9        // Monthly
};

/**
 * @brief Tipe aksi korporasi
 */
enum class CorporateAction : uint8_t {
    NONE = 0,
    DIVIDEND = 1,
    SPLIT = 2,
    MERGER = 3,
    SPINOFF = 4
};

// ============================================================================
// DATA STRUCTURES
// ============================================================================

/**
 * @brief Representasi tick data (setiap trade)
 * 
 * Tick data adalah level paling granular dari market data
 * Digunakan untuk HFT dan microstructure analysis
 */
struct Tick {
    Timestamp timestamp;   ///< Waktu trade dalam nanoseconds
    Price price;           ///< Harga eksekusi
    Volume volume;         ///< Volume trade
    OrderSide side;        ///< Aggressor side (buy/sell)
    
    Tick() : timestamp(0), price(0.0), volume(0.0), side(OrderSide::UNKNOWN) {}
    
    Tick(Timestamp ts, Price p, Volume v, OrderSide s)
        : timestamp(ts), price(p), volume(v), side(s) {}
};

/**
 * @brief OHLCV Bar (Open-High-Low-Close-Volume)
 * 
 * OHLCV adalah aggregation dari tick data dalam timeframe tertentu
 * Paling umum digunakan untuk technical analysis
 */
struct Bar {
    Timestamp timestamp;   ///< Start time of bar
    Price open;            ///< Opening price
    Price high;            ///< Highest price in period
    Price low;             ///< Lowest price in period
    Price close;           ///< Closing price
    Volume volume;         ///< Total volume
    
    Bar() : timestamp(0), open(0.0), high(0.0), low(0.0), close(0.0), volume(0.0) {}
    
    /**
     * @brief Return true jika bar sudah complete (ada semua data)
     */
    bool is_complete() const {
        return open > 0 && high >= open && low <= open && close > 0 && volume >= 0;
    }
    
    /**
     * @brief Return typical price ((H+L+C)/3)
     */
    Price typical_price() const {
        return (high + low + close) / 3.0;
    }
    
    /**
     * @brief Return range (H-L)
     */
    Price range() const {
        return high - low;
    }
};

/**
 * @brief Identifikasi sekuritas
 */
struct Symbol {
    std::string ticker;        ///< Ticker symbol (e.g., "AAPL")
    std::string exchange;      ///< Exchange code (e.g., "NYSE")
    std::string currency;      ///< Currency (e.g., "USD")
    std::string asset_class;   ///< Asset class (e.g., "EQUITY", "FUTURE")
    
    Symbol() : currency("USD"), asset_class("EQUITY") {}
    
    Symbol(const std::string& t, const std::string& e = "US")
        : ticker(t), exchange(e), currency("USD"), asset_class("EQUITY") {}
    
    /**
     * @brief Return full symbol string
     */
    std::string to_string() const {
        return exchange + ":" + ticker;
    }
    
    bool operator==(const Symbol& other) const {
        return ticker == other.ticker && exchange == other.exchange;
    }
    
    bool operator<(const Symbol& other) const {
        if (ticker != other.ticker) return ticker < other.ticker;
        return exchange < other.exchange;
    }
};

/**
 * @brief Order representation
 */
struct Order {
    int64_t order_id;          ///< Unique order identifier
    Symbol symbol;             ///< Security to trade
    OrderSide side;            ///< Buy or sell
    OrderType type;            ///< Market, limit, etc.
    Quantity quantity;         ///< Amount to trade
    Price price;               ///< Limit price (if applicable)
    Price stop_price;          ///< Stop price (if applicable)
    OrderStatus status;        ///< Current status
    Timestamp timestamp;       ///< Order creation time
    std::string strategy_id;   ///< Which strategy generated this
    
    Order() 
        : order_id(0), side(OrderSide::BUY), type(OrderType::MARKET),
          quantity(0), price(0), stop_price(0), status(OrderStatus::PENDING),
          timestamp(0) {}
};

/**
 * @brief Fill/execution report
 */
struct Fill {
    int64_t fill_id;           ///< Unique fill identifier
    int64_t order_id;          ///< Parent order ID
    Symbol symbol;             ///< Security traded
    OrderSide side;            ///< Buy or sell
    Price fill_price;          ///< Execution price
    Quantity fill_quantity;    ///< Executed quantity
    Timestamp timestamp;       ///< Execution time
    double commission;         ///< Transaction cost
    
    Fill() 
        : fill_id(0), order_id(0), side(OrderSide::BUY),
          fill_price(0), fill_quantity(0), timestamp(0), commission(0) {}
};

/**
 * @brief Position representation
 */
struct Position {
    Symbol symbol;             ///< Security
    Quantity quantity;         ///< Net quantity (positive=long, negative=short)
    Price avg_cost;            ///< Average entry price
    Price current_price;       ///< Current market price
    Timestamp last_update;     ///< Last update time
    
    Position() 
        : quantity(0), avg_cost(0), current_price(0), last_update(0) {}
    
    /**
     * @brief Return unrealized P&L
     */
    double unrealized_pnl() const {
        return (current_price - avg_cost) * quantity;
    }
    
    /**
     * @brief Return market value
     */
    double market_value() const {
        return current_price * quantity;
    }
    
    /**
     * @brief Return cost basis
     */
    double cost_basis() const {
        return avg_cost * quantity;
    }
};

} // namespace quantlab

#endif // QUANTLABCPP_TYPES_H
