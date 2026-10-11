#include "kagefont.h"

namespace Kage {

    KageFont::KageFont(double size) {
        _size = size;
    }

    KageFontType KageFont::GetType() {
        return _type;
    }

    std::vector<Canva> KageFont::DrawGlyphSeparated(
        std::vector<Stroke> strokes) {
        std::vector<Canva> result;
        for(auto& stroke: strokes)
            result.push_back(DrawGlyph(std::vector<Stroke>{stroke}));
        return result;
    }

} // namespace Kage
