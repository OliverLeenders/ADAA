#include "Interpolator.h"

Interpolator::Interpolator() {
    std::vector<cubic_spline> s;
    this->splines = s;
}

Interpolator::Interpolator(std::vector<double> y) {
    // https://en.wikipedia.org/w/index.php?title=Spline_%28mathematics%29&oldid=288288033#Algorithm_for_computing_natural_cubic_splines
    size_t n = y.size();
    // 1.
    std::vector<double> a(n + 1);
    for (int i = 0; i < n; i++) {
        a[i] = y[i];
    }
    // 2.
    std::vector<double> b(n), d(n);
    // 3.
    std::vector<double> h(n);
    for (size_t i = 0; i < n - 1; i++) {
        h[i] = 1.0;
    }
    // 4.
    std::vector<double> alpha(n);
    for (size_t i = 1; i < n - 1; i++) {
        alpha[i] = 3 / h[i] * (a[i + 1] - a[i]) - 3 / h[i - 1] * (a[i] - a[i - 1]);
    }
    // 5.
    std::vector<double> c(n + 1), l(n + 1), mu(n + 1), z(n + 1);
    // 6.
    l[0] = 1.0;
    mu[0] = z[0] = 0.0;
    // 7.
    for (size_t i = 1; i < n - 1; i++) {
        l[i] = 4 - h[i - 1] * mu[i - 1];
        mu[i] = h[i] / l[i];
        z[i] = (alpha[i] - h[i - 1] * z[i - 1]) / l[i];
    }
    // 8.
    l[n] = 1.0;
    z[n] = c[n] = 0.0;
    // 9.
    for (long long j = n - 1; j >= 0; j--) {
        c[j] = z[j] - mu[j] * c[j + 1];
        b[j] = ((a[j + 1] - a[j]) / h[j]) - (h[j] * (c[j + 1] + 2 * c[j]) / 3);
        d[j] = (c[j + 1] - c[j]) / (3 * h[j]);
    }

    std::vector<cubic_spline> splines(n);
    for (int i = 0; i < n; i++) {
        cubic_spline s;
        s.a = a[i];
        s.b = b[i];
        s.c = c[i];
        s.d = d[i];
        splines[i] = s;
    }
    this->splines = splines;
}

double Interpolator::interpolate(double x) {
    double floor = __floor(x);
    int idx = (int)std::max(0.0, floor);
    cubic_spline s = this->splines[idx];
    double x_t = x - idx;
    return s.a + s.b * x_t + s.c * x_t * x_t + s.d * x_t * x_t * x_t;
    // return 0.0;
}

double Interpolator::interpolate_unprepared(double x, double RANGE) {
    x /= RANGE;
    // shift for index calculation
    x += 1.0;
    x /= 2;

    x *= this->splines.size();

    double floor = __floor(x);
    int idx = (int)std::max(0.0, floor);
    cubic_spline s = this->splines[idx];
    double x_t = x - idx;
    
    return s.a + s.b * x_t + s.c * x_t * x_t + s.d * x_t * x_t * x_t;
}

double Interpolator::interpolate_unprepared(double x) {
    // shift for index calculation
    x += 1.0;
    x /= 2;

    x *= this->splines.size();

    double floor = __floor(x);
    int idx = (int)std::max(0.0, floor);
    idx = std::min((size_t)idx, this->splines.size() - 1);
    cubic_spline s = this->splines[idx];
    double x_t = x - idx;

    return s.a + s.b * x_t + s.c * x_t * x_t + s.d * x_t * x_t * x_t;
}
