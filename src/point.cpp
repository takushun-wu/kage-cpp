#include <cmath>

#include "point.h"

namespace Kage {

    // util.js/rad_to_dir
    Dir Rad2Dir(double rad) {
        return {sin: std::sin(rad), cos: std::cos(rad)};
    }

    Point::Point() {
        x = NAN, y = NAN;
    }

    // util.js/rad_to_vector
    Point::Point(double rad) {
        x = std::cos(rad), y = std::sin(rad);
    }

    Point::Point(double _x, double _y) {
        x = _x, y = _y;
    }

    bool Point::isVaild() {
        return !(std::isnan(x) || std::isnan(x));
    }

    Point Point::operator+(Point right) {
        return Point(x + right.x, y + right.y);
    }

    Point Point::operator-(Point right) {
        return Point(x - right.x, y - right.y);
    }

    Point Point::operator*(double right) {
        return Point(x * right, y * right);
    }

    Point operator*(double left, Point p) {
        return Point(left * p.x, left * p.y);
    }

    Point Point::operator/(double right) {
        return Point(x / right, y / right);
    }

    bool Point::operator==(Point right) {
        return x == right.x && y == right.y;
    }

    bool Point::operator!=(Point right) {
        return x != right.x || y != right.y;
    }

    Point operator/(double left, Point p) {
        return Point(left / p.x, left / p.y);
    }

    double Point::Dot(Point right) {
        return x * right.x + y * right.y;
    }

    Point Point::Normalize() {
        return this->toLength(1);
    }

    // util.js/norm2
    double Point::GetLength() {
        return std::sqrt(x * x + y * y);
    }

    // util.js/get_dir
    Dir Point::GetDir() {
        if(y == 0)
            return x < 0 ? DIR_NEGX : DIR_POSX;
        else if(x == 0)
            return y < 0 ? DIR_NEGY : DIR_POSY;
        else {
            double rad = std::atan2(y, x);
            return {sin: std::sin(rad), cos: std::cos(rad)};
        }
    }

    // util.js/moved_point
    Point Point::MoveByDir(Dir dir, double delta) {
        return Point(x + delta * dir.cos, y + delta * dir.sin);
    }

    // util.js/get_extended_dest
    // 计算起始点与终止点的延长点，delta为距离
    Point Point::GetExtendedDest(Point dest, double delta) {
        return dest.MoveByDir((dest - *this).GetDir(), delta);
    }

    // util.js/unit_normal_vector
    // to the right(clockwise (in the display coordinate))
    // 将向量转变成x轴为正的单位向量
    Point Point::UnitNormalVector() {
        double ia, ib;
        // line SUICHOKU by vector
        if(x != 0 && y != 0) {
            double ir = std::atan(y / x * -1.0);
            ia = std::sin(ir), ib = std::cos(ir);
        } else if(x == 0)
            ia = y < 0 ? -1 : 1, ib = 0;
        else
            ia = 0, ib = 1;
        // reverse if vector is going 2nd/3rd quadrants
        if(x <= 0) ia = ia * -1, ib = ib * -1;
        return Point(ia, ib);
    }

    // util.js/vector_to_len
    Point Point::toLength(double l) {
        return (*this) * l / this->GetLength();
    }

    // util.js/get_rad
    double Point::GetRad() {
        return std::atan2(y, x);
    }

    // pointmaker.js/PointMaker/constructor
    PointMaker::PointMaker(Point p, Dir dir, double scale) {
        _p = p, _dir = dir, _scale = scale;
    }

    // pointmaker.js/PointMaker/setpos
    void PointMaker::SetPos(Point p) {
        _p = p;
    }

    void PointMaker::SetDir(Dir dir) {
        _dir = dir;
    }

    void PointMaker::SetScale(double scale) {
        _scale = scale;
    }

    Point PointMaker::Vec(Point p) {
        return _p +
            _scale *
            Point(_dir.cos * p.x - _dir.sin * p.y,
                _dir.sin * p.x + _dir.cos * p.y);
    }

    std::string Double2String(double num) {
        char buffer[128];
        sprintf(buffer, "%g", num);
        return std::string(buffer);
    }

} // namespace Kage
