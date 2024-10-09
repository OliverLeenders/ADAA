#pragma once

#include <vector>
#include <corecrt_math.h>
struct cubic_spline {
    double a = 0.0;
    double b = 0.0;
    double c = 0.0;
    double d = 0.0;
};

class Interpolator {
public:
    Interpolator();
    Interpolator (std::vector<double> y);
    double interpolate(double x);
    double interpolate_unprepared(double x);
    double interpolate_unprepared(double x, double RANGE);
    private:
    std::vector<cubic_spline> splines;
};


