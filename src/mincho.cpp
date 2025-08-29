#include <cmath>

#include "bezier.h"
#include "mincho.h"

namespace Kage {

    const auto CURVE_THIN = 0.1910;
    const auto BEZ_CIR    = 4 * (std::sqrt(2) - 1) / 3;

    // mincho.js/Mincho/constructor
    Mincho::Mincho(double size) {
        _type = KAGEFONT_MINCHO;
        if(size == 1) {
            kMinWidthY = 1.2, kMinWidthU = 1.2, kMinWidthT = 3.6, kWidth = 3,
            kKakato = 1.8, kL2RDfatten = 1.1, kMage = 6;

            kAdjustKakatoL = {
                8, 5, 3, 1}; // for KAKATO adjustment 000,100,200,300,400
            kAdjustKakatoR = {
                4, 3, 2, 1};          // for KAKATO adjustment 000,100,200,300
            kAdjustKakatoRangeX = 12; // check area width
            kAdjustKakatoRangeY = {1, 11, 14, 18}; // 3 steps of checking
            kAdjustKakatoStep   = 3;               // number of steps

            kAdjustUrokoX = {
                14, 12, 9, 7}; // for UROKO adjustment 000,100,200,300
            kAdjustUrokoY = {
                7, 6, 5, 4}; // for UROKO adjustment 000,100,200,300
            kAdjustUrokoLength     = {13, 21, 30}; // length for checking
            kAdjustUrokoLengthStep = 3;            // number of steps
            kAdjustUrokoLine       = {
                13, 15, 18}; // check for crossing. corresponds to length

            kAdjustUroko2Step = 3, kAdjustUroko2Length = 25;
            kAdjustTateStep = 4, kAdjustUroko2Length = 5;
        } else if(size == 3) {
            kMinWidthY = 3, kMinWidthU = 3, kMinWidthT = 8, kWidth = 6,
            kKakato = 4, kL2RDfatten = 1.1, kMage = 14;

            kAdjustKakatoL = {
                20, 13, 7, 3}; // for KAKATO adjustment 000,100,200,300,400
            kAdjustKakatoR = {
                12, 9, 6, 3};         // for KAKATO adjustment 000,100,200,300
            kAdjustKakatoRangeX = 26; // check area width
            kAdjustKakatoRangeY = {2, 26, 30, 40}; // 3 steps of checking
            kAdjustKakatoStep   = 3;               // number of steps

            kAdjustUrokoX = {
                30, 25, 20, 15}; // for UROKO adjustment 000,100,200,300
            kAdjustUrokoY = {
                15, 14, 13, 12}; // for UROKO adjustment 000,100,200,300
            kAdjustUrokoLength     = {29, 40, 62}; // length for checking
            kAdjustUrokoLengthStep = 3;            // number of steps
            kAdjustUrokoLine       = {
                29, 34, 39}; // check for crossing. corresponds to length

            kAdjustUroko2Step = 3, kAdjustUroko2Length = 50;
            kAdjustTateStep = 4, kAdjustMageStep = 5;
        } else if(size > 1) {
            kMinWidthY = size, kMinWidthU = size, kMinWidthT = size * 2.6,
            kWidth = size * 2.2, kKakato = size * 1.2 + 0.6, kL2RDfatten = 1.2,
            kMage = size * 4 + 2;

            kAdjustKakatoL = {14, 11, 8, 6, 4,
                2}; // for KAKATO adjustment 000,100,200,300,400
            kAdjustKakatoR = {
                8, 6.5, 5, 4.5, 3, 2}; // for KAKATO adjustment 000,100,200,300
            kAdjustKakatoRangeX = 20;  // check area width
            kAdjustKakatoRangeY = {3, 9, 15, 21, 27, 32}; // 5 steps of checking
            kAdjustKakatoStep   = 5;                      // number of steps

            kAdjustUrokoX = {size * 9.5 + 4, size * 8 + 3.5, size * 6.5 + 3,
                size * 5 + 2.5}; // for UROKO adjustment 000,100,200,300
            kAdjustUrokoY = {size * 4.6 + 2, size * 4.4 + 1.5, size * 4.2 + 1,
                size * 4.0 + 0.5}; // for UROKO adjustment 000,100,200,300
            kAdjustUrokoLength     = {size * 7 + 7, size * 11 + 11,
                    size * 15 + 15};    // length for checking
            kAdjustUrokoLengthStep = 3; // number of steps
            kAdjustUrokoLine       = {size * 7 + 7, size * 9 + 8,
                      size * 11 + 9}; // check for crossing. corresponds to length

            kAdjustUroko2Step = 3, kAdjustUroko2Length = size * 20;
            kAdjustTateStep = 4, kAdjustMageStep = 5;
        } else {
            kMinWidthY = 2,
            kMinWidthYY =
                2; // 横線の太さのみを決める。kMinWidthY以上の値が望ましい
            kMinWidthYY *= 1.2;
            kMinWidthU = 2, kMinWidthT = 6,
            kMinWidthT_adjust = 6; // use consistent parameter for adjustment
            kMinWidthC  = 1; // 開放・左下右下カドの傾きを決める。1なら元と同じ
            kMinWidthC *= 0.8;
            kWidth = 5, kKakato = 3, kL2RDfatten = 1.1, kMage = 10;
            if(size == HAIR_LINE) // デバッグ用
                kMinWidthY *= 0.3, kMinWidthYY *= 0.3, kMinWidthT *= 0.2;
            else if(size == EXTRA_LIGHT)
                kMinWidthU *= 0.71, kMinWidthY *= 0.89, kMinWidthYY *= 0.85,
                    kMinWidthT *= 0.76;
            else if(size == LIGHT)
                kMinWidthU *= 0.85, kMinWidthY *= 0.94, kMinWidthYY *= 0.92,
                    kMinWidthT *= 0.87;
            else if(size == MEDIUM)
                kMinWidthU *= 1.2, kMinWidthYY *= 1.1, kMinWidthT *= 1.15;
            else if(size == DEMIBOLD) // 曲線に課題あり
                kMinWidthU *= 1.44, kMinWidthYY *= 1.21, kMinWidthT *= 1.34;

            kAdjustKakatoL = {12, 9.6, 7.3, 5, 3, 2}; // for KAKATO adjustment
            kAdjustKakatoR = {10, 8, 6, 4, 2, 1};     // for KAKATO adjustment
            kAdjustKakatoRangeX = 20;                 // check area width
            kAdjustKakatoRangeY = {1, 14, 19, 24, 29,
                35};                 //[0]は0以上[1]以下であればなんでもよい？
            kAdjustKakatoStep   = 5; // number of steps

            kAdjustUrokoX = {
                24, 21, 18, 16, 14, 12}; // for UROKO adjustment 000,100,200,300
            kAdjustUrokoY = {
                12, 11, 10, 9, 8.5, 8}; // for UROKO adjustment 000,100,200,300
            kAdjustUrokoLength = {
                16, 23, 30, 38, 46, 55}; // length for checking
            kAdjustUrokoLengthStep = 5;  // number of steps
            kAdjustUrokoLine       = {18, 20, 23, 26, 30,
                      35}; // check for crossing. corresponds to length

            kAdjustUroko2Step   = 5; // max value
            kAdjustUroko2Length = 40;

            kAdjustTateStep = 4;
            kAdjustMageStep = 5;
        }
    }

    // util.js/widfun_stop
    double WidFuncStop(double t, Point p1, Point p2, double wid) {
        const auto len = (p2 - p1).GetLength(), p = 1 + std::sqrt(100 / len);
        return ((std::sqrt(p * p + (p - 1) * (p - 1) - (p - t) * (p - t)) -
                    (p - 1)) *
                       (1.10 - CURVE_THIN) +
                   CURVE_THIN) *
            wid;
    }

    // util.js/widfun_stop_d
    double WidFuncStopD(double t, Point p1, Point p2, double wid) {
        const auto len = (p2 - p1).GetLength(), p = 1 + std::sqrt(100 / len);
        return wid * (1.10 - CURVE_THIN) * 0.5 * 2 * (p - t) /
            std::sqrt(p * p + (p - 1) * (p - 1) - (p - t) * (p - t));
    }

    // util.js/widfun
    double WidFunc(double t, Point p1, Point p2, double wid) {
        const auto len = (p2 - p1).GetLength(), p = 1 + std::sqrt(100 / len);
        return ((std::sqrt(p * p + (p - 1) * (p - 1) - (p - t) * (p - t)) -
                    (p - 1)) *
                       (1 - CURVE_THIN) +
                   CURVE_THIN) *
            wid;
    }

    // util.js/widfun_d
    double WidFuncD(double t, Point p1, Point p2, double wid) {
        const auto len = (p2 - p1).GetLength(), p = 1 + std::sqrt(100 / len);
        return wid * (1 - CURVE_THIN) * 0.5 * 2 * (p - t) /
            std::sqrt(p * p + (p - 1) * (p - 1) - (p - t) * (p - t));
    }

    // fat version (used in cubic bezier)
    //  util.js/widfun_fat
    double WidFuncFat(double t, Point p1, Point p2, double wid) {
        const auto len = (p2 - p1).GetLength(), p = 1 + std::sqrt(40 / len);
        return ((std::sqrt(p * p + (p - 1) * (p - 1) - (p - t) * (p - t)) -
                    (p - 1)) *
                       (1 - CURVE_THIN) +
                   CURVE_THIN) *
            wid;
    }

    // util.js/widfun_fat_d
    double WidFuncFatD(double t, Point p1, Point p2, double wid) {
        const auto len = (p2 - p1).GetLength(), p = 1 + std::sqrt(40 / len);
        return wid * (1 - CURVE_THIN) * 0.5 * 2 * (p - t) /
            std::sqrt(p * p + (p - 1) * (p - 1) - (p - t) * (p - t));
    }

    // fontcanvas.js/FontCanvas/drawUroko
    void DrawUroko(Canva& cv, Point p2, Dir dir, double kagekMinWidthY,
        double urokoX, double urokoY) {
        Contour contour;
        contour.MoveTo(Point(
            p2.x + dir.sin * kagekMinWidthY, p2.y - dir.cos * kagekMinWidthY));
        contour.LineTo(Point(p2.x - dir.cos * urokoX, p2.y - dir.sin * urokoX));
        contour.LineTo(Point(p2.x - dir.cos * urokoX / 2 + dir.sin * urokoY,
            p2.y - dir.sin * urokoX / 2 - dir.cos * urokoY));
        cv.Push(contour);
    }

    // fontcanvas.js/FontCanvas/drawUroko_h
    void DrawUrokoH(Canva& cv, Point p2, double kagekMinWidthY, double urokoX,
        double urokoY) {
        DrawUroko(cv, p2, DIR_POSX, kagekMinWidthY, urokoX, urokoY);
    }

    // fontcanvas.js/FontCanvas/drawUpperRightCorner_straight_v
    void DrawUpperRightCornerStraightV(Canva& cv, Point p1, double kMinWidthT,
        double kagekMinWidthY, double kagekWidth) {
        PointMaker p(p1);
        Contour    poly;
        poly.MoveTo(p.Vec(Point(-kMinWidthT, -kagekMinWidthY)));
        poly.LineTo(p.Vec(Point(0, -kagekMinWidthY - kagekWidth)));
        poly.LineTo(p.Vec(Point(kMinWidthT + kagekWidth, kagekMinWidthY)));
        poly.LineTo(p.Vec(Point(kMinWidthT, kMinWidthT)));
        poly.LineTo(p.Vec(Point(-kMinWidthT, 0)));
        cv.Push(poly);
    }

    // fontcanvas.js/FontCanvas/drawOpenBegin_straight
    void DrawOpenBeginStraight(Canva& cv, Point p1, double kMinWidthT,
        double kagekMinWidthY, double rad) {
        const auto dir = Rad2Dir((rad + PI * 0.621) * 0.5);
        Contour    poly;
        PointMaker p0(p1, Rad2Dir(rad), kMinWidthT);
        poly.MoveTo(p0.Vec(Point(1, 1)));
        poly.LineTo(p0.Vec(Point(std::sin((rad - PI * 0.621) * 0.5), 1)));
        PointMaker pm1(p1, dir);
        PointMaker p2(pm1.Vec(Point(0, -kMinWidthT)), dir, kMinWidthT * 0.876);
        poly.LineTo(p2.Vec(Point(0, 0)));
        poly.CubicTo(p2.Vec(Point(0, -1.4)), p2.Vec(Point(0.8, -1.4)),
            p2.Vec(Point(1.5, 0.5)));
        cv.Push(poly);
    }

    // fontcanvas.js/FontCanvas/drawUpperRightCorner
    void DrawUpperRightCorner(Canva& cv, Point p1, double kMinWidthT,
        double kagekMinWidthY, double kagekWidth) {
        PointMaker p(p1);
        Contour    poly;
        poly.MoveTo(p.Vec(Point(-kMinWidthT, -kagekMinWidthY)));
        poly.LineTo(p.Vec(Point(0, -kagekMinWidthY - kagekWidth)));
        poly.LineTo(p.Vec(Point(kMinWidthT + kagekWidth, kagekMinWidthY)));
        poly.CubicTo(p.Vec(Point(kMinWidthT, kMinWidthT * 0.8)),
            p.Vec(Point(0, kMinWidthT * 1.2)),
            p.Vec(Point(-kMinWidthT * 0.9, kMinWidthT * 1.2)));
        cv.Push(poly);
    }

    // fontcanvas.js/FontCanvas/drawTurnLeft
    void DrawTurnLeft(
        Canva& cv, Point p2, double kMinWidthT, double length_param) {
        Contour poly;
        poly.MoveTo(Point(p2.x, p2.y - kMinWidthT));
        poly.LineTo(Point(p2.x - length_param, p2.y - kMinWidthT));
        poly.LineTo(Point(p2.x - length_param, p2.y - kMinWidthT * 0.5));
        poly.CubicTo(Point(p2.x - kMinWidthT * 0.3, p2.y - kMinWidthT * 0.3),
            Point(p2.x, p2.y + kMinWidthT * 0.3),
            Point(p2.x, p2.y + kMinWidthT));
        poly.LineTo(Point(p2.x + 0.1, p2.y));
        poly.Reverse();
        cv.Push(poly);
    }

    // fontcanvas.js/FontCanvas/drawNewGTHbox
    // for new GTH box's left bottom corner MUKI KANKEINASHI
    void DrawNewGTHbox(
        Canva& cv, Point p2, double kMinWidthT, double kagekMinWidthY) {
        Contour poly;
        poly.MoveTo(Point(p2.x, p2.y - kagekMinWidthY * 5));
        poly.LineTo(Point(p2.x - kMinWidthT * 2, p2.y));
        poly.LineTo(Point(p2.x - kagekMinWidthY, p2.y + kagekMinWidthY * 5));
        poly.LineTo(Point(p2.x + kMinWidthT, p2.y + kagekMinWidthY));
        poly.LineTo(Point(p2.x, p2.y));
        poly.Reverse();
        cv.Push(poly);
    }

    // fontcanvas.js/FontCanvas/drawNewGTHbox_v
    void DrawNewGTHboxV(
        Canva& cv, Point p2, double kMinWidthT, double kagekMinWidthY) {
        Contour poly;
        poly.MoveTo(Point(p2.x - kMinWidthT, p2.y - kagekMinWidthY * 3));
        poly.LineTo(Point(p2.x - kMinWidthT * 2, p2.y));
        poly.LineTo(Point(p2.x - kagekMinWidthY, p2.y + kagekMinWidthY * 5));
        poly.LineTo(Point(p2.x + kMinWidthT, p2.y + kagekMinWidthY));
        poly.Reverse();
        cv.Push(poly);
    }

    // fontcanvas.js/FontCanvas/drawLowerRightHT_v
    // for T design
    void DrawLowerRightHTv(
        Canva& cv, Point p2, double kMinWidthT, double kagekMinWidthY) {
        Contour poly;
        poly.MoveTo(Point(p2.x, p2.y + kagekMinWidthY));
        poly.LineTo(Point(p2.x + kMinWidthT, p2.y - kagekMinWidthY * 3));
        poly.LineTo(Point(p2.x + kMinWidthT * 2, p2.y - kagekMinWidthY));
        poly.LineTo(Point(p2.x + kMinWidthT * 2, p2.y + kagekMinWidthY));
        cv.Push(poly);
    }

    // fontcanvas.js/FontCanvas/drawLowerRightHT
    // for T design
    void DrawLowerRightHT(
        Canva& cv, Point p2, double kMinWidthT, double kagekMinWidthY) {
        Contour poly;
        poly.MoveTo(Point(p2.x, p2.y + kagekMinWidthY));
        poly.LineTo(Point(p2.x + kMinWidthT * 0.5, p2.y - kagekMinWidthY * 4));
        poly.LineTo(Point(p2.x + kMinWidthT * 2, p2.y - kagekMinWidthY));
        poly.LineTo(Point(p2.x + kMinWidthT * 2, p2.y + kagekMinWidthY));
        cv.Push(poly);
    }

    // fontcanvas.js/FontCanvas/drawOpenBegin_curve_down2
    void DrawOpenBeginCurveDown2(Canva& cv, Point p1, double rad,
        double kMinWidthT, double rad_offset, double kagekMinWidthY) {
        Contour    poly;
        const auto dir = Rad2Dir(rad);
        PointMaker pm1(p1, dir);
        PointMaker pm0(
            pm1.Vec(Point(-kagekMinWidthY * 0.79, 0)), dir, kMinWidthT);
        Point p0 = pm1.Vec(Point(0, kMinWidthT));
        poly.MoveTo(p0);

        PointMaker p2(UNINIT_POINT);
        p2.SetScale(kMinWidthT * 0.876);
        const auto offset_limit = std::atan2(kagekMinWidthY * 0.79, kMinWidthT);
        rad_offset              = std::min(rad_offset, offset_limit);
        p2.SetDir(Rad2Dir(rad + rad_offset));

        if(rad_offset < -offset_limit) {
            PointMaker p3(p0, dir);
            p2.SetPos(p3.Vec(
                Point(kMinWidthT * 2 * std::sin(rad_offset), -kMinWidthT * 2)));
        } else {
            poly.LineTo(pm0.Vec(Point(-std::sin(rad_offset), 1)));
            p2.SetPos(pm0.Vec(Point(std::sin(rad_offset), -1)));
        }
        poly.LineTo(p2.Vec(Point(0, 0)));
        poly.CubicTo(p2.Vec(Point(0, -1.0)), p2.Vec(Point(0.6, -1.0)),
            p2.Vec(Point(1.8, 1.0)));
        cv.Push(poly);
    }

    // fontcanvas.js/FontCanvas/drawOpenBegin_curve_down
    void DrawOpenBeginCurveDown(Canva& cv, Point p1, double rad,
        double kMinWidthT, double kagekMinWidthY) {
        auto rad_offset = (PI * 0.621 - rad) * 0.5;
        DrawOpenBeginCurveDown2(
            cv, p1, rad, kMinWidthT, rad_offset, kagekMinWidthY);
    }

    // fontcanvas.js/FontCanvas/drawOpenBegin_curve_up
    void DrawOpenBeginCurveUp(Canva& cv, Point p1, Dir dir, double kMinWidthT,
        double kagekMinWidthY) {
        Contour    poly;
        PointMaker pm1(p1, dir);
        const auto offs_sin = std::sin(-0.1 * PI),
                   offs_cos = std::cos(-0.1 * PI);
        poly.MoveTo(pm1.Vec(Point(0, -kMinWidthT)));
        poly.LineTo(pm1.Vec(
            Point((offs_sin / offs_cos) * 2 * kMinWidthT, -kMinWidthT)));

        PointMaker p2(UNINIT_POINT);
        p2.SetScale(kMinWidthT * 0.876);
        p2.SetDir({
            sin: dir.sin * offs_cos + dir.cos * offs_sin,
            cos: dir.cos * offs_cos - dir.sin * offs_sin
        });
        p2.SetPos(pm1.Vec(Point(0, kMinWidthT)));

        poly.LineTo(p2.Vec(Point(0, 0)));
        poly.CubicTo(p2.Vec(Point(0, 1.1)), p2.Vec(Point(0.7, 1.1)),
            p2.Vec(Point(1.4, -0.5)));
        poly.Reverse();
        cv.Push(poly);
    }

    // fontcanvas.js/FontCanvas/drawUpperRightCorner2
    void DrawUpperRightCorner2(Canva& cv, Point p1, double kMinWidthT,
        double kagekMinWidthY, double kagekWidth, bool is_roofed_thin) {
        PointMaker p(p1);
        Contour    poly;
        poly.MoveTo(p.Vec(Point(-kMinWidthT, -kagekMinWidthY)));
        poly.LineTo(p.Vec(Point(0, -kagekMinWidthY - kagekWidth)));
        poly.LineTo(p.Vec(Point(kMinWidthT + kagekWidth, kagekMinWidthY)));
        if(is_roofed_thin) {
            poly.LineTo(p.Vec(Point(kMinWidthT * 0.3, kMinWidthT * 1.15)));
            poly.LineTo(p.Vec(Point(0, kagekMinWidthY)));
        } else
            poly.CubicTo(p.Vec(Point(kMinWidthT, kMinWidthT * 0.8)),
                p.Vec(Point(0, kMinWidthT * 1.2)),
                p.Vec(Point(-kMinWidthT * 0.9, kMinWidthT * 1.2)));
        cv.Push(poly);
    }

    // fontcanvas.js/FontCanvas/drawUpperLeftCorner
    void DrawUpperLeftCorner(Canva& cv, Point p1, Dir dir, double kMinWidthT) {
        PointMaker p(p1, dir, kMinWidthT);
        Contour    poly;
        poly.MoveTo(p.Vec(Point(0, 1)));
        poly.LineTo(p.Vec(Point(0.01, 0))); // fix_union
        poly.LineTo(p.Vec(Point(0, -1)));
        poly.LineTo(p.Vec(Point(-1, 1)));
        poly.Reverse();
        cv.Push(poly);
    }

    // fontcanvas.js/FontCanvas/drawTailCircle_tan
    // draw a (semi)circle on the tail of the line to (tailX, tailY)
    void DrawTailCircleTan(
        Canva& cv, Point tail, Dir dir, double r, Point tan1, Point tan2) {
        Contour poly;
        auto    vec1 = tan1.toLength(r * BEZ_CIR * 0.74),
             vec2    = tan2.toLength(r * BEZ_CIR * 0.78);
        PointMaker p(tail, dir, r);
        Point      p1 = p.Vec(Point(0, -1)), p2 = p.Vec(Point(0, 1));
        poly.MoveTo(p1);
        poly.CubicTo(p1 + vec1, p.Vec(Point(0.94, -BEZ_CIR * 1.09)),
            p.Vec(Point(0.94, 0)));
        poly.CubicTo(p.Vec(Point(0.94, BEZ_CIR * 1.09)), p2 + vec2, p2);
        poly.LineTo(p.Vec(Point(-0.01, 0))); // fix_union
        cv.Push(poly);
    }

    // fontcanvas.js/FontCanvas/drawTailCircle
    // draw a (semi)circle on the tail of the line to (tailX, tailY)
    void DrawTailCircle(Canva& cv, Point tail, Dir dir, double r) {
        Contour    poly;
        PointMaker p(tail, dir, r);
        poly.MoveTo(p.Vec(Point(0, -1)));
        poly.CubicTo(p.Vec(Point(BEZ_CIR, -1)), p.Vec(Point(1, -BEZ_CIR)),
            p.Vec(Point(1, 0)));
        poly.CubicTo(p.Vec(Point(1, BEZ_CIR)), p.Vec(Point(BEZ_CIR, 1)),
            p.Vec(Point(0, 1)));
        poly.LineTo(p.Vec(Point(-0.01, 0))); // fix_union
        cv.Push(poly);
    }

    // fontcanvas.js/FontCanvas/drawTurnUpwards_pos
    void DrawTurnUpwardsPos(
        Canva& cv, Point p2, double kMinWidthT, double length_param, Dir dir) {
        Contour    poly;
        PointMaker p(p2, dir);
        poly.MoveTo(p.Vec(Point(kMinWidthT * 0.7, -kMinWidthT * 0.7)));
        poly.CubicTo(p.Vec(Point(kMinWidthT * 0.22, -kMinWidthT)),
            p.Vec(Point(kMinWidthT * 0.167, -kMinWidthT - length_param * 0.05)),
            p.Vec(
                Point(kMinWidthT * 0.181, -kMinWidthT - length_param * 0.15)));
        poly.LineTo(p.Vec(Point(kMinWidthT * 0.3, -kMinWidthT - length_param)));
        poly.LineTo(p.Vec(Point(0, -kMinWidthT - length_param)));
        poly.LineTo(
            p.Vec(Point(-kMinWidthT * 0.75, -kMinWidthT - length_param / 4)));
        poly.CubicTo(
            p.Vec(Point(-kMinWidthT * 0.875, -kMinWidthT - length_param / 8)),
            p.Vec(Point(-kMinWidthT * 1.25, -kMinWidthT)),
            p.Vec(Point(-kMinWidthT * 1.6, -kMinWidthT)));
        poly.Reverse(); // for fill-rule
        cv.Push(poly);
    }

    // fontcanvas.js/FontCanvas/drawL2RSweepEnd
    void DrawL2RSweepEnd(Canva& cv, Point p2, Dir dir, double kMinWidthT,
        double kagekL2RDfatten) {
        auto type =
            (std::atan2(std::abs(dir.sin), std::abs(dir.cos)) / PI * 2 - 0.6);
        type         *= type > 0 ? 8 : 3;
        const auto pm = (type < 0) ? -1 : 1;
        PointMaker p(p2, dir);
        Contour    poly;
        poly.MoveTo(p.Vec(Point(0, kMinWidthT * kagekL2RDfatten)));
        poly.LineTo(p.Vec(Point(-0.1, 0))); // fix_union
        poly.LineTo(p.Vec(Point(0, -kMinWidthT * kagekL2RDfatten)));
        poly.LineTo(p.Vec(Point(kMinWidthT * kagekL2RDfatten * std::abs(type),
            kMinWidthT * kagekL2RDfatten * pm)));
        cv.Push(poly);
    }

    // fontcanvas.js/FontCanvas/drawCircle_bend_pos
    void DrawCircleBendPos(Canva& cv, Point p2, Dir dir, double kMinWidthT2) {
        Contour    poly;
        PointMaker p(p2, dir, kMinWidthT2);
        poly.MoveTo(p.Vec(Point(0, -1)));
        poly.CubicTo(
            p.Vec(Point(1.5, -1)), p.Vec(Point(0.9, 1)), p.Vec(Point(0, 1)));
        poly.LineTo(p.Vec(Point(-0.01, 0))); // fix_union
        cv.Push(poly);
    }

    // fontcanvas.js/FontCanvas/drawCircle_bend_neg
    void DrawCircleBendNeg(Canva& cv, Point p2, Dir dir, double kMinWidthT2) {
        Contour    poly;
        PointMaker p(p2, dir, kMinWidthT2);
        poly.MoveTo(p.Vec(Point(0, 1)));
        poly.CubicTo(
            p.Vec(Point(1.5, 1)), p.Vec(Point(0.9, -1)), p.Vec(Point(0, -1)));
        poly.LineTo(p.Vec(Point(-0.01, 0))); // fix_union
        cv.Push(poly);
    }

    // fontcanvas.js/FontCanvas/drawTurnUpwards_neg
    void DrawTurnUpwardsNeg(
        Canva& cv, Point p2, double kMinWidthT, double length_param, Dir dir) {
        Contour    poly;
        PointMaker p(p2, dir);
        poly.MoveTo(p.Vec(Point(kMinWidthT * 0.7, kMinWidthT * 0.7)));
        poly.CubicTo(p.Vec(Point(0, kMinWidthT)),
            p.Vec(Point(kMinWidthT * 0.3, kMinWidthT + length_param / 2)),
            p.Vec(Point(kMinWidthT * 0.5, kMinWidthT + length_param)));
        poly.LineTo(p.Vec(Point(0, kMinWidthT + length_param)));
        poly.CubicTo(
            p.Vec(Point(-kMinWidthT * 0.6, kMinWidthT + length_param / 4)),
            p.Vec(Point(-kMinWidthT * 1.8, kMinWidthT)),
            p.Vec(Point(-kMinWidthT * 2.2, kMinWidthT)));
        cv.Push(poly);
    }

    // mincho.js/Mincho/adjustUrokoParam
    double Mincho::AdjustUrokoParam(Stroke stroke, std::vector<Stroke> others) {
        // for illegal strokes
        if(stroke.endOpt != 0) return stroke.endOpt % 10;
        if(stroke.typeOpt != 0) return 0;
        // STROKETYPE.STRAIGHT && ENDTYPE.OPEN
        for(size_t k = 0; k < kAdjustUrokoLengthStep; k++) {
            double tx, ty, tlen;
            if(stroke.p1.y == stroke.p2.y) // YOKO
                tx = stroke.p2.x - kAdjustUrokoLine[k], ty = stroke.p2.y - 0.5,
                tlen = stroke.p2.x - stroke.p1.x;
            else {
                double rad = std::atan(
                    (stroke.p2.y - stroke.p1.y) / (stroke.p2.x - stroke.p1.x));
                tx = stroke.p2.x - kAdjustUrokoLine[k] * std::cos(rad) -
                    0.5 * std::sin(rad),
                ty = stroke.p2.y - kAdjustUrokoLine[k] * std::sin(rad) -
                    0.5 * std::cos(rad),
                tlen = (stroke.p2 - stroke.p1).GetLength();
            }
            if(tlen < kAdjustUrokoLength[k] ||
                isCrossWithOthers(others, -1, Point(tx, ty), stroke.p2))
                return kAdjustUrokoLengthStep - k;
        }
        return 0; // a3 += res * 100;
    }

    // mincho.js/Mincho/adjustUroko2Param
    double Mincho::AdjustUroko2Param(
        Stroke stroke, std::vector<Stroke> others) {
        // for illegal strokes
        if(stroke.endOpt != 0) return stroke.endOpt % 10;
        if(stroke.typeOpt != 0) return 0;
        // STROKETYPE.STRAIGHT && ENDTYPE.OPEN && y1==y2
        std::vector<double> pressures = {};
        for(auto other: others) {
            if((other.type == STROKE_STRAIGHT && other.typeOpt == 0 &&
                   other.p1.y == other.p2.y &&
                   !(stroke.p1.x + 1 > other.p2.x ||
                       stroke.p2.x - 1 < other.p1.x) &&
                   std::abs(stroke.p1.y - other.p1.y) < kAdjustUroko2Length) ||
                (other.type == STROKE_BENDING && other.typeOpt == 0 &&
                    other.p2.y == other.p3.y &&
                    !(stroke.p1.x + 1 > other.p3.x ||
                        stroke.p2.x - 1 < other.p2.x) &&
                    std::abs(stroke.p1.y - other.p2.y) < kAdjustUroko2Length))
                pressures.push_back(std::pow(
                    kAdjustUroko2Length - std::abs(stroke.p1.y - other.p2.y),
                    1.1));
        }
        double pressure = 0;
        for(auto i: pressures)
            pressure = Point(pressure, i).GetLength();
        pressure *= 1.9; // 1.7を取ってnorm2ではなく+にすると以前と同じ
        return std::min(std::floor(pressure / kAdjustUroko2Length),
            kAdjustUroko2Step); // a3 += res * 100;
    }

    // mincho.js/Mincho/adjustTateParam
    double Mincho::AdjustTateParam(Stroke stroke, std::vector<Stroke> others) {
        // for illegal strokes
        if(stroke.startOpt >= 10) return (stroke.startOpt % 100) / 10;
        if(stroke.typeOpt != 0) return 0;
        //(STROKETYPE.STRAIGHT || STROKETYPE.BENDING || STROKETYPE.VCURVE)
        if(stroke.p1.x != stroke.p2.x) return 0;
        std::vector<double> res_arr = {};
        for(auto other: others) {
            if((other.type == STROKE_STRAIGHT || other.type == STROKE_BENDING ||
                   other.type == STROKE_VCURVE) &&
                other.typeOpt == 0 && other.p1.x == other.p2.x &&
                !(stroke.p1.y + 1 > other.p2.y ||
                    stroke.p2.y - 1 < other.p1.y) &&
                std::abs(stroke.p1.x - other.p1.x) <
                    kMinWidthT_adjust * kAdjustTateStep)
                res_arr.push_back(kAdjustTateStep -
                    std::floor(std::abs(stroke.p1.x - other.p1.x) /
                        kMinWidthT_adjust));
        }
        const double kAdjustTateStep_org = 4; // original implementation
        double       res                 = 0;
        for(auto i: res_arr)
            res = Point(res, i).GetLength();
        res *= 1.1; // 1.1を取ってnorm2ではなく+にすると以前と同じ
        return std::min(res, kAdjustTateStep_org); // a2 += res * 1000
    }

    // mincho.js/Mincho/adjustHaneParam
    // adjust "Hane" (short line turning to the left)
    double Mincho::AdjustHaneParam(
        Stroke stroke, Point ep, std::vector<Stroke> others) {
        // for illegal strokes
        if(stroke.endOpt != 0) return stroke.endOpt % 10;
        if(stroke.typeOpt != 0) return 0;
        // endPointX, endPointY
        //(STROKETYPE.STRAIGHT || STROKETYPE.CURVE || STROKETYPE.BEZIER)
        //&&ENDTYPE.TURN_LEFT
        double res     = 0,
               nearest = INFINITY; // the nearest point to the short line
        if(ep.x + 18 < 100) nearest = ep.x + 18;
        for(auto other: others) {
            if(other.type == STROKE_STRAIGHT && other.typeOpt == 0 &&
                other.p1.x == other.p2.x && other.p1.x < ep.x &&
                other.p1.y <= ep.y && other.p2.y >= ep.y) {
                if(ep.x - other.p1.x < 100)
                    nearest = std::min(nearest, ep.x - other.p1.x);
            }
        }
        if(nearest != INFINITY) res = 7 - std::floor(nearest / 15);
        return res; // a3 += res * 100;
    }

    // mincho.js/Mincho/adjustKakatoParam
    double Mincho::AdjustKakatoParam(
        Stroke stroke, std::vector<Stroke> others) {
        // for illegal strokes
        if(stroke.endOpt != 0) return stroke.endOpt % 10;
        if(stroke.typeOpt != 0) return 0;
        // if (STROKETYPE.STRAIGHT && (LOWER_LEFT_CORNER || LOWER_RIGHT_CORNER))
        for(size_t k = 0; k < kAdjustKakatoStep; k++) {
            if(isCrossBoxWithOthers(others, -1,
                   Point(stroke.p2.x - kAdjustKakatoRangeX / 2,
                       stroke.p2.y + kAdjustKakatoRangeY[k]),
                   Point(stroke.p2.x + kAdjustKakatoRangeX / 2,
                       stroke.p2.y + kAdjustKakatoRangeY[k + 1])) |
                stroke.p2.y + kAdjustKakatoRangeY[k + 1] >
                    200 // adjust for baseline
                | stroke.p2.y - stroke.p1.y <
                    kAdjustKakatoRangeY[k + 1] // for thin box
            )
                return kAdjustKakatoStep - k;
        }
        return 0;
    }

    // mincho.js/Mincho/adjustKakatoParam
    // connecting to other strokes.
    bool Mincho::AdjustKirikuchiParam(
        Stroke stroke, std::vector<Stroke> others) {
        // for illegal strokes
        if(stroke.startOpt % 10 == 1) return true;
        if(stroke.typeOpt != 0) return false;
        // STROKETYPE.CURVE, STARTTYPE.CONNECTING_V
        if(stroke.p1.x > stroke.p2.x && stroke.p1.y < stroke.p2.y) {
            for(auto other: others) {
                if(other.type == STROKE_STRAIGHT && other.typeOpt == 0 &&
                    other.p1.x < stroke.p1.x && other.p2.x > stroke.p1.x &&
                    other.p1.y == stroke.p1.y && other.p1.y == other.p2.y)
                    return true;
            }
        }
        return false;
        // if (res) a2 += 100;
    }

    // mincho.js/Mincho/adjustMageParam
    double Mincho::AdjustMageParam(Stroke stroke, std::vector<Stroke> others) {
        // for illegal strokes
        if(stroke.startOpt % 10 != 0) return (stroke.startOpt % 100) / 10;
        if(stroke.typeOpt != 0) return 0;
        // STROKETYPE.BENDING
        // applied only if y2=y3
        if(stroke.p2.y != stroke.p3.y) return 0;
        std::vector<double> res0_above = {}, res0_below = {};
        for(auto other: others) {
            if((other.type == STROKE_STRAIGHT && other.typeOpt == 0 &&
                   other.p1.y == other.p2.y &&
                   !(stroke.p2.x + 1 > other.p2.x ||
                       stroke.p3.x - 1 < other.p1.x) &&
                   std::abs(stroke.p2.y - other.p1.y) <
                       kMinWidthT_adjust * kAdjustMageStep) ||
                (other.type == STROKE_BENDING && other.typeOpt == 0 &&
                    other.p2.y == other.p3.y &&
                    !(stroke.p2.x + 1 > other.p3.x ||
                        stroke.p3.x - 1 < other.p2.x) &&
                    std::abs(stroke.p2.y - other.p2.y) <
                        kMinWidthT_adjust * kAdjustMageStep)) {
                auto p = kAdjustMageStep -
                    std::floor(
                        std::abs(stroke.p2.y - other.p2.y) / kMinWidthT_adjust);
                if((other.type == STROKE_STRAIGHT && other.typeOpt == 0 &&
                       stroke.p2.y < other.p1.y) ||
                    (other.type == STROKE_BENDING && other.typeOpt == 0 &&
                        stroke.p2.y < other.p2.y)) // lines "above" the hane
                    res0_above.push_back(p);
                else
                    res0_below.push_back(p);
            }
        }
        double res_above = 0, res_below = 0;
        for(auto i: res0_above)
            res_above = std::max(res_above, i);
        for(auto i: res0_below)
            res_below = std::max(res_below, i);
        res_below *= 1.3;
        auto res   = std::max(res_above,
              res_below); // 1.3とかを外して上2行を含めたmaxとかnorm2を+にすると以前と同じ

        const auto maxlen = (stroke.p2.y - stroke.p1.y) * 0.6; // y2-y1から算出
        const auto res2   = maxlen <= 0 ? 0
                                        : (1 - (maxlen / kWidth - 1) / 4) *
                kAdjustMageStep; //"this.kWidth * (4 * (1 - param_mage /
                                   // this.kAdjustMageStep) + 1)" を参考に逆算
        res = std::max(res,
            res2); // 小数値が返るため、問題が出る可能性もある？今のところ問題なし
        res = std::min(res, kAdjustMageStep);
        return res; // a3 += res * 1000;
    }

    // mincho.js/Mincho/getStartOfOffsetLine
    // get polygon data for the start of line (with offset)
    Contour GetStartOfOffsetLine(Point p1, Dir dir, double halfWidth,
        double off_right1, double off_left1) {
        Contour poly(2);
        poly.Set(1,
            Point(p1.x + dir.sin * halfWidth - dir.cos * off_left1,
                p1.y - dir.cos * halfWidth - dir.sin * off_left1));
        poly.Set(0,
            Point(p1.x - dir.sin * halfWidth - dir.cos * off_right1,
                p1.y + dir.cos * halfWidth - dir.sin * off_right1));
        return poly;
    }

    // mincho.js/Mincho/getStartOfLine
    // get polygon data for the start of line
    Contour GetStartOfLine(Point p1, Dir dir, double halfWidth) {
        return GetStartOfOffsetLine(p1, dir, halfWidth, 0, 0);
    }

    // mincho.js/Mincho/getStartOfVLine
    Contour Mincho::GetStartOfVLine(
        Point p1, Point p2, StartType a1, double kMinWidthT, Canva& cv) {
        const auto rad = (p2 - p1).GetRad();
        const auto dir = (p2 - p1).GetDir();
        Contour    poly_start(2);
        if(dir.cos == 0) { // 竖直
            double left1, right1;
            switch(a1) {
            case START_OPEN:
                right1 = -kMinWidthT * 0.5, left1 = -kMinWidthT * 0.7;
                break;
            case START_UPPER_LEFT_CORNER:
                right1 = kMinWidthY + kMinWidthT, left1 = kMinWidthY;
                break;
            case START_CONNECTING_V:
                right1 = kMinWidthY - 0.001, left1 = kMinWidthY - 0.001;
                break;
            default: right1 = 0, left1 = 0; break;
            }
            poly_start =
                GetStartOfOffsetLine(p1, dir, kMinWidthT, right1, left1);
            if(a1 == START_UPPER_RIGHT_CORNER)
                DrawUpperRightCornerStraightV(
                    cv, p1, kMinWidthT, kMinWidthYY, kWidth);
            if(a1 == START_OPEN)
                DrawOpenBeginStraight(cv, p1, kMinWidthT, kMinWidthYY, rad);
        } else {
            const auto v = 1; // previously (x1 > x2) ? -1 : 1;
            if(a1 == START_UPPER_RIGHT_CORNER) {
                if(dir.sin == 0) // error
                    poly_start = GetStartOfLine(p1, dir, kMinWidthT);
                else {
                    // poly_start.set(1, x1 + (kMinWidthT * v + 1) /
                    // Math.sin(rad), y1 + 1);//" + 1" ??
                    poly_start.Set(1,
                        Point(p1.x + (kMinWidthT * v) / std::sin(rad), p1.y));
                    poly_start.Set(0,
                        Point(p1.x - (kMinWidthT * v) / std::sin(rad), p1.y));
                }
            } else if(a1 == START_CONNECTING_V) {
                if(dir.sin == 0) // error
                    poly_start = GetStartOfLine(p1, dir, kMinWidthT);
                else {
                    poly_start.Set(1,
                        Point(p1.x + (kMinWidthT * v) / std::sin(rad), p1.y));
                    poly_start.Set(0,
                        Point(p1.x - (kMinWidthT * v) / std::sin(rad), p1.y));
                }
            } else {
                double left1, right1;
                switch(a1) {
                case START_OPEN:
                    right1 = -kMinWidthT * 0.5, left1 = -kMinWidthT * 0.7;
                    break;
                case START_UPPER_LEFT_CORNER:
                    right1 = kMinWidthY + kMinWidthT, left1 = kMinWidthY;
                    break;
                default: right1 = 0, left1 = 0; break;
                }
                poly_start =
                    GetStartOfOffsetLine(p1, dir, kMinWidthT, right1, left1);
            }
            if(a1 == START_UPPER_RIGHT_CORNER) // SHIKAKU MIGIUE UROKO NANAME
                                               // DEMO MASSUGU MUKI
                DrawUpperRightCorner(cv, p1, kMinWidthT, kMinWidthYY, kWidth);
            if(a1 == START_OPEN) // beginning of the stroke
                DrawOpenBeginStraight(cv, p1, kMinWidthT, kMinWidthYY, rad);
        }
        return poly_start;
    }

    // mincho.js/Mincho/getEndOfOffsetLine
    // get polygon data for the end of line (with offset)
    Contour GetEndOfOffsetLine(Point p1, Point p2, double halfWidth,
        double off_right2, double off_left2) {
        const auto dir = (p2 - p1).GetDir();
        Contour    poly(2);
        poly.Set(0,
            Point(p2.x + dir.sin * halfWidth + dir.cos * off_left2,
                p2.y - dir.cos * halfWidth + dir.sin * off_left2));
        poly.Set(1,
            Point(p2.x - dir.sin * halfWidth + dir.cos * off_right2,
                p2.y + dir.cos * halfWidth + dir.sin * off_right2));
        return poly;
    }

    // mincho.js/Mincho/getEndOfLine
    // get polygon data for the end of line
    Contour GetEndOfLine(Point p1, Point p2, double halfWidth) {
        return GetEndOfOffsetLine(p1, p2, halfWidth, 0, 0);
    }

    // mincho.js/Mincho/minchoDrawCurve
    void Mincho::DrawCurve(Canva& cv, Point p1pre, Point ps, Point p2pre,
        StartType a1, bool a1adjusted, EndType a2, double kMinWidthT_mod,
        double end_width_factor) {
        double delta = 0;
        if(!a1adjusted) {
            switch(a1) {
            case START_OPEN:
            case START_THIN:
            case START_ROOFED_THIN:       delta = -1 * kMinWidthY * 0.5; break;
            case START_UPPER_LEFT_CORNER: delta = kMinWidthY; break;
            default:                      delta = 0; break;
            }
        }
        auto p1 = ps.GetExtendedDest(p1pre, delta);
        switch(a2) {
        case END_STOP: // get shorten for tail's circle
            delta = -1 * kMinWidthT_mod * 0.52;
            break;
        case END_TURN_LEFT: delta = -kMage * 0.439; break;
        default:            delta = 0; break;
        }
        auto p2 = ps.GetExtendedDest(p2pre, delta);

        double cornerOffset = 0;
        if(!a1adjusted &&
            (a1 == START_UPPER_RIGHT_CORNER || a1 == START_ROOFED_THIN) &&
            a2 == END_LEFT_SWEEP) {
            auto ps1 = ps, ps2 = ps; // 元の実装と名前を揃える
            auto contourLength = (ps1 - p1).GetLength() +
                (ps2 - ps1).GetLength() + (p2 - ps2).GetLength();
            if(contourLength < 100) {
                cornerOffset = (kMinWidthT_mod > 6)
                    ? (kMinWidthT_mod - 6) * ((100 - contourLength) / 100)
                    : 0;
                p1.x        += cornerOffset;
            }
        }

        std::function<double(double)> width_func, width_func_d;
        std::vector<CubicSpline>      bez1, bez2;
        double                        thin_stop_param;
        if(!a1adjusted && (a1 == START_THIN || a1 == START_ROOFED_THIN) &&
            a2 == END_STOP) { // stop
            // const slant_cos =
            const auto len  = (p2 - p1).GetLength();
            thin_stop_param = (1 + (len - 100) * 0.0007);
            width_func      = [p1, p2, kMinWidthT_mod, thin_stop_param](
                             double t) -> double {
                return WidFuncStop(t, p1, p2, kMinWidthT_mod) * thin_stop_param;
            },
            width_func_d = [p1, p2, kMinWidthT_mod, thin_stop_param](
                               double t) -> double {
                return WidFuncStopD(t, p1, p2, kMinWidthT_mod) *
                    thin_stop_param;
            };
            qBezier2(p1, ps, p2, width_func, width_func_d, bez1, bez2);
        } else {
            if(!a1adjusted && (a1 == START_THIN || a1 == START_ROOFED_THIN) &&
                a2 == END_OPEN) // L2RD: fatten
                width_func = [p1, p2, kMinWidthT_mod, this](
                                 double t) -> double {
                    return WidFunc(t, p1, p2, kMinWidthT_mod) * kL2RDfatten;
                },
                width_func_d = [p1, p2, kMinWidthT_mod, this](
                                   double t) -> double {
                    return WidFuncD(t, p1, p2, kMinWidthT_mod) * kL2RDfatten;
                };
            else if(!a1adjusted && a1 == START_CONNECTING_V &&
                a2 == END_LOWER_LEFT_CORNER) // 未使用。さんずい用 (実験)
                width_func = [kMinWidthT_mod](double t) -> double {
                    return ((1 - t) * 0.628 + std::pow((1 - t), 30) * 0.600 +
                               0.222) *
                        kMinWidthT_mod;
                }, width_func_d = [kMinWidthT_mod](double t) -> double {
                    return (-0.628 - 30 * std::pow((1 - t), 29) * 0.600) *
                        kMinWidthT_mod;
                };
            // don't feel like 'export'ing CURVE_THIN for this experimental
            // change...
            else if(!a1adjusted && a1 == START_THIN ||
                a1 == START_ROOFED_THIN || a1 == START_CONNECT_THIN)
                width_func = [p1, p2, kMinWidthT_mod](double t) -> double {
                    return WidFunc(t, p1, p2, kMinWidthT_mod);
                }, width_func_d = [p1, p2, kMinWidthT_mod](double t) -> double {
                    return WidFuncD(t, p1, p2, kMinWidthT_mod);
                };
            else if(a2 == END_LEFT_SWEEP)
                width_func = [p1, p2, kMinWidthT_mod](double t) -> double {
                    return WidFunc(1 - t, p1, p2, kMinWidthT_mod);
                }, width_func_d = [p1, p2, kMinWidthT_mod](double t) -> double {
                    return -WidFuncD(1 - t, p1, p2, kMinWidthT_mod);
                };
            else {
                if(a2 == END_TURN_LEFT) end_width_factor = 0;
                width_func = [kMinWidthT_mod, end_width_factor](
                                 double t) -> double {
                    return kMinWidthT_mod * (1 - t * end_width_factor);
                },
                width_func_d = [kMinWidthT_mod, end_width_factor](
                                   double t) -> double {
                    return -kMinWidthT_mod * end_width_factor;
                };
            }
            qBezier(p1, ps, p2, width_func, width_func_d, bez1, bez2);
        }
        if(a1adjusted && a1 == START_CONNECTING_V && p1.x != ps.x) {
            auto b1 = Bezier2y(bez2[bez2.size() - 1], p1.y);
            if(b1.size() != 0) bez2[bez2.size() - 1] = b1;
            CubicSpline temp;
            for(size_t i = 0; i < bez1[0].size(); i++)
                temp[bez1[0].size() - 1 - i] = bez1[0][i];
            auto b2 = Bezier2y(temp, p1.y);
            if(b2.size() != 0) {
                for(size_t i = 0; i < b2.size(); i++)
                    temp[bez1[0].size() - 1 - i] = b2[i];
            }
            bez1[0] = temp;
        } else if(!a1adjusted && 40 <= a1 && a1 <= 80) {
            auto r = Point((p2pre.x - p1pre.x) * std::pow(1.4, (a1 % 10) - 4.5),
                p2pre.y - p1pre.y)
                         .GetRad();
            if(a1 >= 50) r = -r;
            if(a1 == 60) r = PI * 0.5; // vertical edge
            auto b1 = Bezier2Line(bez2[bez2.size() - 1], p1, r);
            if(b1.size() != 0) bez2[bez2.size() - 1] = b1;
            CubicSpline temp;
            for(size_t i = 0; i < bez1[0].size(); i++)
                temp[bez1[0].size() - 1 - i] = bez1[0][i];
            auto b2 = Bezier2Line(temp, p1, r);
            if(b2.size() != 0) {
                for(size_t i = 0; i < b2.size(); i++)
                    temp[bez1[0].size() - 1 - i] = b2[i];
            }
        } else if(!a1adjusted && a1 == START_UPPER_RIGHT_CORNER &&
            p1.x != ps.x) {
            auto b1 = Bezier2y(bez2[bez2.size() - 1], p1.y);
            if(b1.size() != 0) bez2[bez2.size() - 1] = b1;
        }
        auto poly = CubicSpline2Contour(bez1);
        poly.Concat(CubicSpline2Contour(bez2));
        if(a1 == START_UPPER_RIGHT_CORNER) poly.LineTo(p1);
        cv.Push(poly);
        if(a2 == END_STOP) {
            if(a1 == START_THIN || a1 == START_ROOFED_THIN) {
                auto bez1e  = bez1[bez1.size() - 1][3],
                     bez1c2 = bez1[bez1.size() - 1][2], bez2s = bez2[0][0],
                     bez2c1     = bez2[0][1];
                const auto tan1 = bez1e - bez1c2, tan2 = bez2s - bez2c1,
                           cent = (p1 + 4 * ps + p2) / 6;
                auto rad_end    = (p2 - cent).GetDir();
                DrawTailCircleTan(cv, p2, rad_end,
                    kMinWidthT_mod * 1.1 * thin_stop_param, tan1, tan2);
            } else { // CONNECT_THIN or others
                auto enddir = (p2 - ps).GetDir();
                DrawTailCircle(
                    cv, p2, enddir, kMinWidthT_mod * (1 - end_width_factor));
            }
        }
        if(a1 == START_CONNECT_THIN) {
            auto dir_to_start = (p1 - ps).GetDir();
            DrawTailCircle(cv, p1, dir_to_start, kMinWidthT_mod * CURVE_THIN);
        }
    }

    // mincho.js/Mincho/minchoDrawBezier
    PointArray2 Mincho::DrawBezier(Canva& cv, Point p1pre, Point ps1, Point ps2,
        Point p2pre, StartType a1, bool a1adjusted, EndType a2,
        double kMinWidthT_mod) {
        double delta = 0;
        if(!a1adjusted) {
            switch(a1) {
            case START_OPEN:
            case START_THIN:              delta = -1 * kMinWidthY * 0.5; break;
            case START_UPPER_LEFT_CORNER: delta = kMinWidthY; break;
            default:                      delta = 0; break;
            }
        }
        auto p1 = ps1.GetExtendedDest(p1pre, delta);
        switch(a2) {
        case END_STOP: // get shorten for tail's circle
            delta = -1 * kMinWidthT_mod * 0.52;
            break;
        case END_TURN_LEFT: delta = -kMage * 0.439; break;
        default:            delta = 0; break;
        }
        auto p2 = ps2.GetExtendedDest(p2pre, delta);

        double cornerOffset = 0;
        if(!a1adjusted &&
            (a1 == START_UPPER_RIGHT_CORNER || a1 == START_ROOFED_THIN) &&
            a2 == END_LEFT_SWEEP) {
            auto contourLength = (ps1 - p1).GetLength() +
                (ps2 - ps1).GetLength() + (p2 - ps2).GetLength();
            if(contourLength < 100) {
                cornerOffset = (kMinWidthT_mod > 6)
                    ? (kMinWidthT_mod - 6) * ((100 - contourLength) / 100)
                    : 0;
                p1.x        += cornerOffset;
            }
        }
        std::function<double(double)> width_func, width_func_d;
        std::vector<CubicSpline>      bez1, bez2;
        double                        thin_stop_param;
        if(!a1adjusted && (a1 == START_THIN || a1 == START_ROOFED_THIN) &&
            a2 == END_STOP) { // stop
            width_func = [p1, p2, kMinWidthT_mod](double t) -> double {
                return WidFuncStop(t, p1, p2, kMinWidthT_mod);
            }, width_func_d = [p1, p2, kMinWidthT_mod](double t) -> double {
                return WidFuncStopD(t, p1, p2, kMinWidthT_mod);
            };
            cBezier(p1, ps1, ps2, p2, width_func, width_func_d, bez1, bez2);
        } else {
            if(!a1adjusted && (a1 == START_THIN || a1 == START_ROOFED_THIN) &&
                a2 == END_OPEN) // L2RD: fatten
                width_func = [p1, p2, kMinWidthT_mod, this](
                                 double t) -> double {
                    return WidFunc(t, p1, p2, kMinWidthT_mod) * kL2RDfatten;
                },
                width_func_d = [p1, p2, kMinWidthT_mod, this](
                                   double t) -> double {
                    return WidFuncD(t, p1, p2, kMinWidthT_mod) * kL2RDfatten;
                };
            else if(!a1adjusted &&
                (a1 == START_THIN || a1 == START_ROOFED_THIN ||
                    a1 == START_CONNECT_THIN))
                width_func = [p1, p2, kMinWidthT_mod](double t) -> double {
                    return WidFuncFat(t, p1, p2, kMinWidthT_mod);
                }, width_func_d = [p1, p2, kMinWidthT_mod](double t) -> double {
                    return WidFuncFatD(t, p1, p2, kMinWidthT_mod);
                };
            else if(!a1adjusted && a2 == END_LEFT_SWEEP)
                width_func = [p1, p2, kMinWidthT_mod](double t) -> double {
                    return WidFunc(1 - t, p1, p2, kMinWidthT_mod);
                }, width_func_d = [p1, p2, kMinWidthT_mod](double t) -> double {
                    return -WidFuncD(1 - t, p1, p2, kMinWidthT_mod);
                };
            else {
                width_func = [kMinWidthT_mod](double t) -> double {
                    return kMinWidthT_mod;
                }, width_func_d = [](double t) -> double {
                    return 0;
                };
            }
            cBezier(p1, ps1, ps2, p2, width_func, width_func_d, bez1, bez2);
        }
        // 以下は今は実行されないコードだが実行時には2次ベジエのときと同様にdeep
        // copyが必要か？
        if(a1adjusted && a1 == START_CONNECTING_V && p1.x != ps1.x) {
            auto b1 = Bezier2y(bez2[bez2.size() - 1], p1.y);
            if(b1.size() != 0) bez2[bez2.size() - 1] = b1;
            CubicSpline temp;
            for(size_t i = 0; i < bez1[0].size(); i++)
                temp[bez1[0].size() - 1 - i] = bez1[0][i];
            auto b2 = Bezier2y(temp, p1.y);
            if(b2.size() != 0) {
                for(size_t i = 0; i < b2.size(); i++)
                    temp[bez1[0].size() - 1 - i] = b2[i];
            }
            bez1[0] = temp;
        } else if(!a1adjusted && a1 == START_UPPER_RIGHT_CORNER &&
            p1.x > ps1.x) {
            auto b1 = Bezier2y(bez2[bez2.size() - 1], p1.y);
            if(b1.size() != 0) bez2[bez2.size() - 1] = b1;
            CubicSpline temp;
            for(size_t i = 0; i < bez1[0].size(); i++)
                temp[bez1[0].size() - 1 - i] = bez1[0][i];
            auto b2 = Bezier2y(temp, p1.y + 1); //" + 1" ??
            if(b2.size() != 0) {
                for(size_t i = 0; i < b2.size(); i++)
                    temp[bez1[0].size() - 1 - i] = b2[i];
            }
            bez1[0] = temp;
        }
        auto poly = CubicSpline2Contour(bez1);
        poly.Concat(CubicSpline2Contour(bez2));
        cv.Push(poly);
        if(a1 == START_CONNECT_THIN) {
            auto dir_to_start = (p1 - ps1).GetDir();
            DrawTailCircle(cv, p1, dir_to_start, kMinWidthT_mod * CURVE_THIN);
        }
        auto bez1e  = bez1[bez1.size() - 1][3],
             bez1c2 = bez1[bez1.size() - 1][2], bez2s = bez2[0][0],
             bez2c1     = bez2[0][1];
        const auto tan1 = bez1e - bez1c2, tan2 = bez2s - bez2c1;
        return {tan1, tan2};
    }

    // mincho.js/Mincho/drawAdjustedStroke
    void Mincho::DrawAdjustedStroke(
        Canva& cv, Stroke s, std::vector<Stroke> others) {
        const Dir dir12    = (s.p2 - s.p1).GetDir(),
                  dir23    = (s.p3 - s.p2).GetDir(),
                  dir34    = (s.p4 - s.p3).GetDir();
        const double rad12 = (s.p2 - s.p1).GetRad(),
                     rad23 = (s.p3 - s.p2).GetRad();
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
        }
        case STROKE_STRAIGHT: {
            const Dir dir = dir12;
            if(s.end == END_CONNECTING_H) // usually horizontal
                cv.DrawLine(s.p1, s.p2, kMinWidthYY);
            else if(s.end == END_OPEN &&
                std::abs(s.p2.y - s.p1.y) < s.p2.x - s.p1.x) { // 横笔
                // 绘制横线
                cv.DrawLine(s.p1, s.p2, kMinWidthYY);
                // 计算横末端三角的参数
                const auto param_uroko  = AdjustUrokoParam(s, others),
                           param_uroko2 = AdjustUroko2Param(s, others);
                const double urokoScale =
                    (kMinWidthU / kMinWidthY - 1.0) / 4.0 + 1.0;
                if(s.p1.y == s.p2.y) { // horizontal
                    const size_t uroko_max = std::floor(
                        Point(param_uroko, param_uroko2).GetLength());
                    // const uroko_max = param_uroko == 0 ? param_uroko2 :
                    // param_uroko
                    // ↑元の実装だとadjustUrokoによる調整がかかったものはadjustUroko2を一切通らないのでそれ以上小さくならない。
                    // Math.max(param_uroko, param_uroko2) などのほうが合理的
                    DrawUrokoH(cv, s.p2, kMinWidthYY,
                        kAdjustUrokoX[uroko_max] * urokoScale,
                        kAdjustUrokoY[uroko_max] * urokoScale);
                } else
                    DrawUroko(cv, s.p2, dir, kMinWidthYY,
                        kAdjustUrokoX[param_uroko] * urokoScale,
                        kAdjustUrokoY[param_uroko] * urokoScale);
            } else { // 竖笔
                Contour    poly_end(2);
                const auto param_tate    = AdjustTateParam(s, others);
                const auto kMinWidthT_m  = kMinWidthT - param_tate / 2;
                bool       connectVerror = false;  // Modified(2025/8/28),为表现与原版kage一致
                // 头部
                auto poly_start =
                    GetStartOfVLine(s.p1, s.p2, s.start, kMinWidthT_m, cv);
                if(s.start == START_CONNECTING_MANUAL) {
                    auto r = (s.p1 - s.p3).GetRad() - (s.p2 - s.p1).GetRad() -
                        PI / 2;
                    poly_start = GetStartOfOffsetLine(s.p1, dir, kMinWidthT_m,
                        kMinWidthT_m * std::tan(r),
                        kMinWidthT_m * -std::tan(r));
                }
                // 尾端
                switch(s.end) {
                case END_OPEN: {
                    const auto right2 = kMinWidthC * kMinWidthT_m / 2,
                               left2  = kMinWidthC * -kMinWidthT_m / 2;
                    poly_end          = GetEndOfOffsetLine(
                        s.p1, s.p2, kMinWidthT_m, right2, left2);
                    break;
                }
                case END_TURN_LEFT: {
                    auto       t1         = s.p2.MoveByDir(dir12, -kMage);
                    const auto width_func = [kMinWidthT_m](double t) -> double {
                        return kMinWidthT_m;
                    };
                    const auto new_x2 = s.p2.x -
                        kMage *
                            (((kAdjustTateStep + 4) - param_tate) /
                                (kAdjustTateStep + 4));
                    auto newp2 = Point(new_x2, s.p2.y);
                    cv.DrawQBezier(t1, s.p2, newp2, width_func,
                        [](double t) -> double { return 0; }, 0, true, false);
                    const auto param_hane = AdjustHaneParam(s, s.p2, others);
                    DrawTurnLeft(cv, newp2, kMinWidthT_m,
                        kWidth * 4 *
                            std::min(1 - param_hane / 10,
                                std::pow(kMinWidthT_m / kMinWidthT, 3)));
                    poly_end = GetEndOfLine(s.p1, t1, kMinWidthT_m);
                    break;
                }
                case END_LOWER_LEFT_CORNER: {
                    const auto param_kakato = AdjustKakatoParam(s, others);
                    const auto right2       = kAdjustKakatoL[param_kakato] +
                        kMinWidthC * kMinWidthT_m,
                               left2 = kAdjustKakatoL[param_kakato];
                    poly_end         = GetEndOfOffsetLine(
                        s.p1, s.p2, kMinWidthT_m, right2, left2);
                    break;
                }
                case END_LOWER_RIGHT_CORNER: {
                    const auto param_kakato = AdjustKakatoParam(s, others);
                    const auto right2       = kAdjustKakatoR[param_kakato] +
                        kMinWidthC * kMinWidthT_m,
                               left2 = kAdjustKakatoR[param_kakato];
                    poly_end         = GetEndOfOffsetLine(
                        s.p1, s.p2, kMinWidthT_m, right2, left2);
                    break;
                }
                case END_LOWER_LEFT_ZH_NEW: {
                    if(s.p1.x == s.p2.x) // vertical
                        DrawNewGTHboxV(cv, s.p2, kMinWidthT_m, kMinWidthY);
                    else {
                        int m;
                        if(s.p1.x > s.p2.x && s.p1.y != s.p2.y)
                            m = std::floor(
                                (s.p1.x - s.p2.x) / (s.p2.y - s.p1.y) * 3);
                        DrawNewGTHbox(cv, Point(s.p2.x + m, s.p2.y),
                            kMinWidthT_m, kMinWidthY);
                    }
                    // in the original implementation, opt2 is calculated to 413
                    // % 100 = 4, and kAdjustKakatoL[4] is manually set to 0.
                    // The appearance is typically remedied by the crossing
                    // horizontal line.
                    const auto right2 = kMinWidthC * kMinWidthT_m, left2 = 0.0;
                    poly_end = GetEndOfOffsetLine(
                        s.p1, s.p2, kMinWidthT_m, right2, left2);
                    break;
                }
                case END_LOWER_LEFT_ZH_OLD: {
                    // in the original implementation, opt2 is calculated to 313
                    // % 100 = 3, corresponding to (original) kAdjustKakatoStep.
                    const auto right2 = kAdjustKakatoL[kAdjustKakatoStep] +
                        kMinWidthC * kMinWidthT_m,
                               left2 = kAdjustKakatoL[kAdjustKakatoStep];
                    poly_end         = GetEndOfOffsetLine(
                        s.p1, s.p2, kMinWidthT_m, right2, left2);
                    break;
                }
                case END_CONNECTING_V: {
                    if(s.p1.y == s.p2.y) { // horizontal (error)
                        // cv.DrawLine(s.p1, s.p2, kMinWidthT_m);
                        cv.DrawLine(s.p1, s.p2, kMinWidthYY);  // Modified(2025/8/28)
                        connectVerror = true;
                    } else if(s.p1.x == s.p2.x) { // vertical
                        poly_end.Set(0,
                            Point(s.p2.x + kMinWidthT_m,
                                s.p2.y + kMinWidthY - 0.001));
                        poly_end.Set(1,
                            Point(s.p2.x - kMinWidthT_m,
                                s.p2.y + kMinWidthY - 0.001));
                    } else {
                        const auto rad =
                            std::atan((s.p2.y - s.p1.y) / (s.p2.x - s.p1.x));
                        const auto v = (s.p1.x > s.p2.x) ? -1 : 1;
                        poly_end.Set(0,
                            Point(s.p2.x + (kMinWidthT_m * v) / std::sin(rad),
                                s.p2.y));
                        poly_end.Set(1,
                            Point(s.p2.x - (kMinWidthT_m * v) / std::sin(rad),
                                s.p2.y));
                    }
                    break;
                }
                case END_LOWER_RIGHT_HT: {
                    if(s.p1.x == s.p2.x) // vertical
                        DrawLowerRightHTv(cv, s.p2, kMinWidthT_m, kMinWidthY);
                    else
                        DrawLowerRightHT(cv, s.p2, kMinWidthT_m, kMinWidthY);
                    if(s.p1.y == s.p2.y) { // horizontal (error)
                        // cv.DrawLine(s.p1, s.p2, kMinWidthT_m);
                        cv.DrawLine(s.p1, s.p2, kMinWidthYY);  // Modified(2025/8/28)
                        connectVerror = true;
                    } else if(s.p1.x == s.p2.x) { // vertical
                        poly_end.Set(0,
                            Point(s.p2.x + kMinWidthT_m, s.p2.y + kMinWidthY));
                        poly_end.Set(1,
                            Point(s.p2.x - kMinWidthT_m, s.p2.y + kMinWidthY));
                    } else {
                        const auto rad =
                            std::atan((s.p2.y - s.p1.y) / (s.p2.x - s.p1.x));
                        const auto v = (s.p1.x > s.p2.x) ? -1 : 1;
                        poly_end.Set(0,
                            Point(s.p2.x + (kMinWidthT_m * v) / std::sin(rad),
                                s.p2.y));
                        poly_end.Set(1,
                            Point(s.p2.x - (kMinWidthT_m * v) / std::sin(rad),
                                s.p2.y));
                    }
                    break;
                }
                default: break;
                }
                // 直线部分
                if(!connectVerror) {  // Modified(2025/8/28)
                    poly_start.Concat(poly_end);
                    cv.Push(poly_start);
                }
            }
            break;
        }
        case STROKE_CURVE: {
            // for CONNECTING_MANUAL stroke (very very tricky implementation)
            if(s.start == START_CONNECTING_MANUAL) {
                s.type = STROKE_STRAIGHT; // CURVE -> STRAIGHT
                DrawAdjustedStroke(
                    cv, s, others); // treat as STRAIGHT line data
                return;
            }
            const auto kMinWidthT_mod =
                           kMinWidthT - (int)((s.startOpt % 100) / 10) / 2,
                       end_width_factor =
                           ((int)(s.endOpt / 10) / 2) / kMinWidthT;
            // 头部
            if(s.start == START_OPEN) {
                auto p1ext = s.p1.MoveByDir(dir12, 1 * kMinWidthY * 0.5);
                if(p1ext.y <= s.p3.y) // from up to bottom
                    DrawOpenBeginCurveDown(
                        cv, p1ext, rad12, kMinWidthT_mod, kMinWidthY);
                else // from bottom to up
                    DrawOpenBeginCurveUp(
                        cv, p1ext, dir12, kMinWidthT_mod, kMinWidthY);
            } else if(s.start == START_UPPER_RIGHT_CORNER ||
                s.start == START_ROOFED_THIN)
                DrawUpperRightCorner2(cv, s.p1, kMinWidthT_mod, kMinWidthY,
                    kWidth, s.start == START_ROOFED_THIN);
            else if(s.start == START_UPPER_LEFT_CORNER) {
                auto p1ext = s.p1.MoveByDir(dir12, -kMinWidthY);
                DrawUpperLeftCorner(
                    cv, p1ext, dir12, kMinWidthT_mod); // this.kMinWidthC * ?
            }
            // 曲线部分
            const auto a2temp = s.start == START_CONNECTING_V &&
                AdjustKirikuchiParam(s, others);
            DrawCurve(cv, s.p1, s.p2, s.p3, s.start, a2temp, s.end,
                kMinWidthT_mod, end_width_factor);
            // 尾部
            switch(s.end) {
            case END_TURN_LEFT: {
                auto       t1         = s.p3.MoveByDir(dir23, -kMage * 0.439);
                const auto param_hane = AdjustHaneParam(s, s.p3, others);
                const auto width_func = [kMinWidthT_mod](double t) -> double {
                    return kMinWidthT_mod;
                };
                cv.DrawQBezier(t1, s.p3, s.p3 - Point(kMage, 0), width_func,
                    [](double t) -> double { return 0; }, 0, true, false);
                DrawTurnLeft(cv, s.p3 - Point(kMage, 0), kMinWidthT_mod,
                    kWidth * 4 *
                        std::min(1 - param_hane / 10,
                            std::pow(kMinWidthT_mod / kMinWidthT, 3)));
                break;
            }
            case END_TURN_UPWARDS:
                DrawTailCircle(cv, s.p3, dir23, kMinWidthT_mod);
                DrawTurnUpwardsPos(cv, s.p3, kMinWidthT_mod, kWidth * 5,
                    (s.p1.y < s.p3.y) ? DIR_POSX : DIR_NEGX);
                break;
            case END_STOP: break;
            default:
                if(s.end == END_OPEN) {
                    if(s.start == START_THIN || s.start == START_ROOFED_THIN)
                        DrawL2RSweepEnd(
                            cv, s.p3, dir23, kMinWidthT_mod, kL2RDfatten);
                    else if(s.start == START_CONNECT_THIN)
                        DrawL2RSweepEnd(cv, s.p3, dir23, kMinWidthT_mod, 1);
                }
                break;
            }
            break;
        }
        case STROKE_BENDING:
        case STROKE_BENDING_ROUND: {
            const auto param_tate =
                s.type == STROKE_BENDING ? AdjustTateParam(s, others) : 0;
            const auto param_mage =
                s.type == STROKE_BENDING ? AdjustMageParam(s, others) : 0;
            const auto kMinWidthT_m    = kMinWidthT - param_tate / 2;
            const auto kMinWidthT_mage = kMinWidthT - param_mage / 2;
            double     rate;
            if(s.type == STROKE_BENDING)
                rate = 1;
            else { // BENDING_ROUND
                rate = 6;
                if((s.p3 - s.p2).GetLength() < 120) // smaller than 120 x 120
                    rate = (s.p3 - s.p2).GetLength() / 120 * 6;
            }
            auto t1 = s.p2.MoveByDir(dir12, -kMage * rate),
                 t2 = s.p2.MoveByDir(dir23, kMage * rate);
            // first line
            auto poly_start =
                GetStartOfVLine(s.p1, s.p2, s.start, kMinWidthT_m, cv);
            const auto width_func = [kMinWidthT_mage, kMinWidthT_m](
                                        double t) -> double {
                return (kMinWidthT_mage - kMinWidthT_m) * t + kMinWidthT_m;
            };
            std::vector<CubicSpline> bez1, bez2;
            qBezier(t1, s.p2, t2, width_func,
                [](double t) -> double { return 0; }, bez1, bez2);
            poly_start.Concat(CubicSpline2Contour(bez1));
            auto edd = GetEndOfLine(t2, s.p3, kMinWidthT_mage);
            poly_start.Concat(edd);
            poly_start.Concat(CubicSpline2Contour(bez2));
            cv.Push(poly_start);

            if(s.p2.y == s.p3.y) {
                if(t2.x < s.p3.x)
                    DrawCircleBendPos(cv, s.p3, DIR_POSX, kMinWidthT_mage);
                else
                    DrawCircleBendNeg(cv, s.p3, DIR_NEGX, kMinWidthT_mage);
                if(s.end == END_TURN_UPWARDS) {
                    if(t2.x < s.p3.x)
                        DrawTurnUpwardsPos(cv, s.p3, kMinWidthT_mage,
                            kWidth *
                                (4 * (1 - param_mage / kAdjustMageStep) + 1),
                            DIR_POSX);
                    else
                        DrawTurnUpwardsNeg(cv, s.p3, kMinWidthT_mage,
                            kWidth *
                                (4 * (1 - param_mage / kAdjustMageStep) + 1),
                            DIR_NEGX);
                }
            } else {
                const auto dir = (s.p3 - s.p2).GetDir();
                if(t2.x < s.p3.x)
                    DrawCircleBendPos(cv, s.p3, dir, kMinWidthT_mage);
                else
                    DrawCircleBendNeg(cv, s.p3, dir, kMinWidthT_mage);
                if(s.end == END_TURN_UPWARDS) {
                    if(t2.x < s.p3.x)
                        DrawTurnUpwardsPos(
                            cv, s.p3, kMinWidthT_mage, kWidth * 5, dir);
                    else
                        DrawTurnUpwardsNeg(
                            cv, s.p3, kMinWidthT_mage, kWidth * 5, dir);
                }
            }
            break;
        }
        case 12: {
            // throw "error: unknown stroketype 12";
            break;
        }
        case STROKE_BEZIER: {
            const double kMinWidthT_mod =
                kMinWidthT - (int)((s.startOpt % 100) / 10) / 2.0;
            // head
            if(s.start == START_OPEN) {
                auto p1ext = s.p1.MoveByDir(dir12, 1 * kMinWidthY * 0.5);
                if(p1ext.y <= s.p4.y) // from up to bottom
                    DrawOpenBeginCurveDown(
                        cv, p1ext, rad12, kMinWidthT_mod, kMinWidthY);
                else // from bottom to up
                    DrawOpenBeginCurveUp(
                        cv, p1ext, dir12, kMinWidthT_mod, kMinWidthY);
            } else if(s.start == START_UPPER_RIGHT_CORNER ||
                s.start == START_ROOFED_THIN)
                DrawUpperRightCorner2(cv, s.p1, kMinWidthT_mod, kMinWidthY,
                    kWidth, s.start == START_ROOFED_THIN);
            else if(s.start == START_UPPER_LEFT_CORNER) {
                auto p1ext = s.p1.MoveByDir(dir12, -kMinWidthY);
                DrawUpperLeftCorner(cv, p1ext, dir12, kMinWidthT_mod);
            }
            // body
            auto tan = DrawBezier(cv, s.p1, s.p2, s.p3, s.p4, s.start,
                s.startOpt == 1, s.end, kMinWidthT_mod);
            // tail
            switch(s.end) {
            case END_TURN_LEFT: {
                auto       pt1        = s.p4.MoveByDir(dir34, -kMage * 0.439);
                const auto width_func = [kMinWidthT_mod](double t) -> double {
                    return kMinWidthT_mod;
                };
                cv.DrawQBezier(pt1, s.p4, Point(s.p4.x - kMage, s.p4.y),
                    width_func, [](double t) -> double { return 0; }, 0, true,
                    false);
                const auto param_hane = AdjustHaneParam(s, s.p4, others);
                DrawTurnLeft(cv, Point(s.p4.x - kMage, s.p4.y), kMinWidthT_mod,
                    kWidth * 4 *
                        std::min(1 - param_hane / 10,
                            std::pow(kMinWidthT_mod / kMinWidthT, 3)));
                break;
            }
            case END_TURN_UPWARDS:
                DrawTailCircle(cv, s.p4, dir34, kMinWidthT_mod);
                DrawTurnUpwardsPos(cv, s.p4, kMinWidthT_mod, kWidth * 5,
                    (s.p1.y < s.p4.y) ? DIR_POSX : DIR_NEGX);
                break;
            case END_STOP: {
                auto p4ex = s.p4.MoveByDir(dir34, -kMinWidthT_mod * 0.52);
                if(s.start == START_THIN || s.start == START_ROOFED_THIN)
                    DrawTailCircleTan(
                        cv, p4ex, dir34, kMinWidthT_mod * 1.1, tan[0], tan[1]);
                else // CONNECT_THIN
                    DrawTailCircle(cv, p4ex, dir34, kMinWidthT_mod);
                break;
            }
            default:
                if((s.start == START_THIN || s.start == START_ROOFED_THIN) &&
                    s.end == END_OPEN)
                    DrawL2RSweepEnd(
                        cv, s.p4, dir34, kMinWidthT_mod, kL2RDfatten);
                break;
            }
            break;
        }
        case STROKE_VCURVE: {
            const auto param_tate   = AdjustTateParam(s, others);
            const auto kMinWidthT_m = kMinWidthT - param_tate / 2;
            // straight
            auto poly_start =
                GetStartOfVLine(s.p1, s.p2, s.start, kMinWidthT_m, cv);
            auto poly_end = GetEndOfLine(s.p1, s.p2, kMinWidthT_m);
            poly_start.Concat(poly_end);
            cv.Push(poly_start);
            // semicircle for connection point
            DrawTailCircle(cv, s.p2, Rad2Dir(rad23 + PI), kMinWidthT_m);
            // curve
            const auto width_func = [kMinWidthT_m](double t) -> double {
                // const deltad = Math.pow(1.0-t,0.7)*0.8+0.2;
                const auto deltad = (1 - std::pow(t, 1.8)) * 0.85 + 0.15;
                return deltad * kMinWidthT_m;
            };
            cv.DrawQBezier(s.p2, s.p3, s.p4, width_func,
                [kMinWidthT_m](double t) -> double {
                return -1.8 * std::pow(t, 0.8) * 0.85 * kMinWidthT_m;
            }, 0, false, false);
            break;
        }
        case 9: // may not be exist ... no need
            // kageCanvas[y1][x1] = 0;
            // kageCanvas[y2][x2] = 0;
            break;
        default: break;
        }
    }

    // mincho.js/Mincho/getPolygons
    Canva Mincho::DrawGlyph(std::vector<Stroke> strokes) {
        Canva cv;
        for(size_t i = 0; i < strokes.size(); i++) {
            auto tempdata = strokes;
            tempdata.erase(tempdata.begin() + i);
            DrawAdjustedStroke(cv, strokes[i], tempdata);
        }
        return cv;
    }

} // namespace Kage
