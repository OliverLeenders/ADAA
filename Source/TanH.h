#pragma once

#include <JuceHeader.h>

namespace TanH {
    static double (*f)(double) = tanh;
    static double log_cosh(double x) { return std::log(std::cosh(x)); };
    
    static double (*F)(double) = log_cosh;
}; // namespace TanH