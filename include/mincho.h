#ifndef _MINCHO_H
#define _MINCHO_H

#include <array>
#include <vector>

#include "canva.h"
#include "gwdata.h"
#include "kagefont.h"

namespace Kage {

#define HAIR_LINE   -5
#define EXTRA_LIGHT -4
#define LIGHT       -3
#define MEDIUM      -2
#define DEMIBOLD    -1

#define PI 3.14159265358979

    typedef struct std::array<Point, 2> PointArray2;

    class Mincho: public KageFont {
        double kRate = 50;
        // 基本粗细参数
        double kMinWidthY, kMinWidthU, kMinWidthT, kWidth, kKakato, kL2RDfatten,
            kMage;
        // 竖的末端
        std::vector<double> kAdjustKakatoL, kAdjustKakatoR, kAdjustKakatoRangeY;
        double              kAdjustKakatoRangeX;
        size_t              kAdjustKakatoStep;
        // 横尾端小三角
        std::vector<double> kAdjustUrokoX, kAdjustUrokoY, kAdjustUrokoLength,
            kAdjustUrokoLine;
        size_t kAdjustUrokoLengthStep;
        //
        double kAdjustUroko2Step, kAdjustUroko2Length;
        double kAdjustTateStep, kAdjustMageStep, kMinWidthYY, kMinWidthT_adjust,
            kMinWidthC;

        void DrawAdjustedStroke(
            Canva& cv, Stroke s, std::vector<Stroke> others);

        double AdjustUrokoParam(Stroke stroke, std::vector<Stroke> others);
        double AdjustUroko2Param(Stroke stroke, std::vector<Stroke> others);
        double AdjustTateParam(Stroke stroke, std::vector<Stroke> others);
        double AdjustHaneParam(
            Stroke stroke, Point ep, std::vector<Stroke> others);
        double AdjustKakatoParam(Stroke stroke, std::vector<Stroke> others);
        bool   AdjustKirikuchiParam(Stroke stroke, std::vector<Stroke> others);
        double AdjustMageParam(Stroke stroke, std::vector<Stroke> others);

        Contour GetStartOfVLine(
            Point p1, Point p2, StartType a1, double kMinWidthT, Canva& cv);

        void        DrawCurve(Canva& cv, Point p1pre, Point ps, Point p2pre,
                   StartType a1, bool a1adjusted, EndType a2, double kMinWidthT_mod,
                   double end_width_factor);
        PointArray2 DrawBezier(Canva& cv, Point p1pre, Point ps1, Point ps2,
            Point p2pre, StartType a1, bool a1adjusted, EndType a2,
            double kMinWidthT_mod);

    public:
        Mincho(double size = 0);
        ~Mincho() = default;
        Canva DrawGlyph(std::vector<Stroke> strokes);
    };

} // namespace Kage

#endif
