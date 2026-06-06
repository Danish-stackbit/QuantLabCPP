/**
 * @file equal_weight.cpp
 * @brief Equal weight portfolio allocation implementation
 */

#include "portfolio.h"

namespace quantlab {

std::vector<double> PortfolioEngine::equal_weight(int n_assets) {
    if (n_assets <= 0) {
        return {};
    }
    
    double weight = 1.0 / n_assets;
    std::vector<double> weights(n_assets, weight);
    
    return weights;
}

} // namespace quantlab
