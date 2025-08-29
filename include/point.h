#ifndef _POINT_H
#define _POINT_H

#include <string>

namespace Kage {

    typedef struct {
        double sin;
        double cos;
    } Dir;

    const Dir DIR_POSX = {sin: 0, cos: 1};
    const Dir DIR_POSY = {sin: 1, cos: 0};
    const Dir DIR_NEGX = {sin: 0, cos: -1};
    const Dir DIR_NEGY = {sin: -1, cos: 0};

    Dir Rad2Dir(double rad);

    class Point {
    public:
        double x;
        double y;

        Point();
        Point(double rad);
        Point(double _x, double _y);

        bool         isVaild();
        Point        operator+(Point right);
        Point        operator-(Point right);
        Point        operator*(double right);
        friend Point operator*(double left, Point p);
        Point        operator/(double right);
        friend Point operator/(double left, Point p);
        bool         operator==(Point right);
        bool         operator!=(Point right);
        double       Dot(Point right);
        Point        Normalize();

        double GetLength();
        Dir    GetDir();
        Point  MoveByDir(Dir dir, double delta);
        Point  GetExtendedDest(Point dest, double delta);
        Point  UnitNormalVector();
        Point  toLength(double l);
        double GetRad();
    };

    class PointMaker {
        Point  _p;
        Dir    _dir;
        double _scale;

    public:
        PointMaker(Point p, Dir dir = DIR_POSX, double scale = 1);

        void  SetPos(Point p);
        void  SetDir(Dir dir);
        void  SetScale(double scale);
        Point Vec(Point p);
    };

#define UNINIT_POINT Point(NAN, NAN)

    std::string Double2String(double num);

} // namespace Kage

#endif
