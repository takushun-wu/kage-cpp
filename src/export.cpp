#include "export.h"

#include <chrono>
#include <cstdio>
#include <ctime>
#include <iostream>

#ifdef _WIN32
#    include <windows.h>
#else
#    include <pwd.h>
#    include <unistd.h>
#endif

#include "point.h"

namespace Kage {

    namespace {

#ifdef _WIN32
        std::string WideToUTF8(const WCHAR* value) {
            int size = WideCharToMultiByte(
                CP_UTF8, 0, value, -1, nullptr, 0, nullptr, nullptr);
            if(size <= 1) return "Unknown";
            std::string out(size, '\0');
            WideCharToMultiByte(
                CP_UTF8, 0, value, -1, &out[0], size, nullptr, nullptr);
            if(!out.empty() && out.back() == '\0') out.pop_back();
            return out.empty() ? "Unknown" : out;
        }

        std::string GetUserNameUTF8() {
            WCHAR buffer[256];
            DWORD length = sizeof(buffer) / sizeof(buffer[0]);
            if(GetUserNameW(buffer, &length)) return WideToUTF8(buffer);
            return "Unknown";
        }

        std::string GetDeviceNameUTF8() {
            WCHAR buffer[256];
            DWORD length = sizeof(buffer) / sizeof(buffer[0]);
            if(GetComputerNameW(buffer, &length)) return WideToUTF8(buffer);
            return "Unknown";
        }
#else
        std::string GetUserNameUTF8() {
            struct passwd* pw = getpwuid(getuid());
            return pw ? pw->pw_name : "Unknown";
        }

        std::string GetDeviceNameUTF8() {
            char hostname[256];
            if(gethostname(hostname, sizeof(hostname)) == 0)
                return hostname;
            return "Unknown";
        }
#endif

    } // namespace

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

    std::string SFDHeader(const std::string& jobname, size_t chars) {
        std::string out;
        auto        now      = std::chrono::system_clock::now();
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);
        std::tm*    local_time = std::localtime(&now_time);
        auto        epoch =
            std::chrono::duration_cast<std::chrono::seconds>(
                now.time_since_epoch())
                .count();
        int year = local_time ? 1900 + local_time->tm_year : 1970;

        out += "SplineFontDB: 3.2\n";
        out += "FontName: " + jobname + "\n";
        out += "FullName: " + jobname + "\n";
        out += "FamilyName: " + jobname + "\n";
        out += "Weight: Regular\n";
        out += "Copyright: Copyright (c) " + std::to_string(year) + ", " +
            GetUserNameUTF8() + "@" + GetDeviceNameUTF8() + "\n";
        out += "Version: 001.000\n";
        out += "Ascent: 880\n";
        out += "Descent: 120\n";
        out += "LayerCount: 2\n";
        out += "Layer: 0 0 \"Back\" 1\n";
        out += "Layer: 1 0 \"Fore\" 0\n";
        out += "CreationTime: " + std::to_string(epoch) + "\n";
        out += "ModificationTime: " + std::to_string(epoch) + "\n";
        out += "OS2Vendor: 'PfEd'\n";
        out += "Encoding: Original\n";
        out += "Grid\n";
        out += "42 838 m 1\n";
        out += " 958 838 l 1\n";
        out += " 958 -78 l 1\n";
        out += " 42 -78 l 1\n";
        out += " 42 838 l 1\n";
        out += "500 880 m 1\n";
        out += " 500 -120 l 1025\n";
        out += "0 380 m 1\n";
        out += " 1000 380 l 1025\n";
        out += "EndSplineSet\n";
        out += "BeginChars: " + std::to_string(chars) + " " +
            std::to_string(chars) + "\n\n";
        return out;
    }

    std::string BuildSFD(
        const std::string& jobname, const std::vector<SFDGlyph>& glyphs) {
        std::string out = SFDHeader(jobname, glyphs.size());
        size_t      charCount = 0;
        for(const auto& glyph: glyphs) {
            out += "StartChar: " + glyph.name + "\n";
            out += "Encoding: " + std::to_string(charCount) + " " +
                std::to_string(glyph.code) + " " +
                std::to_string(charCount) + " \n";
            out += "Width: 1000\n";
            out += "Flags: HW\n";
            out += "LayerCount: 2\n";
            out += "Fore\n";
            out += glyph.spline;
            out += "EndChar\n\n";
            charCount++;
        }
        out += "EndChars\n";
        out += "EndSplineFont\n";
        return out;
    }

} // namespace Kage
