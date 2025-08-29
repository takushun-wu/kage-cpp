#ifndef _CONTOUR_H
#define _CONTOUR_H

#include <cstdint>
#include <vector>

#include "point.h"

namespace Kage {

    // 1=Quadratic, 2=Cubic
    typedef uint8_t ptType;

    const ptType PT_POINT = 0;
    const ptType PT_QUAD  = 1;
    const ptType PT_CUBIC = 2;

    typedef struct {
        ptType ptType;
        Point  pt;
    } PointTuple;

    class Contour {
        std::vector<ptType> ptTypes;
        std::vector<Point>  pts;
        bool                _isClosed;

    public:
        Contour();
        Contour(size_t num);

        std::vector<ptType>& PointTypes();
        std::vector<Point>&  Points();
        bool                 isClosed();

        size_t     Push(Point p, ptType type);
        size_t     Push(PointTuple ptuple);
        void       Set(size_t ind, Point p, ptType type = PT_POINT);
        void       Reverse();
        size_t     Concat(Contour contour);
        PointTuple Shift();
        void       SetClosed(bool closed);

        size_t Size();
        void   MoveTo(Point p);
        void   LineTo(Point p);
        void   QuadraticTo(Point p1, Point p2);
        void   CubicTo(Point p1, Point p2, Point p3);
    };

} // namespace Kage

#endif
