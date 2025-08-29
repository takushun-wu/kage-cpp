#include <cmath>

#include "bezier.h"
#include "gothic.h"

namespace Kage {

    const auto CURVE_THIN = 0.1910;
    const auto BEZ_CIR    = 4 * (std::sqrt(2) - 1) / 3;

    // gothic.js/Gothic/constructor
    Gothic::Gothic(double size) {
        _type = KAGEFONT_GOTHIC;
        if(size == 1)
            kWidth = 3, kKakato = 1.8, kMage = 6;
        else
            kWidth = 5, kKakato = 3, kMage = 10;
    }

    // util.js/get_extended_dest_wrong
    // The process for lines directed exactly in the negative x-direction or
    // y-direction is not correct, so it's named as "wrong".
    Point GetExtendedDestWrong(Point dest, Point src, double delta) {
        auto dest_new = dest;
        if(src.x == dest.x)
            dest_new.y = dest.y + delta;
        else if(src.y == dest.y)
            dest_new.x = dest.x + delta;
        else {
            const double rad = std::atan((dest.y - src.y) / (dest.x - src.x));
            int          v   = src.x < dest.x ? 1 : -1;
            dest_new.x       = dest.x + delta * std::cos(rad) * v,
            dest_new.y       = dest.y + delta * std::sin(rad) * v;
        }
        return dest_new;
    }

    // gothic.js/Gothic/gothicDrawLine
    void Gothic::DrawLine(
        Canva& cv, Point pt1, Point pt2, StartType ta1, EndType ta2) {
        auto p1 = pt1, p2 = pt2;
        if(ta1 % 10 == 2)
            p1 = pt2.GetExtendedDest(pt1, kWidth);
        else if(ta1 % 10 == 3)
            p1 = pt2.GetExtendedDest(pt1, kWidth * kKakato);
        if(ta2 % 10 == 2)
            p2 = pt1.GetExtendedDest(pt2, kWidth);
        else if(ta2 % 10 == 3)
            p2 = pt1.GetExtendedDest(pt2, kWidth * kKakato);
        cv.DrawLine(p1, p2, kWidth);
    }

    // gothic.js/Gothic/gothicDrawCurve
    void Gothic::DrawCurve(Canva& cv, Point p1, Point p2, Point p3,
        StrokeType ta1, StartType ta2) {
        unsigned a1 = 0, a2 = 0;
        if(a1 % 10 == 2)
            p1 = GetExtendedDestWrong(p1, p2, kWidth);
        else if(a1 % 10 == 3)
            p1 = GetExtendedDestWrong(p1, p2, kWidth * kKakato);
        if(a2 % 10 == 2)
            p3 = GetExtendedDestWrong(p3, p2, kWidth);
        else if(a2 % 10 == 3)
            p3 = GetExtendedDestWrong(p3, p2, kWidth * kKakato);
        cv.DrawQBezier(p1, p2, p3, [this](double t) -> double {
            return kWidth;
        }, [](double t) -> double { return 0; }, 1000 / kRate, false, false);
    }

    // gothic.js/Gothic/drawStroke
    void Gothic::DrawStroke(Canva& cv, Stroke s) {
        switch(s.type) {
        case STROKE_SPECIAL: {
            if(s.spe1 == SPECIAL1_FLIP_LEFTRIGHT)
                cv.FlipLR(s.p1, s.p2);
            else if(s.spe1 == SPECIAL1_FLIP_UPDOWN)
                cv.FlipUD(s.p1, s.p2);
            else if(s.spe1 == SPECIAL1_ROTATE && s.spe2 == SPECIAL2_ROTATE_90)
                cv.Rotate90(s.p1, s.p2);
            else if(s.spe1 == SPECIAL1_ROTATE && s.spe2 == SPECIAL2_ROTATE_180)
                cv.Rotate180(s.p1, s.p2);
            else if(s.spe1 == SPECIAL1_ROTATE && s.spe2 == SPECIAL2_ROTATE_270)
                cv.Rotate270(s.p1, s.p2);
            break;
        } // Modified(2025/8/28)
        case STROKE_STRAIGHT: {
            if(s.end == END_TURN_LEFT) {
                auto pt1 = s.p1.GetExtendedDest(s.p2, -kMage);
                DrawLine(
                    cv, s.p1, pt1, s.start, END_INTERNAL_LAST_FILLED_CIRCLE);
                DrawCurve(cv, pt1, s.p2, s.p2 - Point(kMage * 2, kMage * 0.5),
                    s.type, s.start);
            } else
                DrawLine(cv, s.p1, s.p2, s.start, s.end);
            break;
        }
        case STROKE_CURVE:
        case 12:           {
            if(s.end == END_TURN_LEFT) {
                auto pt1 = GetExtendedDestWrong(s.p3, s.p2, -kMage);
                DrawCurve(cv, s.p1, s.p2, pt1, s.type, s.start);
                DrawCurve(cv, pt1, s.p3, s.p3 - Point(kMage * 2, kMage * 0.5),
                    s.type, s.start);
            } else if(s.end == END_TURN_UPWARDS) {
                auto pt1 = s.p3 + Point(kMage, 0),
                     pt2 = Point(pt1.x + kMage * 0.5, s.p3.y - kMage * 2);
                DrawCurve(cv, s.p1, s.p2, s.p3, s.type, s.start);
                DrawCurve(cv, s.p3, pt1, pt2, s.type, s.start);
            } else
                DrawCurve(cv, s.p1, s.p2, s.p3, s.type, s.start);
            break;
        }
        case STROKE_BENDING: {
            auto pt1 = s.p1.GetExtendedDest(s.p2, -kMage),
                 pt2 = s.p3.GetExtendedDest(s.p2, -kMage);
            if(s.end == END_TURN_UPWARDS) {
                auto pt3 = s.p3 - Point(kMage, 0),
                     pt4 = s.p3 + Point(kMage * 0.5, -kMage * 2);
                DrawLine(
                    cv, s.p1, pt1, s.start, END_INTERNAL_LAST_FILLED_CIRCLE);
                DrawCurve(cv, pt1, s.p2, pt2, s.type, s.start);
                DrawLine(cv, pt2, pt3, START_INTERNAL_PLANE,
                    END_INTERNAL_LAST_FILLED_CIRCLE);
                DrawCurve(cv, pt3, s.p3, pt4, s.type, s.start);
            } else {
                DrawLine(
                    cv, s.p1, pt1, s.start, END_INTERNAL_LAST_FILLED_CIRCLE);
                DrawCurve(cv, pt1, s.p2, pt2, s.type, s.start);
                DrawLine(cv, pt2, s.p3, START_INTERNAL_PLANE, s.end);
            }
            break;
        }
        case STROKE_BENDING_ROUND: {
            double rate = 6;
            if((s.p3 - s.p2).GetLength() < 120) // smaller than 120 x 120
                rate = (s.p3 - s.p2).GetLength() / 120 * 6;
            auto pt1 = s.p1.GetExtendedDest(s.p2, -kMage * rate),
                 pt2 = s.p3.GetExtendedDest(s.p2, -kMage * rate);
            if(s.end == END_TURN_UPWARDS) {
                auto pt3 = s.p3 - Point(kMage, 0),
                     pt4 = s.p3 + Point(kMage * 0.5, -kMage * 2);
                DrawLine(
                    cv, s.p1, pt1, s.start, END_INTERNAL_LAST_FILLED_CIRCLE);
                DrawCurve(cv, pt1, s.p2, pt2, s.type, s.start);
                DrawLine(cv, pt2, pt3, START_INTERNAL_PLANE,
                    END_INTERNAL_LAST_FILLED_CIRCLE);
                DrawCurve(cv, pt3, s.p3, pt4, s.type, s.start);
            } else {
                DrawLine(
                    cv, s.p1, pt1, s.start, END_INTERNAL_LAST_FILLED_CIRCLE);
                DrawCurve(cv, pt1, s.p2, pt2, s.type, s.start);
                DrawLine(cv, pt2, s.p3, START_INTERNAL_PLANE, s.end);
            }
            break;
        } // Modified(2025/8/28)
        case STROKE_BEZIER: {
            if(s.end == END_TURN_UPWARDS) {
                auto pt1 = s.p4 - Point(kMage, 0),
                     pt2 = s.p4 + Point(kMage * 0.5, -kMage * 2);
                cv.DrawCBezier(s.p1, s.p2, s.p3, pt1,
                    [this](double t) -> double { return kWidth; },
                    [](double t) -> double { return 0; }, 1000 / kRate);
                DrawCurve(cv, pt1, s.p4, pt2, s.type, s.start);
            } else
                cv.DrawCBezier(s.p1, s.p2, s.p3, s.p4,
                    [this](double t) -> double { return kWidth; },
                    [](double t) -> double { return 0; }, 1000 / kRate);
            break;
        }
        case STROKE_VCURVE: {
            DrawLine(cv, s.p1, s.p2, s.start, END_INTERNAL_LAST_FILLED_CIRCLE);
            DrawCurve(cv, s.p2, s.p3, s.p4, s.type, s.start);
            break;
        }
        case 9: // may not be exist ... no need
            // kageCanvas[y1][x1] = 0;
            // kageCanvas[y2][x2] = 0;
            break;
        default: break;
        }
    }

    // gothic.js/Gothic/getPolygons
    Canva Gothic::DrawGlyph(std::vector<Stroke> strokes) {
        Canva cv;
        for(auto stroke: strokes) {
            DrawStroke(cv, stroke);
        }
        return cv;
    }

} // namespace Kage
