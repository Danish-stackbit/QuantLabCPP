/**
 * @file csv_loader.cpp
 * @brief Implementation of CSV loading functionality
 */

#include "csv_loader.h"
#include <stdexcept>
#include <algorithm>
#include <ctime>
#include <iomanip>

namespace quantlab {

CSVLoader::CSVLoader(char delimiter) 
    : delimiter_(delimiter), row_count_(0) {}

std::vector<std::string> CSVLoader::split(const std::string& line) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(line);
    
    while (std::getline(tokenStream, token, delimiter_)) {
        // Trim whitespace
        token.erase(0, token.find_first_not_of(" \t\r\n"));
        token.erase(token.find_last_not_of(" \t\r\n") + 1);
        tokens.push_back(token);
    }
    
    return tokens;
}

Timestamp CSVLoader::parse_timestamp(const std::string& ts_str) {
    // Try parsing as integer first (nanoseconds since epoch)
    try {
        return std::stoll(ts_str);
    } catch (...) {
        // Try datetime format: YYYY-MM-DD HH:MM:SS
        std::tm tm = {};
        std::istringstream ss(ts_str);
        ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
        
        if (ss.fail()) {
            // Try date only: YYYY-MM-DD
            ss.clear();
            ss.str(ts_str);
            ss >> std::get_time(&tm, "%Y-%m-%d");
        }
        
        if (ss.fail()) {
            throw std::runtime_error("Invalid timestamp format: " + ts_str);
        }
        
        // Convert to nanoseconds since epoch
        std::time_t time = std::mktime(&tm);
        return static_cast<Timestamp>(time) * 1000000000LL;
    }
}

double CSVLoader::parse_double(const std::string& str) {
    try {
        return std::stod(str);
    } catch (...) {
        return 0.0;
    }
}

std::vector<std::vector<std::string>> CSVLoader::load_raw(
    const std::string& filename, bool has_header) {
    
    std::vector<std::vector<std::string>> data;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        last_error_ = "Cannot open file: " + filename;
        throw std::runtime_error(last_error_);
    }
    
    std::string line;
    bool first_line = true;
    
    while (std::getline(file, line)) {
        // Skip empty lines
        if (line.empty() || line.find_first_not_of(" \t\r\n") == std::string::npos) {
            continue;
        }
        
        // Skip header if requested
        if (has_header && first_line) {
            first_line = false;
            continue;
        }
        
        first_line = false;
        data.push_back(split(line));
        row_count_++;
    }
    
    file.close();
    return data;
}

std::vector<Bar> CSVLoader::load_ohlcv(const std::string& filename) {
    auto raw_data = load_raw(filename, true);
    std::vector<Bar> bars;
    bars.reserve(raw_data.size());
    
    for (const auto& row : raw_data) {
        if (row.size() < 6) {
            continue; // Skip incomplete rows
        }
        
        Bar bar;
        bar.timestamp = parse_timestamp(row[0]);
        bar.open = parse_double(row[1]);
        bar.high = parse_double(row[2]);
        bar.low = parse_double(row[3]);
        bar.close = parse_double(row[4]);
        bar.volume = parse_double(row[5]);
        
        bars.push_back(bar);
    }
    
    return bars;
}

std::vector<Tick> CSVLoader::load_ticks(const std::string& filename) {
    auto raw_data = load_raw(filename, true);
    std::vector<Tick> ticks;
    ticks.reserve(raw_data.size());
    
    for (const auto& row : raw_data) {
        if (row.size() < 4) {
            continue;
        }
        
        Tick tick;
        tick.timestamp = parse_timestamp(row[0]);
        tick.price = parse_double(row[1]);
        tick.volume = parse_double(row[2]);
        
        // Parse side
        std::string side_str = row[3];
        std::transform(side_str.begin(), side_str.end(), side_str.begin(), ::toupper);
        if (side_str == "BUY" || side_str == "B") {
            tick.side = OrderSide::BUY;
        } else if (side_str == "SELL" || side_str == "S") {
            tick.side = OrderSide::SELL;
        } else {
            tick.side = OrderSide::UNKNOWN;
        }
        
        ticks.push_back(tick);
    }
    
    return ticks;
}

} // namespace quantlab
