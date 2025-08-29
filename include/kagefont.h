#ifndef _KAGEFONT_H
#define _KAGEFONT_H

#include <vector>

#include "canva.h"
#include "gwdata.h"

namespace Kage {

    typedef enum {
        KAGEFONT_DEFAULT,
        KAGEFONT_MINCHO,
        KAGEFONT_GOTHIC
    } KageFontType;

    class KageFont {
    protected:
        KageFontType _type = KAGEFONT_DEFAULT;
        double       _size;

    public:
        KageFont(double size = 0);
        virtual ~KageFont()                                  = default;
        virtual Canva DrawGlyph(std::vector<Stroke> strokes) = 0;

        KageFontType GetType();
    };

} // namespace Kage

#endif
