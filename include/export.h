#ifndef _EXPORT_H
#define _EXPORT_H

#include <string>

#include "canva.h"

namespace Kage {

    std::string Canva2SVG(Canva canva);
    std::string Canva2SFD(Canva canva);

} // namespace Kage

#endif
