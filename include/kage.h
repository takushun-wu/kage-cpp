#ifndef _KAGE_H
#define _KAGE_H

#include <functional>
#include <memory>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "canva.h"
#include "gwdata.h"
#include "kagefont.h"
#include "point.h"

namespace Kage {

    using DBsearchCallbackFunc = std::function<std::string(std::string)>;

    typedef enum: unsigned {
        CHECK_OK,
        CHECK_BUHIN_NOTFOUND,
        CHECK_LOOPBACK_DETECTED
    } CheckGlyphState;

    class Kage {
        std::unordered_map<std::string, std::vector<Stroke>> _kageDB;
        std::shared_ptr<KageFont>                            _pkFont = nullptr;
        double                                               _kRate;
        DBsearchCallbackFunc _dbSearchCallback = {};
        std::vector<Stroke>  _notDefGlyph      = {};
        static thread_local std::unordered_set<std::string> _glyphStack;

        std::vector<Stroke> GetStrokes(std::vector<Stroke> glyphData);
        std::vector<Stroke> GetStrokesOfBuhin(
            std::vector<Stroke> buhin, Point p1, Point p2, Point ps, Point ps2);

    public:
        Kage(KageFontType font = KAGEFONT_MINCHO, double size = 0);

        void SetBuhin(std::string name, std::vector<Stroke> data);
        void SetBuhin(std::string name, std::string data);
        void PushBuhin(std::string name, std::vector<Stroke> data);
        void PushBuhin(std::string name, std::string data);
        std::vector<Stroke> SearchBuhin(std::string name);

        void                SetFont(KageFontType type, double size = 0);
        void                SetDBsearchCallback(DBsearchCallbackFunc callback);
        void                SetNotDefGlyph(std::vector<Stroke> glyph);
        void                MakeGlyph(Canva& canva, std::string buhin);
        void                MakeGlyph2(Canva& canva, std::vector<Stroke> data);
        std::vector<Stroke> ExtractGlyph(std::string buhin);
        std::vector<Stroke> ExtractGlyph2(std::vector<Stroke> data);
        CheckGlyphState     CheckGlyph(std::string buhin);
        CheckGlyphState     CheckGlyph2(std::vector<Stroke> data);
    };

} // namespace Kage

#endif
