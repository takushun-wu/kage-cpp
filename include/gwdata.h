#ifndef _GWDATA_H
#define _GWDATA_H

#include <string>
#include <vector>

#include "utf8.h"

#include "point.h"

namespace Kage {

    typedef enum: unsigned {
        STROKE_SPECIAL,       // 特殊指令
        STROKE_STRAIGHT,      // 直线
        STROKE_CURVE,         // 二次曲线
        STROKE_BENDING,       // 折线（札、己）
        STROKE_BENDING_ROUND, // 折弯（乙）
        STROKE_BEZIER = 6,    // 三次曲线
        STROKE_VCURVE,        // 竖撇
        STROKE_REFERENCE = 99 // 引用部件
    } StrokeType;

    typedef enum: unsigned {
        SPECIAL1_FLIP_UPDOWN    = 97,
        SPECIAL1_FLIP_LEFTRIGHT = 98,
        SPECIAL1_ROTATE         = 99
    } SpecialInt1;

    typedef enum: unsigned {
        SPECIAL2_ROTATE_90  = 1,
        SPECIAL2_ROTATE_180 = 2,
        SPECIAL2_ROTATE_270 = 3
    } SpecialInt2;

    typedef enum: unsigned {
        START_OPEN,                    // 开放
        START_CONNECTING_H       = 2,  // 横笔连接
        START_UPPER_LEFT_CORNER  = 12, // 左上角
        START_UPPER_RIGHT_CORNER = 22, // 右上角
        START_CONNECTING_V       = 32, // 竖笔连接
        START_THIN               = 7,  // 尖端（㇏）
        START_ROOFED_THIN        = 27, // 横捺角（旧字形“入”）
        // 原版kage引擎内部使用
        START_INTERNAL_FOR_BENDING = 6,
        START_INTERNAL_PLANE       = 1,
        // ge9自定义
        START_CONNECT_THIN      = 37,
        START_CONNECTING_MANUAL = 39
        // 40-60 is used for curve strokes
    } StartType;

    typedef enum: unsigned {
        END_OPEN,                     // 开放
        END_CONNECTING_H       = 2,   // 横笔连接
        END_CONNECTING_V       = 32,  // 竖笔连接
        END_LOWER_LEFT_CORNER  = 13,  // 左下角
        END_LOWER_RIGHT_CORNER = 23,  // 右下角
        END_LOWER_LEFT_ZH_OLD  = 313, // 旧字形左下角
        END_LOWER_LEFT_ZH_NEW  = 413, // 新字形左下角
        END_TURN_LEFT          = 4,   // 竖钩（←）
        END_LOWER_RIGHT_HT     = 24,  // 台标右下角
        END_TURN_UPWARDS       = 5,   // 弯钩/斜勾（↑）
        END_LEFT_SWEEP         = 7,   // 尖端（㇒）
        END_STOP,                     // 点（㇔）
        // 原版kage引擎内部使用
        END_INTERNAL_LAST_FILLED_CIRCLE = 1,
        END_INTERNAL_TURN_LEFT          = 14,
        END_INTERNAL_TURN_UPWARDS
    } EndType;

    typedef struct {
        StrokeType type;
        StartType  start;
        EndType    end;
        Point      p1, p2, p3, p4;
        signed     typeOpt, startOpt, endOpt;
        // 引用部件专属
        std::string buhin;
        Point       s1, s2; // 拉伸参数
        // 特殊命令（翻转、旋转）
        SpecialInt1 spe1;
        SpecialInt2 spe2;
    } Stroke;

    typedef struct {
        double minX;
        double minY;
        double maxX;
        double maxY;
    } BoundingData;

    std::vector<Stroke> StrokesParse(std::string strokeStr);
    std::string         StrokesToString(
                std::vector<Stroke> strokes, char32_t sep = U'$');
    double Stretch(double dp, double sp, double p, double min, double max);
    BoundingData GetBoundingBox(std::vector<Stroke> strokes);
    bool         isCrossBoxWithOthers(
                std::vector<Stroke> strokesArray, size_t i, Point pb1, Point pb2);
    bool isCrossWithOthers(
        std::vector<Stroke> strokesArray, size_t i, Point pb1, Point pb2);

} // namespace Kage

#endif
