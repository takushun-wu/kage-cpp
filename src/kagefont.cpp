#include "kagefont.h"

namespace Kage {

    KageFont::KageFont(double size) {
        _size = size;
    }

    KageFontType KageFont::GetType() {
        return _type;
    }

} // namespace Kage
