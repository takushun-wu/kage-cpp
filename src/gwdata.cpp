#include <cmath>
#include <cstdio>
#include <exception>

#include "gwdata.h"
#include "point.h"

namespace Kage {

    std::vector<std::string> StringSplit(std::string str, char32_t split) {
        std::u32string           strU32 = utf8::utf8to32(str), buffer = U"";
        std::vector<std::string> out;
        for(size_t i = 0; i < strU32.length() + 1; i++)
            if(i >= strU32.length() || strU32[i] == split) {
                out.push_back(utf8::utf32to8(buffer));
                buffer = U"";
            } else
                buffer += strU32[i];
        return out;
    }

    std::vector<std::string> StringSplit2(
        std::string str, char32_t split1, char32_t split2) {
        std::u32string           strU32 = utf8::utf8to32(str), buffer = U"";
        std::vector<std::string> out;
        for(size_t i = 0; i < strU32.length() + 1; i++)
            if(i >= strU32.length() || strU32[i] == split1 ||
                strU32[i] == split2) {
                out.push_back(utf8::utf32to8(buffer));
                buffer = U"";
            } else
                buffer += strU32[i];
        return out;
    }

    bool StrokeParse(std::string strokeStr, Stroke& stroke) {
        auto splited = StringSplit(strokeStr, U':');
        if(splited.size() < 4) return false;
        StrokeType strokeType;
        unsigned   typeOpt;
        // 确定笔画种类
        try {
            strokeType = (StrokeType)(std::stoull(splited[0]) % 100),
            typeOpt    = std::stoull(splited[0]) / 100;
        } catch(const std::exception& e) {
            return false;
        }
        stroke.type = strokeType, stroke.typeOpt = typeOpt;
        double sx, sy, x1, y1, x2, y2, sx2 = 0, sy2 = 0, x3 = 0, y3 = 0, x4 = 0,
                                       y4 = 0;
        int spe1, spe2, start, end, temp;
        switch(strokeType) {
        case STROKE_REFERENCE:
            // 99:<sx>:<sy>:<x1>:<y1>:<x2>:<y2>:<buhin>:0:<sx2>:<sy2>
            if(splited.size() < 8) return false;
            try {
                sx = std::stod(splited[1]), sy = std::stod(splited[2]),
                x1 = std::stod(splited[3]), y1 = std::stod(splited[4]),
                x2 = std::stod(splited[5]), y2 = std::stod(splited[6]);
            } catch(const std::exception& e) {
                return false;
            }
            try {
                if(splited.size() >= 10) sx2 = std::stod(splited[9]);
                if(splited.size() >= 11) sy2 = std::stod(splited[10]);
            } catch(const std::exception& e) {
                ;
            }
            stroke.buhin = splited[7], stroke.p1 = Point(x1, y1),
            stroke.p2 = Point(x2, y2), stroke.s1 = Point(sx, sy),
            stroke.s2 = Point(sx2, sy2);
            break;
        case STROKE_SPECIAL:
            // 0:<spe1>:<spe2>:<x1>:<y1>:<x2>:<y2>
            // Special case for blank character (0:0/0:-1)
            if(splited.size() < 2) return false;
            try {
                temp = std::stoi(splited[1]);
            } catch(const std::exception& e) {
                return false;
            }
            if(temp <= 0) {
                stroke.spe1 = (SpecialInt1)0, stroke.spe2 = (SpecialInt2)0;
                return true;
            }
            // Normal case
            if(splited.size() < 7) return false;
            try {
                spe1 = std::stoi(splited[1]), spe2 = std::stoi(splited[2]),
                x1 = std::stod(splited[3]), y1 = std::stod(splited[4]),
                x2 = std::stod(splited[5]), y2 = std::stod(splited[6]);
            } catch(const std::exception& e) {
                return false;
            }
            stroke.spe1 = (SpecialInt1)spe1, stroke.spe2 = (SpecialInt2)spe2,
            stroke.p1 = Point(x1, y1), stroke.p2 = Point(x2, y2);
            break;
        default:
            // <type>:<start>:<end>:<x1>:<y1>:<x2>:<y2>:<x3>:<y3>:<x4>:<y4>
            if(splited.size() < 7) return false;
            try {
                start = std::stoi(splited[1]), end = std::stoi(splited[2]),
                x1 = std::stod(splited[3]), y1 = std::stod(splited[4]),
                x2 = std::stod(splited[5]), y2 = std::stod(splited[6]);
            } catch(const std::exception& e) {
                return false;
            }
            if(start >= 0)
                stroke.start    = (StartType)(start % 100),
                stroke.startOpt = start / 100;
            else {
                int t           = start % 100;
                stroke.start    = (StartType)((100 + t) % 100);
                stroke.startOpt = start / 100 - (t < 0);
            } // Modified(2025/8/28)
            if((EndType)end == END_LOWER_LEFT_ZH_OLD ||
                (EndType)end == END_LOWER_LEFT_ZH_NEW)
                stroke.end = (EndType)end, stroke.endOpt = 0;
            else {
                if(end >= 0)
                    stroke.end    = (EndType)(end % 100),
                    stroke.endOpt = end / 100;
                else {
                    int t         = end % 100;
                    stroke.end    = (EndType)((100 + t) % 100);
                    stroke.endOpt = end / 100 - (t < 0);
                } // Modified(2025/8/28)
            }
            stroke.p1 = Point(x1, y1), stroke.p2 = Point(x2, y2);
            if(strokeType == STROKE_CURVE || strokeType == STROKE_BENDING ||
                strokeType == STROKE_BENDING_ROUND ||
                strokeType == STROKE_BEZIER || strokeType == STROKE_VCURVE ||
                (strokeType == STROKE_STRAIGHT &&
                    stroke.start == START_CONNECTING_MANUAL)) {
                if(splited.size() < 9) return false;
                try {
                    x3 = std::stod(splited[7]), y3 = std::stod(splited[8]);
                } catch(const std::exception& e) {
                    return false;
                }
                stroke.p3 = Point(x3, y3);
            }
            if(strokeType == STROKE_BEZIER || strokeType == STROKE_VCURVE) {
                if(splited.size() < 11) return false;
                try {
                    x4 = std::stod(splited[9]), y4 = std::stod(splited[10]);
                } catch(const std::exception& e) {
                    return false;
                }
                stroke.p4 = Point(x4, y4);
            }
            break;
        }
        return true;
    }

    std::string StrokeToString(Stroke stroke) {
        std::string out;
        out = Double2String(stroke.type) + ":";
        switch(stroke.type) {
        case STROKE_REFERENCE:
            // 99:<sx>:<sy>:<x1>:<y1>:<x2>:<y2>:<buhin>:0:<sx2>:<sy2>
            out += Double2String(stroke.s1.x) + ":" +
                Double2String(stroke.s1.y) + ":" + Double2String(stroke.p1.x) +
                ":" + Double2String(stroke.p1.y) + ":" +
                Double2String(stroke.p2.x) + ":" + Double2String(stroke.p2.y) +
                ":" + stroke.buhin;
            if(stroke.s2.isVaild())
                out += ":0:" + Double2String(stroke.s2.x) + ":" +
                    Double2String(stroke.s2.y);
            break;
        case STROKE_SPECIAL:
            // 0:<spe1>:<spe2>:<x1>:<y1>:<x2>:<y2>
            out += Double2String(stroke.spe1) + ":" +
                Double2String(stroke.spe2) + ":" + Double2String(stroke.p1.x) +
                ":" + Double2String(stroke.p1.y) + ":" +
                Double2String(stroke.p2.x) + ":" + Double2String(stroke.p2.y);
            break;
        default:
            // <type>:<start>:<end>:<x1>:<y1>:<x2>:<y2>:<x3>:<y3>:<x4>:<y4>
            out += Double2String(stroke.start + 100 * stroke.startOpt) + ":" +
                Double2String(stroke.end + 100 * stroke.endOpt) + ":" +
                Double2String(stroke.p1.x) + ":" + Double2String(stroke.p1.y) +
                ":" + Double2String(stroke.p2.x) + ":" +
                Double2String(stroke.p2.y);
            if(stroke.p3.isVaild() || stroke.p4.isVaild())
                out += ":" + Double2String(stroke.p3.x) + ":" +
                    Double2String(stroke.p3.y);
            if(stroke.p4.isVaild())
                out += ":" + Double2String(stroke.p4.x) + ":" +
                    Double2String(stroke.p4.y);
            break;
        }
        return out;
    }

    std::vector<Stroke> StrokesParse(std::string glyphData) {
        std::vector<Stroke> strokes;
        auto                textData = StringSplit2(glyphData, U'$', U'\n');
        for(auto i: textData) {
            Stroke stroke;
            if(StrokeParse(i, stroke)) strokes.push_back(stroke);
        }
        return strokes;
    }

    std::string StrokesToString(std::vector<Stroke> strokes, char32_t sep) {
        std::string out;
        for(auto it = strokes.begin(); it != strokes.end(); ++it) {
            out += StrokeToString(*it);
            if(it != strokes.end() - 1) out += sep;
        }
        return out;
    }

    // util.js/stretch
    double Stretch(double dp, double sp, double p, double min, double max) {
        double p1, p2, p3, p4;
        if(p < sp + 100)
            p1 = min, p3 = min, p2 = sp + 100, p4 = dp + 100;
        else
            p1 = sp + 100, p3 = dp + 100, p2 = max, p4 = max;
        return std::floor(((p - p1) / (p2 - p1)) * (p4 - p3) + p3);
    }

    // util.js/getBoundingBox
    BoundingData GetBoundingBox(std::vector<Stroke> strokes) {
        BoundingData a = {minX: 200, minY: 200, maxX: 0, maxY: 0};
        for(auto i: strokes) {
            if(i.type == STROKE_SPECIAL) continue;
            a.minX = std::min(a.minX, i.p1.x),
            a.maxX = std::max(a.maxX, i.p1.x);
            a.minY = std::min(a.minY, i.p1.y),
            a.maxY = std::max(a.maxY, i.p1.y);
            a.minX = std::min(a.minX, i.p2.x),
            a.maxX = std::max(a.maxX, i.p2.x);
            a.minY = std::min(a.minY, i.p2.y),
            a.maxY = std::max(a.maxY, i.p2.y);
            if(i.type == STROKE_STRAIGHT || i.type == STROKE_REFERENCE)
                continue;
            a.minX = std::min(a.minX, i.p3.x),
            a.maxX = std::max(a.maxX, i.p3.x);
            a.minY = std::min(a.minY, i.p3.y),
            a.maxY = std::max(a.maxY, i.p3.y);
            if(i.type == STROKE_CURVE || i.type == STROKE_BENDING ||
                i.type == STROKE_BENDING_ROUND)
                continue;
            a.minX = std::min(a.minX, i.p4.x),
            a.maxX = std::max(a.maxX, i.p4.x);
            a.minY = std::min(a.minY, i.p4.y),
            a.maxY = std::max(a.maxY, i.p4.y);
        }
        return a;
    }

    // 2d.js/getCrossPoint
    Point GetCrossPoint(Point p11, Point p12, Point p21, Point p22) {
        double a1 = p12.y - p11.y, b1 = p11.x - p12.x,
               c1 = -1 * a1 * p11.x - b1 * p11.y;
        double a2 = p22.y - p21.y, b2 = p21.x - p22.x,
               c2   = -1 * a2 * p21.x - b2 * p21.y;
        double temp = b1 * a2 - b2 * a1;
        if(temp == 0) // 平行状态下
            return UNINIT_POINT;
        return Point((c1 * b2 - c2 * b1) / temp, (a1 * c2 - a2 * c1) / temp);
    }

    // 2d.js/isCross
    bool isCross(Point p11, Point p12, Point p21, Point p22) {
        Point temp = GetCrossPoint(p11, p12, p21, p22);
        if(!temp.isVaild()) return false;
        if((p11.x < p12.x && (temp.x < p11.x || p12.x < temp.x)) ||
            (p11.x > p12.x && (temp.x < p12.x || p11.x < temp.x)) ||
            (p11.y < p12.y && (temp.y < p11.y || p12.y < temp.y)) ||
            (p11.y > p12.y && (temp.y < p12.y || p11.y < temp.y)))
            return false;
        if((p21.x < p22.x && (temp.x < p21.x || p22.x < temp.x)) ||
            (p21.x > p22.x && (temp.x < p22.x || p21.x < temp.x)) ||
            (p21.y < p22.y && (temp.y < p21.y || p22.y < temp.y)) ||
            (p21.y > p22.y && (temp.y < p22.y || p21.y < temp.y)))
            return false;
        return true;
    }

    // 2d.js/isCrossBox
    bool isCrossBox(Point p1, Point p2, Point pb1, Point pb2) {
        return isCross(p1, p2, pb1, Point(pb2.x, pb1.y)) ||
            isCross(p1, p2, Point(pb2.x, pb1.y), pb2) ||
            isCross(p1, p2, Point(pb1.x, pb2.y), pb2) ||
            isCross(p1, p2, pb1, Point(pb1.x, pb2.y));
    }

    // 2d.js/isCrossBoxWithOthers
    bool isCrossBoxWithOthers(
        std::vector<Stroke> strokesArray, size_t i, Point pb1, Point pb2) {
        for(size_t j = 0; j < strokesArray.size(); j++) {
            if(i == j) continue;
            switch(strokesArray[j].type) {
            case STROKE_BEZIER:
            case STROKE_VCURVE:
                if(isCrossBox(strokesArray[j].p3, strokesArray[j].p4, pb1, pb2))
                    return true;
            case STROKE_CURVE:
            case 12:
            case STROKE_BENDING:
            case STROKE_BENDING_ROUND:
                if(isCrossBox(strokesArray[j].p2, strokesArray[j].p3, pb1, pb2))
                    return true;
            case STROKE_STRAIGHT:
                if(isCrossBox(strokesArray[j].p1, strokesArray[j].p2, pb1, pb2))
                    return true;
            default: break;
            }
        }
        return false;
    }

    // 2d.js/isCrossWithOthers
    bool isCrossWithOthers(
        std::vector<Stroke> strokesArray, size_t i, Point pb1, Point pb2) {
        for(size_t j = 0; j < strokesArray.size(); j++) {
            if(i == j) continue;
            switch(strokesArray[j].type) {
            case STROKE_BEZIER:
            case STROKE_VCURVE:
                if(isCross(strokesArray[j].p3, strokesArray[j].p4, pb1, pb2))
                    return true;
            case STROKE_CURVE:
            case 12:
            case STROKE_BENDING:
            case STROKE_BENDING_ROUND:
                if(isCross(strokesArray[j].p2, strokesArray[j].p3, pb1, pb2))
                    return true;
            case STROKE_STRAIGHT:
                if(isCross(strokesArray[j].p1, strokesArray[j].p2, pb1, pb2))
                    return true;
            default: break;
            }
        }
        return false;
    }

} // namespace Kage
