#pragma once

#include "Interpolator.h"

#include <JuceHeader.h>

namespace TanHSpline {
    static double RANGE;
    static Interpolator smooth_tanh;
    static Interpolator smooth_log_cosh;

    static double compute_tanh(double x) { return smooth_tanh.interpolate_unprepared(x, RANGE); }
    static double compute_log_cosh(double x) { return smooth_log_cosh.interpolate_unprepared(x, RANGE); }
    static double (*f)(double) = compute_tanh;
    static double (*F)(double) = compute_log_cosh;

} // namespace TanHSpline