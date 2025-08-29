#include "kage.h"

#include "gothic.h"
#include "mincho.h"

namespace Kage {

    thread_local std::unordered_set<std::string> Kage::_glyphStack;

    // kage.js/Kage/constructor
    Kage::Kage(KageFontType font, double size) {
        SetFont(font, size);
        _kRate = 100;
    }

    // kage.js/Kage/getStrokes
    std::vector<Stroke> Kage::GetStrokes(std::vector<Stroke> glyphData) {
        std::vector<Stroke> strokes;
        for(auto stroke: glyphData) {
            if(stroke.type != STROKE_REFERENCE)
                strokes.push_back(stroke);
            else {
                std::vector<Stroke> buhin = SearchBuhin(stroke.buhin), temp;
                if(!buhin.empty())
                    temp = GetStrokesOfBuhin(
                        buhin, stroke.p1, stroke.p2, stroke.s1, stroke.s2);
                else
                    temp = _notDefGlyph;
                for(auto t: temp)
                    strokes.push_back(t);
            }
        }
        return strokes;
    }

    // kage.js/Kage/getStrokesOfBuhin
    std::vector<Stroke> Kage::GetStrokesOfBuhin(
        std::vector<Stroke> buhin, Point p1, Point p2, Point ps, Point ps2) {
        auto                temp = GetStrokes(buhin);
        std::vector<Stroke> result;
        auto                box = GetBoundingBox(temp);
        if(ps.x != 0 || ps.y != 0) {
            if(ps.x > 100)
                ps.x -= 200;
            else
                ps2 = Point(0, 0);
        }
        for(auto& t: temp) {
            if(ps.x != 0 || ps.y != 0) {
                t.p1.x = Stretch(ps.x, ps2.x, t.p1.x, box.minX, box.maxX),
                t.p1.y = Stretch(ps.y, ps2.y, t.p1.y, box.minY, box.maxY),
                t.p2.x = Stretch(ps.x, ps2.x, t.p2.x, box.minX, box.maxX),
                t.p2.y = Stretch(ps.y, ps2.y, t.p2.y, box.minY, box.maxY);
                if(t.type != STROKE_REFERENCE)
                    t.p3.x = Stretch(ps.x, ps2.x, t.p3.x, box.minX, box.maxX),
                    t.p3.y = Stretch(ps.y, ps2.y, t.p3.y, box.minY, box.maxY),
                    t.p4.x = Stretch(ps.x, ps2.x, t.p4.x, box.minX, box.maxX),
                    t.p4.y = Stretch(ps.y, ps2.y, t.p4.y, box.minY, box.maxY);
            }
            t.p1.x = p1.x + t.p1.x * (p2.x - p1.x) / 200,
            t.p1.y = p1.y + t.p1.y * (p2.y - p1.y) / 200,
            t.p2.x = p1.x + t.p2.x * (p2.x - p1.x) / 200,
            t.p2.y = p1.y + t.p2.y * (p2.y - p1.y) / 200,
            t.p3.x = p1.x + t.p3.x * (p2.x - p1.x) / 200,
            t.p3.y = p1.y + t.p3.y * (p2.y - p1.y) / 200,
            t.p4.x = p1.x + t.p4.x * (p2.x - p1.x) / 200,
            t.p4.y = p1.y + t.p4.y * (p2.y - p1.y) / 200;
            result.push_back(t);
        }
        return result;
    }

    // buhin.js/Buhin/set
    void Kage::SetBuhin(std::string name, std::vector<Stroke> data) {
        _kageDB[name] = data;
    }

    void Kage::SetBuhin(std::string name, std::string data) {
        SetBuhin(name, StrokesParse(data));
    }

    // buhin.js/Buhin/push
    void Kage::PushBuhin(std::string name, std::vector<Stroke> data) {
        _kageDB.insert({name, data});
    }

    void Kage::PushBuhin(std::string name, std::string data) {
        PushBuhin(name, StrokesParse(data));
    }

    // buhin.js/Buhin/search
    std::vector<Stroke> Kage::SearchBuhin(std::string name) {
        if(_kageDB.find(name) != _kageDB.end())
            return _kageDB.at(name);
        else if(_dbSearchCallback) {
            return StrokesParse(_dbSearchCallback(name));
        } else
            return {};
    }

    // kage.js/Kage/setFont
    void Kage::SetFont(KageFontType type, double size) {
        switch(type) {
        case KAGEFONT_GOTHIC: _pkFont.reset(new Gothic(size)); break;
        case KAGEFONT_MINCHO: _pkFont.reset(new Mincho(size)); break;
        default:              _pkFont.reset(new Mincho(size)); break;
        }
    }

    void Kage::SetDBsearchCallback(DBsearchCallbackFunc callback) {
        _dbSearchCallback = callback;
    }

    void Kage::SetNotDefGlyph(std::vector<Stroke> glyph) {
        _notDefGlyph = glyph;
    }

    // kage.js/Kage/makeGlyph
    // The word "buhin" means "component".  This method converts buhin (KAGE
    // data format) to polygons (path data).  The variable buhin may represent a
    // component of kanji or a kanji itself.
    void Kage::MakeGlyph(Canva& canva, std::string buhin) {
        auto glyphData = SearchBuhin(buhin);
        if(glyphData.empty()) glyphData = _notDefGlyph;
        MakeGlyph2(canva, glyphData);
    }

    // kage.js/Kage/makeGlyph2
    void Kage::MakeGlyph2(Canva& canva, std::vector<Stroke> data) {
        auto kageStrokes = GetStrokes(data);
        canva.Concat(_pkFont->DrawGlyph(kageStrokes));
    }

    std::vector<Stroke> Kage::ExtractGlyph(std::string buhin) {
        return ExtractGlyph2(SearchBuhin(buhin));
    }

    std::vector<Stroke> Kage::ExtractGlyph2(std::vector<Stroke> data) {
        return GetStrokes(data);
    }

    CheckGlyphState Kage::CheckGlyph(std::string buhin) {
        if(_glyphStack.find(buhin) != _glyphStack.end())
            return CHECK_LOOPBACK_DETECTED;
        _glyphStack.insert(buhin);
        auto            glyphData = SearchBuhin(buhin);
        CheckGlyphState out;
        if(glyphData.empty())
            out = CHECK_BUHIN_NOTFOUND;
        else
            out = CheckGlyph2(glyphData);
        _glyphStack.erase(buhin);
        return out;
    }

    CheckGlyphState Kage::CheckGlyph2(std::vector<Stroke> data) {
        for(auto stroke: data) {
            if(stroke.type == STROKE_REFERENCE) {
                CheckGlyphState out = CheckGlyph(stroke.buhin);
                if(out != CHECK_OK) return out;
            }
        }
        return CHECK_OK;
    }

} // namespace Kage
