#ifndef _CANVA_H
#define _CANVA_H

#include <functional>

#include "contour.h"
#include "point.h"

namespace Kage {

    class Canva {
        std::vector<Contour> _contours;

    public:
        Canva();

        std::vector<Contour> Contours();

        void   Clear();
        size_t Push(Contour contour);
        size_t Concat(Canva canva);
        size_t Size();

        void DrawLine(Point p1, Point p2, double halfWidth);
        void DrawQBezier(Point p1, Point ps, Point p2,
            std::function<double(double)> const& width_func,
            std::function<double(double)> const& width_func_d,
            size_t curve_step, bool fix_begin, bool fix_end);
        void DrawCBezier(Point p1, Point ps1, Point ps2, Point p2,
            std::function<double(double)> const& width_func,
            std::function<double(double)> const& width_func_d,
            size_t                               curve_step);

        void FlipLR(Point p1, Point p2);
        void FlipUD(Point p1, Point p2);
        void Rotate90(Point p1, Point p2);
        void Rotate180(Point p1, Point p2);
        void Rotate270(Point p1, Point p2);
    };

} // namespace Kage

#endif
