/**
 * @file csv_loader.h
 * @brief CSV file loading utility for market data
 * 
 * TUJUAN FILE:
 * - Load market data dari CSV files
 * - Parse OHLCV dan tick data formats
 * - Handle berbagai CSV format dan delimiters
 * 
 * KONSEP QUANT:
 * - Market data biasanya disimpan dalam CSV format
 * - OHLCV format: timestamp,open,high,low,close,volume
 * - Tick format: timestamp,price,volume,side
 * 
 * KONSEP C++:
 * - File I/O dengan std::ifstream
 * - String parsing dan tokenization
 * - Error handling dengan exceptions
 * 
 * HUBUNGAN DENGAN HEDGE FUND WORKFLOW:
 * - Data ingestion dari various sources
 * - Historical data loading untuk backtesting
 * - Data validation dan cleaning
 */

#ifndef QUANTLABCPP_CSV_LOADER_H
#define QUANTLABCPP_CSV_LOADER_H

#include "types.h"
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

namespace quantlab {

/**
 * @brief CSV Loader class untuk market data
 */
class CSVLoader {
public:
    /**
     * @brief Constructor
     * @param delimiter Character delimiter (default: comma)
     */
    explicit CSVLoader(char delimiter = ',');
    
    /**
     * @brief Load OHLCV data dari CSV file
     * @param filename Path to CSV file
     * @return Vector of Bar objects
     * 
     * Expected CSV format:
     * timestamp,open,high,low,close,volume
     * 2024-01-01 09:30:00,100.5,101.2,99.8,100.1,1000000
     */
    std::vector<Bar> load_ohlcv(const std::string& filename);
    
    /**
     * @brief Load tick data dari CSV file
     * @param filename Path to CSV file
     * @return Vector of Tick objects
     * 
     * Expected CSV format:
     * timestamp,price,volume,side
     * 1704110400000000000,100.5,100,BUY
     */
    std::vector<Tick> load_ticks(const std::string& filename);
    
    /**
     * @brief Load data dengan custom parser
     * @param filename Path to CSV file
     * @param has_header Whether file has header row
     * @return Vector of strings (raw rows)
     */
    std::vector<std::vector<std::string>> load_raw(
        const std::string& filename, bool has_header = true);
    
    /**
     * @brief Get number of rows loaded
     */
    size_t row_count() const { return row_count_; }
    
    /**
     * @brief Get last error message
     */
    const std::string& last_error() const { return last_error_; }

private:
    char delimiter_;
    size_t row_count_;
    std::string last_error_;
    
    /**
     * @brief Parse timestamp string to Timestamp
     */
    Timestamp parse_timestamp(const std::string& ts_str);
    
    /**
     * @brief Parse double value
     */
    double parse_double(const std::string& str);
    
    /**
     * @brief Split string by delimiter
     */
    std::vector<std::string> split(const std::string& line);
};

} // namespace quantlab

#endif // QUANTLABCPP_CSV_LOADER_H
