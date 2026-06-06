# QuantLabCPP - Institutional-Grade Quantitative Research & Trading System

## 📋 Table of Contents

1. [Introduction](#introduction)
2. [Project Overview](#project-overview)
3. [Installation Guide](#installation-guide)
4. [Building the Project](#building-the-project)
5. [Running the System](#running-the-system)
6. [Module Documentation](#module-documentation)
7. [Troubleshooting](#troubleshooting)
8. [Learning Roadmap](#learning-roadmap)
9. [Contributing](#contributing)
10. [License](#license)

---

## 🎯 Introduction

**QuantLabCPP** adalah platform quantitative research dan trading system institutional-grade yang dibangun sepenuhnya menggunakan C++ modern. Project ini dirancang untuk:

- **Belajar Quant Research**: Memahami teori dan implementasi faktor-faktor quantitative
- **Belajar Quant Development**: Mempelajari best practices dalam membangun sistem trading
- **Portfolio Institutional-Level**: Menunjukkan kemampuan teknis setara hedge fund profesional
- **Simulasi Hedge Fund Nyata**: Experience workflow yang digunakan di Citadel, Jane Street, HRT, dll

### Target Pengguna

- ✅ Pemula total (belum pernah coding)
- ✅ Belum pernah belajar quant finance
- ✅ Belum pernah menggunakan Linux
- ✅ Belum pernah belajar systems programming

### Filosofi Design

```
Simplicity → Correctness → Performance → Scalability
```

Kami mengikuti prinsip ini secara berurutan. Kode harus mudah dipahami terlebih dahulu, 
kemudian benar secara logika, baru dioptimasi untuk performa.

---

## 🏗️ Project Overview

### Architecture Diagram

```
┌─────────────────────────────────────────────────────────────┐
│                     QuantLabCPP System                      │
├─────────────────────────────────────────────────────────────┤
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐      │
│  │ Data Engine  │→│Factor Engine │→│Strategy Engine│      │
│  └──────────────┘  └──────────────┘  └──────────────┘      │
│         ↓                  ↓                  ↓              │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐      │
│  │Backtester    │←│Risk Manager  │←│Portfolio Mgr │      │
│  └──────────────┘  └──────────────┘  └──────────────┘      │
│         ↓                  ↓                  ↓              │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐      │
│  │Execution     │←│Market Sim    │←│HFT Module    │      │
│  └──────────────┘  └──────────────┘  └──────────────┘      │
└─────────────────────────────────────────────────────────────┘
```

### Technology Stack

| Component | Technology | Purpose |
|-----------|------------|---------|
| Language | C++20 | Core implementation |
| Build System | CMake | Cross-platform builds |
| Linear Algebra | Eigen | Portfolio optimization |
| Logging | spdlog | High-performance logging |
| Formatting | fmt | Type-safe formatting |
| Parallelism | OpenMP | Multi-threading |
| Database | SQLite/PostgreSQL | Data storage |
| Testing | GoogleTest | Unit & integration tests |

---

## 🚀 Installation Guide

### Prerequisites

Sistem operasi yang didukung:
- Ubuntu 20.04/22.04 (Native)
- Windows 11 dengan WSL2
- macOS (dengan Homebrew)

### A. Ubuntu Native Installation

#### Step 1: Update System Packages

```bash
sudo apt update && sudo apt upgrade -y
```

#### Step 2: Install Essential Build Tools

```bash
sudo apt install -y build-essential \
    gcc \
    g++ \
    clang \
    cmake \
    make \
    ninja-build \
    git \
    pkg-config
```

**Penjelasan:**
- `build-essential`: Paket meta yang includes gcc, g++, make
- `gcc`: GNU Compiler Collection untuk C
- `g++`: GNU Compiler Collection untuk C++
- `clang`: Alternative compiler dari LLVM
- `cmake`: Build system generator
- `git`: Version control system

#### Step 3: Install Development Libraries

```bash
sudo apt install -y \
    libeigen3-dev \
    libfmt-dev \
    libspdlog-dev \
    libboost-all-dev \
    libomp-dev \
    libsqlite3-dev \
    libpq-dev \
    libgtest-dev \
    libbenchmark-dev
```

**Penjelasan Library:**
- `libeigen3-dev`: Template library untuk linear algebra
- `libfmt-dev`: Fast formatting library
- `libspdlog-dev`: C++ logging library
- `libboost-all-dev`: Boost libraries collection
- `libomp-dev`: OpenMP untuk parallel programming
- `libsqlite3-dev`: Embedded database
- `libpq-dev`: PostgreSQL client library
- `libgtest-dev`: Google Test framework

#### Step 4: Install Docker (Optional)

```bash
# Add Docker's official GPG key
curl -fsSL https://download.docker.com/linux/ubuntu/gpg | \
    sudo gpg --dearmor -o /usr/share/keyrings/docker-archive-keyring.gpg

# Set up stable repository
echo "deb [arch=$(dpkg --print-architecture) \
signed-by=/usr/share/keyrings/docker-archive-keyring.gpg] \
https://download.docker.com/linux/ubuntu \
$(lsb_release -cs) stable" | \
sudo tee /etc/apt/sources.list.d/docker.list > /dev/null

# Install Docker
sudo apt update
sudo apt install -y docker-ce docker-ce-cli containerd.io

# Add user to docker group
sudo usermod -aG docker $USER
```

#### Step 5: Verify Installation

```bash
# Check compiler versions
g++ --version
clang++ --version

# Check CMake version
cmake --version

# Check Eigen installation
pkg-config --modversion eigen3

# Check Git version
git --version
```

### B. Windows 11 with WSL2

#### Step 1: Enable WSL Feature

Open PowerShell as Administrator:

```powershell
wsl --install
```

Restart computer jika diminta.

#### Step 2: Install Ubuntu from Microsoft Store

```powershell
# List available distributions
wsl --list --online

# Install Ubuntu 22.04
wsl --install -d Ubuntu-22.04
```

#### Step 3: Initial WSL Setup

Setelah instalasi selesai, buka Ubuntu dari Start Menu dan jalankan:

```bash
# Update packages
sudo apt update && sudo apt upgrade -y

# Install all dependencies (sama seperti Ubuntu native)
sudo apt install -y build-essential gcc g++ clang cmake make \
    ninja-build git pkg-config libeigen3-dev libfmt-dev \
    libspdlog-dev libboost-all-dev libomp-dev libsqlite3-dev \
    libpq-dev libgtest-dev libbenchmark-dev
```

#### Step 4: Configure VS Code for WSL

1. Install VS Code dari https://code.visualstudio.com/
2. Install "WSL" extension
3. Install "C/C++" extension (ms-vscode.cpptools)
4. Install "CMake Tools" extension (ms-vscode.cmake-tools)
5. Buka terminal WSL di VS Code: `Ctrl+Shift+P` → "WSL: New Window"

### C. macOS Installation

```bash
# Install Homebrew
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install tools
brew install cmake gcc llvm git eigen boost googletest

# Link LLVM clang
export PATH="/opt/homebrew/opt/llvm/bin:$PATH"
```

---

## 🔨 Building the Project

### Quick Build

```bash
# Clone repository
git clone https://github.com/yourusername/QuantLabCPP.git
cd QuantLabCPP

# Create build directory
mkdir build
cd build

# Configure with CMake
cmake ..

# Build
cmake --build . -j$(nproc)

# Run
./quantlab
```

### Build Options

```bash
# Debug build
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Release build with optimizations
cmake .. -DCMAKE_BUILD_TYPE=Release

# Specify compiler
cmake .. -DCMAKE_CXX_COMPILER=g++
cmake .. -DCMAKE_CXX_COMPILER=clang++

# Install to custom prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/opt/quantlab
```

### Understanding Build Commands

| Command | Purpose |
|---------|---------|
| `mkdir build` | Membuat direktori terpisah untuk file build (out-of-source build) |
| `cd build` | Masuk ke direktori build |
| `cmake ..` | Generate Makefiles dari CMakeLists.txt di parent directory |
| `cmake --build .` | Build project menggunakan generated Makefiles |
| `-j$(nproc)` | Parallel build menggunakan semua CPU cores |

---

## ▶️ Running the System

### Basic Usage

```bash
# Run main application
./quantlab

# Run with config file
./quantlab --config ../configs/default.yaml

# Run specific module
./quantlab --module backtester --strategy ma_crossover

# Run tests
./quantlab_tests
```

### Example Workflow

```bash
# 1. Prepare data
./scripts/download_data.sh

# 2. Clean and validate data
./quantlab --module data --action clean --input ../data/raw --output ../data/clean

# 3. Run backtest
./quantlab --module backtester --strategy momentum --data ../data/clean --period 2020-2023

# 4. Generate report
./quantlab --module analytics --generate-report --output ../reports/

# 5. View dashboard
python ../dashboard/main.py
```

---

## 📚 Module Documentation

### Module 1: Data Engine

**File**: `src/data/`, `include/data/`

**Tujuan**: Mengelola seluruh siklus data market dari loading hingga storage.

**Komponen**:
- `CSVLoader`: Load data dari file CSV
- `TickDataLoader`: Handle tick-level data
- `OHLCVLoader`: Handle candlestick data
- `DataCleaner`: Remove outliers dan handle missing values
- `DataValidator`: Validasi integritas data
- `DataStorage`: Interface ke SQLite/PostgreSQL

**Konsep Quant**:
- Data quality adalah fondasi dari setiap strategy
- Garbage in = garbage out
- Tick data vs OHLCV trade-offs

**Konsep C++**:
- RAII untuk resource management
- Smart pointers untuk memory safety
- Stream I/O untuk efficient file reading

### Module 2: Factor Engine

**File**: `src/factors/`, `include/factors/`

**Tujuan**: Implementasi mathematical factors untuk signal generation.

**Factors Implemented**:

#### Trend Factors
- **SMA (Simple Moving Average)**: Rata-rata harga dalam periode N
  ```
  SMA = (P₁ + P₂ + ... + Pₙ) / n
  ```
- **EMA (Exponential Moving Average)**: Bobot lebih pada data recent
  ```
  EMA = α × P + (1-α) × EMA_previous
  ```
- **Momentum**: Rate of change harga
  ```
  Momentum = P_today - P_n_days_ago
  ```

#### Volatility Factors
- **ATR (Average True Range)**: Measure of volatility
- **Historical Volatility**: Standard deviation of returns

#### Volume Factors
- **OBV (On-Balance Volume)**: Cumulative volume flow
- **Volume Spike**: Deviasi dari average volume

#### Risk Factors
- **Sharpe Ratio**: Risk-adjusted return
- **Sortino Ratio**: Downside risk-adjusted return
- **Drawdown**: Peak-to-trough decline

### Module 3: Strategy Engine

**File**: `src/strategies/`, `include/strategies/`

**Tujuan**: Framework untuk implementasi trading strategies.

**Design Pattern**: Strategy Pattern dengan inheritance

```cpp
class BaseStrategy {
    virtual Signal generate_signal(const MarketData&) = 0;
    virtual void on_bar(const Bar&) = 0;
};
```

**Strategies**:
1. **Moving Average Crossover**: Buy saat SMA_fast > SMA_slow
2. **Momentum Strategy**: Buy winners, sell losers
3. **Mean Reversion**: Bet on convergence to mean
4. **Breakout Strategy**: Trade breakouts dari resistance/support

### Module 4: Backtest Engine

**File**: `src/backtester/`, `include/backtester/`

**Tujuan**: Event-driven backtesting framework.

**Event Types**:
- `MarketEvent`: New market data received
- `SignalEvent`: Strategy generates signal
- `OrderEvent`: Order to be executed
- `FillEvent`: Order execution confirmation

**Performance Metrics**:
- CAGR (Compound Annual Growth Rate)
- Sharpe Ratio
- Sortino Ratio
- Maximum Drawdown
- Win Rate
- Profit Factor

### Module 5: Portfolio Engine

**File**: `src/portfolio/`, `include/portfolio/`

**Tujuan**: Optimal capital allocation across strategies/assets.

**Methods**:
1. **Equal Weight**: Simple 1/n allocation
2. **Risk Parity**: Equal risk contribution
3. **Mean-Variance Optimization**: Markowitz portfolio theory

**Mathematics**:
```
Minimize: w'Σw
Subject to: w'μ = target_return
            Σw = 1
```

Dimana:
- w = weight vector
- Σ = covariance matrix
- μ = expected returns vector

### Module 6: Risk Management

**File**: `src/risk/`, `include/risk/`

**Tujuan**: Control exposure and limit losses.

**Components**:
- Position Sizing: Kelly Criterion, Fixed Fractional
- Stop Loss: Hard stops, trailing stops
- Take Profit: Target-based exits
- Risk Budgeting: Allocate risk across positions

### Module 7: Execution Engine

**File**: `src/execution/`, `include/execution/`

**Tujuan**: Simulate order execution with realistic fills.

**Order Types**:
- Market Order: Execute at current price
- Limit Order: Execute at specified price or better
- Stop Order: Trigger market order at stop price

**Slippage Models**:
- Fixed slippage
- Percentage slippage
- Volume-based slippage

### Module 8: Market Simulator

**File**: `src/hft/`, `include/hft/`

**Tujuan**: Simulate exchange matching engine behavior.

**Components**:
- Order Book: Maintain bid/ask queues
- Matching Engine: Price-time priority matching
- Trade Simulator: Generate realistic fills

**Exchange Mechanics**:
1. Orders enter order book
2. Matching engine matches bids with asks
3. Trades executed at agreed price
4. Remaining orders stay in book

### Module 9: Low Latency Systems

**File**: `src/hft/`, `include/hft/`

**Tujuan**: Optimize for minimal latency.

**Techniques**:
- Cache-friendly data structures (contiguous memory)
- Lock-free queues (avoid mutex overhead)
- Ring buffers (pre-allocated circular buffer)
- Memory pools (avoid dynamic allocation)
- Thread pools (reuse threads)

**Performance Concepts**:
- Cache locality: Keep related data together
- False sharing: Avoid multiple threads writing same cache line
- NUMA: Non-Uniform Memory Access awareness
- CPU affinity: Pin threads to specific cores

### Module 10: HFT Module

**File**: `src/hft/`, `include/hft/`

**Tujuan**: High-frequency trading infrastructure.

**Components**:
- Market Data Feed Handler: Parse exchange feeds
- Order Book: Real-time order book maintenance
- Event Processing Engine: Ultra-low latency event handling

**FIX Protocol Basics**:
- FIX (Financial Information eXchange) protocol
- Tag-value message format
- Common messages: NewOrderSingle, ExecutionReport

### Module 11: Visualization

**File**: `dashboard/`

**Tujuan**: Visualize performance and analytics.

**Dashboards**:
- Equity Curve: Portfolio value over time
- Drawdown Chart: Underwater periods
- Returns Distribution: Histogram of returns
- Allocation Pie: Current portfolio weights
- Performance Metrics: Key statistics table

### Module 12: Testing

**File**: `tests/`

**Tujuan**: Ensure correctness through automated tests.

**Test Types**:
- Unit Tests: Test individual components
- Integration Tests: Test component interactions
- Regression Tests: Prevent breaking changes

**Example Test**:
```cpp
TEST(FactorTests, SMACalculation) {
    std::vector<double> prices = {1, 2, 3, 4, 5};
    double sma = calculate_sma(prices, 3);
    EXPECT_DOUBLE_EQ(sma, 4.0);
}
```

### Module 13: Logging

**File**: `src/utils/logger.cpp`, `include/utils/logger.h`

**Tujuan**: Comprehensive logging for debugging and monitoring.

**Log Levels**:
- TRACE: Detailed debugging information
- DEBUG: Diagnostic information
- INFO: General operational messages
- WARN: Warning conditions
- ERROR: Error conditions
- CRITICAL: Critical failures

**Example**:
```cpp
LOG_INFO("Strategy initialized: {}", strategy_name);
LOG_ERROR("Failed to connect to database: {}", error_msg);
```

---

## 🔧 Troubleshooting

### Common Issues and Solutions

#### 1. g++ not found

**Symptom**: `bash: g++: command not found`

**Solution**:
```bash
sudo apt update
sudo apt install -y build-essential g++
```

#### 2. cmake not found

**Symptom**: `bash: cmake: command not found`

**Solution**:
```bash
sudo apt install -y cmake
```

#### 3. linker error -lEigen3 not found

**Symptom**: `/usr/bin/ld: cannot find -lEigen3`

**Solution**:
```bash
sudo apt install -y libeigen3-dev
```

Note: Eigen is header-only, tidak perlu linking. Pastikan include path benar.

#### 4. boost not found

**Symptom**: `fatal error: boost/system.hpp: No such file or directory`

**Solution**:
```bash
sudo apt install -y libboost-all-dev
```

#### 5. eigen not found

**Symptom**: `fatal error: Eigen/Dense: No such file or directory`

**Solution**:
```bash
sudo apt install -y libeigen3-dev
# Verify installation
pkg-config --cflags eigen3
```

#### 6. OpenMP not found

**Symptom**: `fatal error: omp.h: No such file or directory`

**Solution**:
```bash
sudo apt install -y libomp-dev
```

#### 7. CMake configuration error

**Symptom**: `Could NOT find OpenMP`

**Solution**:
```bash
# Check if OpenMP is installed
grep -i openmp /proc/cpuinfo

# Reinstall OpenMP
sudo apt install --reinstall libomp-dev
```

#### 8. WSL2 network issues

**Symptom**: Cannot access network from WSL2

**Solution**:
```powershell
# In PowerShell (Admin)
netsh winsock reset
netsh int ip reset
ipconfig /release
ipconfig /renew
```

Then restart WSL:
```bash
wsl --shutdown
```

#### 9. Permission denied when running executable

**Symptom**: `Permission denied`

**Solution**:
```bash
chmod +x ./quantlab
```

#### 10. Out of memory during build

**Symptom**: `c++: fatal error: Killed signal terminated program cc1plus`

**Solution**:
```bash
# Reduce parallel jobs
cmake --build . -j2

# Or add swap space
sudo fallocate -l 4G /swapfile
sudo chmod 600 /swapfile
sudo mkswap /swapfile
sudo swapon /swapfile
```

---

## 🎓 Learning Roadmap

### LEVEL 0: Computer Fundamentals

**Tujuan**: Memahami dasar-dasar komputer

**Materi**:
- Bagaimana komputer bekerja
- CPU, RAM, Storage
- Binary dan hexadecimal
- Operating system basics

**Latihan**:
- Install Linux dual boot atau VM
- Navigasi file system
- Basic shell commands

**Mini Project**: Setup development environment

**Target**: Bisa install OS dan navigate file system

---

### LEVEL 1: Linux

**Tujuan**: Mahir menggunakan Linux command line

**Materi**:
- Shell basics (bash, zsh)
- File operations (ls, cd, cp, mv, rm)
- Text editors (vim, nano)
- Process management (ps, top, kill)
- Permissions (chmod, chown)
- Package management (apt, yum)

**Latihan**:
- Daily tasks using only terminal
- Write bash scripts
- Configure dotfiles

**Mini Project**: Automate setup script

**Target**: Comfortable working in terminal

---

### LEVEL 2: Git

**Tujuan**: Version control mastery

**Materi**:
- Git basics (init, clone, add, commit)
- Branching and merging
- Remote repositories
- Resolving conflicts
- Git workflows (GitFlow)

**Latihan**:
- Daily commits
- Create branches for features
- Contribute to open source

**Mini Project**: Manage QuantLabCPP dengan Git

**Target**: Fluent Git usage

---

### LEVEL 3: C++ Fundamentals

**Tujuan**: Master C++ syntax dan semantics

**Materi**:
- Variables and types
- Control flow
- Functions
- Classes and objects
- Pointers and references
- Memory management
- STL containers
- Templates basics

**Latihan**:
- Solve problems on LeetCode
- Implement data structures
- Read C++ books (Effective C++)

**Mini Project**: Build calculator, text parser

**Target**: Write correct C++ programs

---

### LEVEL 4: Data Structures & Algorithms

**Tujuan**: Efficient problem solving

**Materi**:
- Arrays, linked lists, stacks, queues
- Trees, graphs, hash tables
- Sorting and searching
- Dynamic programming
- Time/space complexity (Big O)

**Latihan**:
- 100+ LeetCode problems
- Implement all standard DS
- Analyze algorithm complexity

**Mini Project**: Custom STL-like library

**Target**: Optimal algorithm selection

---

### LEVEL 5: Multithreading

**Tujuan**: Concurrent programming

**Materi**:
- Threads and processes
- Mutexes and locks
- Condition variables
- Atomic operations
- Thread pools
- OpenMP

**Latihan**:
- Parallelize algorithms
- Implement lock-free structures
- Debug race conditions

**Mini Project**: Multi-threaded web scraper

**Target**: Safe concurrent code

---

### LEVEL 6: Probability & Statistics

**Tujuan**: Mathematical foundation for quant

**Materi**:
- Probability theory
- Distributions (normal, binomial, etc.)
- Hypothesis testing
- Regression analysis
- Time series analysis

**Latihan**:
- Statistical calculations in C++
- Monte Carlo simulations
- Analyze financial data

**Mini Project**: Statistical analysis toolkit

**Target**: Apply stats to real data

---

### LEVEL 7: Financial Markets

**Tujuan**: Understand market mechanics

**Materi**:
- Asset classes (stocks, bonds, derivatives)
- Market structure (exchanges, ECNs)
- Order types
- Market participants
- Trading hours and sessions

**Latihan**:
- Paper trading
- Follow market news daily
- Analyze market microstructure

**Mini Project**: Market data analyzer

**Target**: Fluency in market terminology

---

### LEVEL 8: Quantitative Finance

**Tujuan**: Quant theory and models

**Materi**:
- Modern Portfolio Theory
- CAPM, Factor Models
- Option pricing (Black-Scholes)
- Risk metrics (VaR, CVaR)
- Stochastic calculus basics

**Latihan**:
- Implement pricing models
- Calculate risk metrics
- Backtest simple strategies

**Mini Project**: Option pricer

**Target**: Apply quant models

---

### LEVEL 9: Backtesting

**Tujuan**: Build robust backtesting systems

**Materi**:
- Event-driven architecture
- Survivorship bias
- Look-ahead bias
- Transaction costs
- Performance metrics

**Latihan**:
- Build backtester from scratch
- Test multiple strategies
- Analyze results critically

**Mini Project**: QuantLabCPP backtester

**Target**: Reliable backtesting

---

### LEVEL 10: Portfolio Construction

**Tujuan**: Optimal capital allocation

**Materi**:
- Mean-variance optimization
- Risk parity
- Black-Litterman model
- Constraints handling
- Rebalancing strategies

**Latihan**:
- Implement optimization algorithms
- Compare allocation methods
- Handle real-world constraints

**Mini Project**: Portfolio optimizer

**Target**: Professional portfolio construction

---

### LEVEL 11: Low Latency Systems

**Tujuan**: Ultra-fast code

**Materi**:
- CPU caches and memory hierarchy
- Branch prediction
- Vectorization (SIMD)
- Lock-free programming
- Kernel bypass techniques

**Latihan**:
- Profile and optimize code
- Implement lock-free queue
- Minimize cache misses

**Mini Project**: Low-latency order book

**Target**: Nanosecond-level optimization

---

### LEVEL 12: HFT Systems

**Tujuan**: High-frequency trading infrastructure

**Materi**:
- Exchange protocols (FIX, OUCH)
- Market data feeds
- Co-location
- FPGA basics
- Regulatory requirements

**Latihan**:
- Build feed handler
- Implement market making logic
- Simulate HFT strategies

**Mini Project**: HFT simulator

**Target**: HFT system understanding

---

### LEVEL 13: Institutional Quant Development

**Tujuan**: Production-ready systems

**Materi**:
- System design
- Risk controls
- Monitoring and alerting
- Disaster recovery
- Compliance

**Latihan**:
- Build end-to-end trading system
- Implement comprehensive monitoring
- Write production documentation

**Mini Project**: Complete QuantLabCPP

**Target**: Ready for hedge fund role

---

## 🤝 Contributing

We welcome contributions! Please follow these steps:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

### Code Style

- Follow Google C++ Style Guide
- Use meaningful variable names
- Comment complex logic
- Write tests for new features

---

## 📄 License

This project is licensed under the MIT License - see the LICENSE file for details.

---

## 📞 Contact

- GitHub: [@yourusername](https://github.com/yourusername)
- Email: your.email@example.com
- LinkedIn: Your Name

---

## 🙏 Acknowledgments

- Inspired by systems used at Citadel, Jane Street, HRT, Two Sigma
- Educational purposes only - not for live trading
- Thanks to the open-source community

---

**Disclaimer**: This software is for educational purposes only. Do not use for live trading without thorough testing and professional advice. Past performance does not guarantee future results. Trading involves substantial risk of loss.
