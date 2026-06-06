cmake_minimum_required(VERSION 3.15)
project(QuantLabCPP VERSION 1.0.0 LANGUAGES CXX)

# Set C++ Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler flags for optimization and warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native -DNDEBUG")

# Find required packages
find_package(OpenMP REQUIRED)
find_package(Eigen3 REQUIRED)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${EIGEN3_INCLUDE_DIR}
)

# Source files - only include files that exist
set(DATA_SOURCES
    src/data/csv_loader.cpp
)

set(FACTOR_SOURCES
    src/factors/trend_factors.cpp
    src/factors/volatility_factors.cpp
    src/factors/volume_risk_factors.cpp
)

set(STRATEGY_SOURCES
    src/strategies/base_strategy.cpp
)

set(BACKTESTER_SOURCES
    src/backtester/event_driven_backtester.cpp
)

set(PORTFOLIO_SOURCES
    src/portfolio/equal_weight.cpp
    src/portfolio/optimization.cpp
)

set(UTILS_SOURCES
    src/utils/logger.cpp
    src/utils/helpers.cpp
)

# Combine all sources
set(ALL_SOURCES
    src/main.cpp
    ${DATA_SOURCES}
    ${FACTOR_SOURCES}
    ${STRATEGY_SOURCES}
    ${BACKTESTER_SOURCES}
    ${PORTFOLIO_SOURCES}
    ${UTILS_SOURCES}
)

# Main executable
add_executable(quantlab ${ALL_SOURCES})

# Link libraries
target_link_libraries(quantlab 
    OpenMP::OpenMP_CXX
)

# Install target
install(TARGETS quantlab DESTINATION bin)

# Print configuration summary
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "OpenMP found: ${OpenMP_FOUND}")
message(STATUS "Eigen3 found: ${Eigen3_FOUND}")
