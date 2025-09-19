#include <cmath>
#include <vector>

#include "bezier.h"
#include "canva.h"
#include "point.h"

namespace Kage {

    // polygons.js/Polygons/constructor
    Canva::Canva() {
        _contours.clear();
    }

    std::vector<Contour> Canva::Contours() {
        return _contours;
    }

    // polygons.js/Polygons/clear
    void Canva::Clear() {
        _contours.clear();
    }

    // polygons.js/Polygons/push
    size_t Canva::Push(Contour contour) {
        // only a simple check
        double minx = 200, maxx = 0, miny = 200, maxy = 0;
        size_t error = 0;
        for(auto i: contour.Points()) {
            if(i.x < minx) minx = i.x;
            if(i.x > maxx) maxx = i.x;
            if(i.y < miny) miny = i.y;
            if(i.y > maxy) maxy = i.y;
            if(!i.isVaild()) error++;
        }
        if(error == 0 && minx != maxx && miny != maxy && contour.Size() >= 1) {
            Contour newArray;
            newArray.Push(contour.Shift());
            while(contour.Size() != 0)
                newArray.Push(contour.Shift());
            if(newArray.Size() >= 1) _contours.push_back(newArray);
        }
        return _contours.size();
    }

    // polygons.js/Polygons/concat
    size_t Canva::Concat(Canva canva) {
        for(auto i: canva._contours)
            _contours.push_back(i);
        return _contours.size();
    }

    size_t Canva::Size() {
        return _contours.size();
    }

    // fontcanvas.js/FontCanvas/drawLine
    void Canva::DrawLine(Point p1, Point p2, double halfWidth) {
        Contour    contour;
        PointMaker p(p1, (p2 - p1).GetDir());
        contour.MoveTo(p.Vec(Point(0, halfWidth)));
        contour.LineTo(p.Vec(Point(0, -halfWidth)));
        p.SetPos(p2);
        contour.LineTo(p.Vec(Point(0, -halfWidth)));
        contour.LineTo(p.Vec(Point(0, halfWidth)));
        this->Push(contour);
    }

    // fontcanvas.js/FontCanvas/drawQBezier
    void Canva::DrawQBezier(Point p1, Point ps, Point p2,
        std::function<double(double)> const& width_func,
        std::function<double(double)> const& width_func_d, size_t curve_step,
        bool fix_begin, bool fix_end) {
        std::vector<CubicSpline> bez1, bez2;
        qBezier(p1, ps, p2, width_func, width_func_d, bez1, bez2);
        auto poly = CubicSpline2Contour(bez1);
        if(fix_end) {
            PointMaker p(p2, (p2 - ps).GetDir());
            poly.LineTo(p.Vec(Point(0.1, -width_func(1)))); // fix_union
            poly.LineTo(p.Vec(Point(0.1, width_func(1))));  // fix_union
        }
        poly.Concat(CubicSpline2Contour(bez2));
        if(fix_begin) {
            PointMaker p(p1, (p1 - ps).GetDir());
            poly.LineTo(p.Vec(Point(0.1, -width_func(0)))); // fix_union
            poly.LineTo(p.Vec(Point(0.1, width_func(0))));  // fix_union
        }
        this->Push(poly);
    }

    // fontcanvas.js/FontCanvas/drawCBezier
    void Canva::DrawCBezier(Point p1, Point ps1, Point ps2, Point p2,
        std::function<double(double)> const& width_func,
        std::function<double(double)> const& width_func_d, size_t curve_step) {
        std::vector<CubicSpline> bez1, bez2;
        cBezier(p1, ps1, ps2, p2, width_func, width_func_d, bez1, bez2);
        auto poly = CubicSpline2Contour(bez1);
        poly.Concat(CubicSpline2Contour(bez2));
        this->Push(poly);
    }

    bool isInside(Contour c, Point p1, Point p2) {
        for(auto j: c.Points()) {
            if(p1.x > j.x || j.x > p2.x || p1.y > j.y || j.y > p2.y)
                return false;
        }
        return true;
    }

    // fontcanvas.js/FontCanvas/flip_left_right
    void Canva::FlipLR(Point p1, Point p2) {
        for(auto& i: _contours) {
            if(isInside(i, p1, p2)) {
                for(auto& j: i.Points())
                    j.x = std::floor((p2.x - (j.x - p1.x)) * 10) / 10;
                i.Reverse();
            }
        }
    }

    // fontcanvas.js/FontCanvas/flip_up_down
    void Canva::FlipUD(Point p1, Point p2) {
        for(auto& i: _contours) {
            if(isInside(i, p1, p2)) {
                for(auto& j: i.Points())
                    j.y = std::floor((p2.y - (j.y - p1.y)) * 10) / 10;
                i.Reverse();
            }
        }
    }

    // fontcanvas.js/FontCanvas/rotate90
    void Canva::Rotate90(Point p1, Point p2) {
        for(auto& i: _contours) {
            if(isInside(i, p1, p2)) {
                for(auto& j: i.Points()) {
                    auto x = j.x, y = j.y;
                    j.x = std::floor((p1.x + (p2.y - y)) * 10) / 10,
                    j.y = std::floor((p1.y + (x - p1.x)) * 10) / 10;
                }
            }
        }
    }

    // fontcanvas.js/FontCanvas/rotate180
    void Canva::Rotate180(Point p1, Point p2) {
        for(auto& i: _contours) {
            if(isInside(i, p1, p2)) {
                for(auto& j: i.Points()) {
                    auto x = j.x, y = j.y;
                    j.x = std::floor((p2.x - (x - p1.x)) * 10) / 10,
                    j.y = std::floor((p2.y - (y - p1.y)) * 10) / 10;
                }
            }
        }
    }

    // fontcanvas.js/FontCanvas/rotate270
    void Canva::Rotate270(Point p1, Point p2) {
        for(auto& i: _contours) {
            if(isInside(i, p1, p2)) {
                for(auto& j: i.Points()) {
                    auto x = j.x, y = j.y;
                    j.x = std::floor((p1.x + (y - p1.y)) * 10) / 10,
                    j.y = std::floor((p2.y - (x - p1.x)) * 10) / 10;
                }
            }
        }
    }

} // namespace Kage
