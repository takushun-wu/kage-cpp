#include "export.h"

#include <cstdio>
#include <iostream>

#include "point.h"

namespace Kage {

    // polygon.js/Polygon/get_sub_path_svg
    std::string Contour2SVGstr(Contour contour) {
        std::string buffer     = "M";
        auto        firstPoint = contour.Points().at(0);
        buffer                += Double2String(firstPoint.x) + "," +
            Double2String(firstPoint.y) + " ";
        ptType mode = -1;
        for(size_t j = 1; j < contour.Points().size(); j++) {
            if(contour.PointTypes().at(j) == PT_QUAD && mode != PT_QUAD)
                buffer += "Q", mode = PT_QUAD;
            else if(contour.PointTypes().at(j - 1) == PT_POINT &&
                contour.PointTypes().at(j) == PT_CUBIC && mode != PT_CUBIC)
                buffer += "C", mode = PT_CUBIC;
            else if(contour.PointTypes().at(j - 1) == PT_POINT &&
                contour.PointTypes().at(j) == PT_POINT && mode != PT_POINT)
                buffer += "L", mode = PT_POINT;
            buffer += Double2String(contour.Points().at(j).x) + "," +
                Double2String(contour.Points().at(j).y) + " ";
        }
        if(contour.isClosed()) buffer += "Z";
        return buffer;
    }

    // polygons.js/Polygons/generateSVG
    std::string Canva2SVG(Canva canva) {
        std::string buffer =
        "<svg xmlns=\"http://www.w3.org/2000/svg\" "
        "xmlns:xlink=\"http://www.w3.org/1999/xlink\" version=\"1.1\" "
        "baseProfile=\"full\" viewBox=\"0 0 200 200\" width=\"200\" "
        "height=\"200\">\n";
        for(auto i: canva.Contours()) {
            try {
                buffer +=
                    "<path d=\"" + Contour2SVGstr(i) + "\" fill=\"black\" />\n";
            } catch(const std::exception& e) {
                std::cerr << "An error occured when generating spline data in "
                             "SVG format, dropping: "
                          << e.what() << '\n';
            }
        }
        buffer += "</svg>\n";
        return buffer;
    }

    std::string Contour2SFDstr(Contour contour) {
        auto        firstPoint = contour.Points().at(0);
        std::string buffer     = Double2String(firstPoint.x * 5.25 - 25) + " " +
            Double2String(firstPoint.y * (-5.25) + 905) + " m 1\n";
        Point lastPoint;
        for(size_t j = 1; j < contour.Points().size();) {
            if(contour.PointTypes().at(j) == PT_POINT) {
                buffer += " " +
                    Double2String(contour.Points().at(j).x * 5.25 - 25) + " " +
                    Double2String(contour.Points().at(j).y * (-5.25) + 905) +
                    " l 1\n";
                lastPoint = contour.Points().at(j);
                j++;
            } else if(j + 1 < contour.Points().size() &&
                contour.PointTypes().at(j) == PT_QUAD) {
                auto p1   = contour.Points().at(j),
                     p    = contour.Points().at(j + 1);
                Point ps1 = lastPoint.GetExtendedDest(
                          p1, (p1 - lastPoint).GetLength() / 3),
                      ps2 = p.GetExtendedDest(p1, (p - p1).GetLength() / 3);
                buffer   += " " + Double2String(ps1.x * 5.25 - 25) + " " +
                    Double2String(ps1.y * (-5.25) + 905) + " " +
                    Double2String(ps2.x * 5.25 - 25) + " " +
                    Double2String(ps2.y * (-5.25) + 905) + " " +
                    Double2String(p.x * 5.25 - 25) + " " +
                    Double2String(p.y * (-5.25) + 905) + " c ";
                if(j + 2 < contour.PointTypes().size() &&
                    (contour.PointTypes().at(j + 2) == PT_QUAD ||
                        contour.PointTypes().at(j + 2) == PT_CUBIC))
                    buffer += "0\n";
                else
                    buffer += "1\n";
                lastPoint = contour.Points().at(j + 1);
                j        += 2;
            } else if(j + 2 < contour.Points().size() &&
                contour.PointTypes().at(j) == PT_CUBIC) {
                buffer += " " +
                    Double2String(contour.Points().at(j).x * 5.25 - 25) + " " +
                    Double2String(contour.Points().at(j).y * (-5.25) + 905) +
                    " " +
                    Double2String(contour.Points().at(j + 1).x * 5.25 - 25) +
                    " " +
                    Double2String(
                        contour.Points().at(j + 1).y * (-5.25) + 905) +
                    " " +
                    Double2String(contour.Points().at(j + 2).x * 5.25 - 25) +
                    " " +
                    Double2String(
                        contour.Points().at(j + 2).y * (-5.25) + 905) +
                    " c ";
                if(j + 3 < contour.PointTypes().size() &&
                    (contour.PointTypes().at(j + 3) == PT_QUAD ||
                        contour.PointTypes().at(j + 3) == PT_CUBIC))
                    buffer += "0\n";
                else
                    buffer += "1\n";
                lastPoint = contour.Points().at(j + 2);
                j        += 3;
            }
        }
        if(contour.isClosed() && lastPoint != firstPoint)
            buffer += " " + Double2String(firstPoint.x * 5.25 - 25) + " " +
                Double2String(firstPoint.y * (-5.25) + 905) + " l 1\n";
        return buffer;
    }

    std::string Canva2SFD(Canva canva) {
        std::string buffer = "SplineSet\n";
        for(auto i: canva.Contours()) {
            try {
                buffer += Contour2SFDstr(i);
            } catch(const std::exception& e) {
                std::cerr << "An error occured when generating spline data in "
                             "SFD format, dropping: "
                          << e.what() << '\n';
            }
        }
        buffer += "EndSplineSet\n";
        return buffer;
    }

} // namespace Kage
