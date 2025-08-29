#include <cmath>

#include "contour.h"

namespace Kage {

    // polygon.js/Polygon/constructor
    Contour::Contour() {
        ptTypes.clear();
        pts.clear();
        _isClosed = true;
    }

    // polygon.js/Polygon/constructor
    Contour::Contour(size_t num) {
        ptTypes.clear();
        pts.clear();
        ptTypes.resize(num);
        pts.resize(num);
        for(size_t i = 0; i < num; i++)
            ptTypes.at(i) = PT_POINT, pts.at(i) = Point(0, 0);
        _isClosed = true;
    }

    std::vector<ptType>& Contour::PointTypes() {
        return ptTypes;
    }

    std::vector<Point>& Contour::Points() {
        return pts;
    }

    bool Contour::isClosed() {
        return _isClosed;
    }

    // polygon.js/Polygon/push
    size_t Contour::Push(Point p, ptType type) {
        ptTypes.push_back(
            (type != PT_QUAD && type != PT_CUBIC) ? PT_POINT : type);
        pts.push_back(p);
        return pts.size();
    }

    size_t Contour::Push(PointTuple ptuple) {
        ptTypes.push_back(
            (ptuple.ptType != PT_QUAD && ptuple.ptType != PT_CUBIC)
                ? PT_POINT
                : ptuple.ptType);
        pts.push_back(ptuple.pt);
        return pts.size();
    }

    // polygon.js/Polygon/set
    void Contour::Set(size_t ind, Point p, ptType type) {
        ptTypes.at(ind) =
            (type != PT_QUAD && type != PT_CUBIC) ? PT_POINT : type,
        pts.at(ind) = p;
    }

    // polygon.js/Polygon/reverse
    void Contour::Reverse() {
        std::vector<ptType> ptTypes_(ptTypes.size());
        std::vector<Point>  pts_(pts.size());
        for(size_t i = 0; i < pts.size(); i++)
            ptTypes_.at(i) = ptTypes.at(pts.size() - 1 - i),
            pts_.at(i)     = pts.at(pts.size() - 1 - i);
        ptTypes.swap(ptTypes_), pts.swap(pts_);
    }

    // polygon.js/Polygon/concat
    size_t Contour::Concat(Contour contour) {
        auto origSize = pts.size();
        ptTypes.resize(ptTypes.size() + contour.ptTypes.size());
        pts.resize(pts.size() + contour.pts.size());
        for(auto i = origSize; i < pts.size(); i++)
            ptTypes.at(i) = contour.ptTypes.at(i - origSize),
            pts.at(i)     = contour.pts.at(i - origSize);
        return pts.size();
    }

    // polygon.js/Polygon/shift
    PointTuple Contour::Shift() {
        PointTuple o = {ptType: ptTypes.at(0), pt: pts.at(0)};
        ptTypes.erase(ptTypes.begin()), pts.erase(pts.begin());
        return o;
    }

    void Contour::SetClosed(bool closed) {
        _isClosed = closed;
    }

    size_t Contour::Size() {
        return pts.size();
    }

    void Contour::MoveTo(Point p) {
        ptTypes.clear(), pts.clear();
        this->Push(p, PT_POINT);
    }

    void Contour::LineTo(Point p) {
        this->Push(p, PT_POINT);
    }

    void Contour::QuadraticTo(Point p1, Point p2) {
        this->Push(p1, PT_QUAD);
        this->Push(p2, PT_POINT);
    }

    void Contour::CubicTo(Point p1, Point p2, Point p3) {
        this->Push(p1, PT_CUBIC);
        this->Push(p2, PT_CUBIC);
        this->Push(p3, PT_POINT);
    }

} // namespace Kage
