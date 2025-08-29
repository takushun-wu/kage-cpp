#ifndef _GOTHIC_H
#define _GOTHIC_H

#include <array>
#include <vector>

#include "canva.h"
#include "gwdata.h"
#include "kagefont.h"

namespace Kage {

#define PI 3.14159265358979

    typedef struct std::array<Point, 2> PointArray2;

    class Gothic: public KageFont {
        double kRate = 50;
        // 基本粗细参数
        double kWidth, kKakato, kMage;

        void DrawStroke(Canva& cv, Stroke s);

        void DrawLine(
            Canva& cv, Point pt1, Point pt2, StartType ta1, EndType ta2);
        void DrawCurve(Canva& cv, Point p1, Point p2, Point p3, StrokeType ta1,
            StartType ta2);

    public:
        Gothic(double size = 0);
        ~Gothic() = default;
        Canva DrawGlyph(std::vector<Stroke> strokes);
    };

} // namespace Kage

#endif
