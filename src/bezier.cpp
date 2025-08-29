#include <algorithm>

#include "bezier.h"

namespace Kage {

    // bezier.js/generalBezier
    void GeneralBezier(std::function<double(double)> const& x_fun,
        std::function<double(double)> const&                y_fun,
        std::function<double(double)> const&                dx_fun,
        std::function<double(double)> const&                dy_fun,
        std::function<double(double)> const&                width_func,
        std::function<double(double)> const&                width_func_d,
        std::vector<CubicSpline>& bez1, std::vector<CubicSpline>& bez2) {
        std::vector<Point> a1(BEZIER_STEPS + 1), a2(BEZIER_STEPS + 1);
        for(auto tt = 0; tt <= BEZIER_STEPS; tt++) {
            double t  = (double)tt / BEZIER_STEPS;
            Point  xy = Point(x_fun(t), y_fun(t)),
                  iab = Point(dx_fun(t), dy_fun(t)).UnitNormalVector() *
                width_func(t);
            a1[tt]                = xy - iab;
            a2[BEZIER_STEPS - tt] = xy + iab; // reverse
        }
        // bez1 = FitCurve(a1, 0.03), bez2 = FitCurve(a2, 0.03);
        bez1 = FitCurve(a1, 0.1), bez2 = FitCurve(a2, 0.1); // Modified(2025/8/28)
    }

    // bezier.js/generalBezier2
    void GeneralBezier2(std::function<double(double)> const& x_fun,
        std::function<double(double)> const&                 y_fun,
        std::function<double(double)> const&                 dx_fun,
        std::function<double(double)> const&                 dy_fun,
        std::function<double(double)> const&                 width_func,
        std::function<double(double)> const&                 width_func_d,
        std::function<Point(double)> const&                  dir_func,
        std::vector<CubicSpline>& bez1, std::vector<CubicSpline>& bez2) {
        std::vector<Point> a1(BEZIER_STEPS + 1), a2(BEZIER_STEPS + 1);
        for(auto tt = 0; tt <= BEZIER_STEPS; tt++) {
            double t              = (double)tt / BEZIER_STEPS;
            Point  xy             = Point(x_fun(t), y_fun(t)),
                  iab             = dir_func(t) * width_func(t);
            a1[tt]                = xy - iab;
            a2[BEZIER_STEPS - tt] = xy + iab; // reverse
        }
        // bez1 = FitCurve(a1, 0.03), bez2 = FitCurve(a2, 0.03);
        bez1 = FitCurve(a1, 0.1), bez2 = FitCurve(a2, 0.1); // Modified(2025/8/28)
    }

    // bezier.js/qBezier
    void qBezier(Point p1, Point c, Point p2,
        std::function<double(double)> const& width_func,
        std::function<double(double)> const& width_func_d,
        std::vector<CubicSpline>& bez1, std::vector<CubicSpline>& bez2) {
        auto x_fun = [&p1, &c, &p2](double t) -> double {
            return (1.0 - t) * (1.0 - t) * p1.x + 2.0 * t * (1.0 - t) * c.x +
                t * t * p2.x;
        };
        auto y_fun = [&p1, &c, &p2](double t) -> double {
            return (1.0 - t) * (1.0 - t) * p1.y + 2.0 * t * (1.0 - t) * c.y +
                t * t * p2.y;
        };
        auto dx_fun = [&p1, &c, &p2](double t) -> double {
            return (p1.x - 2.0 * c.x + p2.x) * 2.0 * t +
                (-2.0 * p1.x + 2.0 * c.x);
        };
        auto dy_fun = [&p1, &c, &p2](double t) -> double {
            return (p1.y - 2.0 * c.y + p2.y) * 2.0 * t +
                (-2.0 * p1.y + 2.0 * c.y);
        };
        GeneralBezier(
            x_fun, y_fun, dx_fun, dy_fun, width_func, width_func_d, bez1, bez2);
    }

    // bezier.js/qBezier2
    void qBezier2(Point p1, Point c, Point p2,
        std::function<double(double)> const& width_func,
        std::function<double(double)> const& width_func_d,
        std::vector<CubicSpline>& bez1, std::vector<CubicSpline>& bez2) {
        // similar to qBezier(), but the direction changes at a constant speed
        // (not decided by normal vector)
        auto x_fun = [&p1, &c, &p2](double t) -> double {
            return (1.0 - t) * (1.0 - t) * p1.x + 2.0 * t * (1.0 - t) * c.x +
                t * t * p2.x;
        };
        auto y_fun = [&p1, &c, &p2](double t) -> double {
            return (1.0 - t) * (1.0 - t) * p1.y + 2.0 * t * (1.0 - t) * c.y +
                t * t * p2.y;
        };
        auto dx_fun = [&p1, &c, &p2](double t) -> double {
            return (p1.x - 2.0 * c.x + p2.x) * 2.0 * t +
                (-2.0 * p1.x + 2.0 * c.x);
        };
        auto dy_fun = [&p1, &c, &p2](double t) -> double {
            return (p1.y - 2.0 * c.y + p2.y) * 2.0 * t +
                (-2.0 * p1.y + 2.0 * c.y);
        };
        double cent_x    = (p1.x + 4 * c.x + p2.x) / 6,
               cent_y    = (p1.y + 4 * c.y + p2.y) / 6,
               rad_begin = std::atan2(cent_y - p1.y, cent_x - p1.x),
               rad_end   = std::atan2(p2.y - cent_y, p2.x - cent_x);
        if(rad_end - rad_begin > PI)
            rad_end -= PI * 2;
        else if(rad_begin - rad_end > PI)
            rad_end += PI * 2;
        std::function<Point(double)> dir_func = [rad_begin, rad_end](
                                                    double t) -> Point {
            return Point((1 - t) * rad_begin + t * rad_end + PI / 2);
        };
        GeneralBezier2(x_fun, y_fun, dx_fun, dy_fun, width_func, width_func_d,
            dir_func, bez1, bez2);
    }

    // bezier.js/cBezier
    void cBezier(Point p1, Point c1, Point c2, Point p2,
        std::function<double(double)> const& width_func,
        std::function<double(double)> const& width_func_d,
        std::vector<CubicSpline>& bez1, std::vector<CubicSpline>& bez2) {
        auto x_fun = [&p1, &c1, &c2, &p2](double t) -> double {
            return (1.0 - t) * (1.0 - t) * (1.0 - t) * p1.x +
                3.0 * t * (1.0 - t) * (1.0 - t) * c1.x +
                3 * t * t * (1.0 - t) * c2.x + t * t * t * p2.x;
        };
        auto y_fun = [&p1, &c1, &c2, &p2](double t) -> double {
            return (1.0 - t) * (1.0 - t) * (1.0 - t) * p1.y +
                3.0 * t * (1.0 - t) * (1.0 - t) * c1.y +
                3 * t * t * (1.0 - t) * c2.y + t * t * t * p2.y;
        };
        auto dx_fun = [&p1, &c1, &c2, &p2](double t) -> double {
            return t * t * (-3 * p1.x + 9 * c1.x - 9 * c2.x + 3 * p2.x) +
                t * (6 * p1.x - 12 * c1.x + 6 * c2.x) - 3 * p1.x + 3 * c1.x;
        };
        auto dy_fun = [&p1, &c1, &c2, &p2](double t) -> double {
            return t * t * (-3 * p1.y + 9 * c1.y - 9 * c2.y + 3 * p2.y) +
                t * (6 * p1.y - 12 * c1.y + 6 * c2.y) - 3 * p1.y + 3 * c1.y;
        };
        GeneralBezier(
            x_fun, y_fun, dx_fun, dy_fun, width_func, width_func_d, bez1, bez2);
    }

    // util.js/solveCubic
    // 只考虑实数情况
    std::vector<double> SolveCubic(double a, double b, double c, double d) {
        if(std::abs(a) < 1e-8) { // Quadratic case, ax^2+bx+c=0
            a = b, b = c, c = d;
            if(std::abs(a) < 1e-8) { // Linear case, ax+b=0
                a = b, b = c;
                if(std::abs(a) < 1e-8) // Degenerate case
                    return {};
                return {-b / a};
            }
            double D = b * b - 4 * a * c;
            if(std::abs(D) < 1e-8)
                return {-b / (2 * a)};
            else if(D > 0)
                return {(-b + std::sqrt(D)) / (2 * a),
                    (-b - std::sqrt(D)) / (2 * a)};
            return {};
        }
        // Convert to depressed cubic t^3+pt+q = 0 (subst x = t - b/3a)
        double p = (3 * a * c - b * b) / (3 * a * a),
               q = (2 * b * b * b - 9 * a * b * c + 27 * a * a * d) /
            (27 * a * a * a);
        std::vector<double> roots;
        if(std::abs(p) < 1e-8) { // p = 0 -> t^3 = -q -> t = -q^1/3
            roots = {std::cbrt(-q)};
        } else if(std::abs(q) < 1e-8) { // q = 0 -> t^3 + pt = 0 -> t(t^2+p)=0
            if(p < 0) roots = {std::sqrt(-p), -std::sqrt(-p)};
        } else {
            double D = q * q / 4 + p * p * p / 27;
            if(std::abs(D) < 1e-8) // D = 0 -> two roots
                roots = {-1.5 * q / p, 3 * q / p};
            else if(D > 0) { // Only one real root
                double u = std::cbrt(-q / 2 - std::sqrt(D));
                roots    = {u - p / (3 * u)};
            } else { // D < 0, three roots, but needs to use complex
                     // numbers/trigonometric solution
                double u = 2 * std::sqrt(-p / 3);
                double t = std::acos(3 * q / p / u) /
                    3; // D < 0 implies p < 0 and acos argument in [-1..1]
                double k = 2 * PI / 3;
                roots    = {u * std::cos(t), u * std::cos(t - k),
                       u * std::cos(t - 2 * k)};
            }
        }
        // Convert back from depressed cubic
        for(auto& i: roots)
            i -= b / (3 * a);
        return roots;
    }

    // util.js/get_extreme_points
    // 求ax³+bx²+cx+d=0的极值点所在x值
    std::vector<double> GetCubicExtremePoints(double a0, double b0, double c0) {
        double a = a0 * 3, b = b0 * 2, c = c0, d = b * b - (4 * a * c);
        if(d > 0)
            return {((-1) * b + std::sqrt(d)) / (2 * a),
                ((-1) * b - std::sqrt(d)) / (2 * a)};
        else if(d == 0)
            return {(-1) * b / (2 * a)};
        else
            return {};
    }

    // util.js/stretch_bezier_end
    // 按t延伸/裁切贝塞尔曲线
    CubicSpline StretchBezierEnd(CubicSpline bez, double t) {
        Point start = bez[0];
        Point c1    = (1 - t) * bez[0] + t * bez[1];
        Point c2    = (1 - t) * (1 - t) * bez[0] + 2.0 * t * (1 - t) * bez[1] +
            t * t * bez[2];
        Point end = (1 - t) * (1 - t) * (1 - t) * bez[0] +
            3 * t * (1 - t) * (1 - t) * bez[1] + 3 * t * t * (1 - t) * bez[2] +
            t * t * t * bez[3];
        return {start, c1, c2, end};
    }

    // util.js/bezier_to_y/hit_extreme
    bool HitExtreme(std::vector<double> ext, double x1, double x2) {
        for(auto e: ext) {
            if(x1 < e && e < x2) return true;
        }
        return false;
    }

    // util.js/bezier_to_y
    // 使贝塞尔曲线延伸至指定y值，若无法延伸则返回原曲线
    CubicSpline Bezier2y(CubicSpline bez, double y) {
        double a = bez[3].y - 3 * bez[2].y + 3 * bez[1].y - bez[0].y,
               b = 3 * bez[2].y - 6 * bez[1].y + 3 * bez[0].y,
               c = 3 * bez[1].y - 3 * bez[0].y, d = bez[0].y;
        auto yy  = SolveCubic(a, b, c, d - y),
             ext = GetCubicExtremePoints(a, b, c);
        std::sort(yy.begin(), yy.end(), std::greater<double>());
        for(auto i: yy) {
            if(0 < i && i < 1) return StretchBezierEnd(bez, i);
        }
        for(auto it = yy.rbegin(); it != yy.rend(); ++it) {
            if(*it > 1 && !HitExtreme(ext, 1, *it))
                return StretchBezierEnd(bez, *it);
        }
        return bez;
    }

    // util.js/bezier_to_line
    // 使贝塞尔曲线延伸至指定直线，若无法延伸则返回原曲线
    CubicSpline Bezier2Line(CubicSpline bez, Point p0, double rad) {
        CubicSpline bezier_genten_rotated, bez_edited_return;
        // genten_rotate（旋转贝塞尔曲线）
        for(size_t i = 0; i < 4; i++) {
            Point p1 = bez[i] - p0;
            bezier_genten_rotated[i] =
                Point(std::cos(-rad) * p1.x - std::sin(-rad) * p1.y,
                    std::sin(-rad) * p1.x + std::cos(-rad) * p1.y);
        }
        auto bez_edited = Bezier2y(bezier_genten_rotated, 0);
        // genten_rotate_inv（逆旋转贝塞尔曲线）
        bez_edited_return[0] =
            bez[0]; // 始点は変わらないはずなので誤差を防ぐため元の値を代入
        for(size_t i = 1; i < 4; i++) {
            Point p = bez_edited[i];
            bez_edited_return[i] =
                Point(std::cos(rad) * p.x - std::sin(rad) * p.y,
                    std::sin(rad) * p.x + std::cos(rad) * p.y) +
                p0;
        }
        return bez_edited_return;
    }

    // bezier.js/Bezier/bez_to_poly
    Contour CubicSpline2Contour(std::vector<CubicSpline> bezs) {
        Contour poly;
        poly.MoveTo(bezs.at(0)[0]);
        for(auto i: bezs)
            poly.CubicTo(i[1], i[2], i[3]);
        return poly;
    }

} // namespace Kage
