#pragma once

#include <JuceHeader.h>
#include <vector>

namespace TanHLerp {
    static double RANGE;
    const static int NUM_POINTS = 2048;
    static std::vector<double> tanh_arr(NUM_POINTS);
    static std::vector<double> log_cosh_arr(NUM_POINTS);
    static double lerp(double l, double r, double lambda) { return (1 - lambda) * l + lambda * r; }
    
    static double interpolate_value(std::vector<double> &arr, double x) {
        x /= RANGE;
        // pos range
        x = 0.5 * (x + 1);
        x = std::min(x, 1.0);
        x = std::max(x, 0.0);
        // compute indices
        double scaled = x * (NUM_POINTS);
        double floor = std::floor(scaled);
        double ceil = std::ceil(scaled);

        double lfactor = ceil - scaled;
        double rfactor = scaled - floor;

        // REALLY make sure we don't get out of bounds...
        int lower_index = (int)std::min(floor, (double)NUM_POINTS - 1.0);
        lower_index = std::max(0, lower_index);
        int higher_index = (int)std::min(ceil, (double)NUM_POINTS - 1.0);
        higher_index = std::max(0, higher_index);

        double lower_val = arr[lower_index];
        double higher_val = arr[higher_index];
        return lerp(lower_val, higher_val, scaled - floor);
    }

    static double compute_tanh(double x) { return interpolate_value(tanh_arr, x); }
    static double compute_log_cosh(double x) { return interpolate_value(log_cosh_arr, x); }
    static double (*f)(double) = compute_tanh;
    static double (*F)(double) = compute_log_cosh;


} // namespace TanHLerp