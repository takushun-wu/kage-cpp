#ifndef _FITCURVE_H
#define _FITCURVE_H

#include <array>
#include <cmath>
#include <vector>


#include "point.h"

namespace Kage {

    typedef struct std::array<Point, 4> CubicSpline;

    std::vector<CubicSpline> FitCurve(
        std::vector<Point> points, double maxError);

} // namespace Kage

#endif
