#ifndef _BEZIER_H
#define _BEZIER_H

#include <array>
#include <cmath>
#include <cstdint>
#include <functional>
#include <vector>

#include "contour.h"
#include "fitcurve.h"
#include "point.h"

namespace Kage {

#define PI 3.14159265358979

    typedef struct std::array<Point, 4> CubicSpline;

    const int BEZIER_STEPS = 200;

    void qBezier(Point p1, Point c, Point p2,
        std::function<double(double)> const& width_func,
        std::function<double(double)> const& width_func_d,
        std::vector<CubicSpline>& bez1, std::vector<CubicSpline>& bez2);
    void qBezier2(Point p1, Point c, Point p2,
        std::function<double(double)> const& width_func,
        std::function<double(double)> const& width_func_d,
        std::vector<CubicSpline>& bez1, std::vector<CubicSpline>& bez2);
    void cBezier(Point p1, Point c1, Point c2, Point p2,
        std::function<double(double)> const& width_func,
        std::function<double(double)> const& width_func_d,
        std::vector<CubicSpline>& bez1, std::vector<CubicSpline>& bez2);

    CubicSpline Bezier2y(CubicSpline bez, double y);
    CubicSpline Bezier2Line(CubicSpline bez, Point p0, double rad);

    Contour CubicSpline2Contour(std::vector<CubicSpline> bezs);

} // namespace Kage

#endif
