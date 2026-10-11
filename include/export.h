#ifndef _EXPORT_H
#define _EXPORT_H

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "canva.h"

namespace Kage {

    struct SFDGlyph {
        std::string name;
        int32_t     code;
        std::string spline;
    };

    std::string Canva2SVG(Canva canva);
    std::string Canva2SFD(Canva canva);
    std::string SFDHeader(const std::string& jobname, size_t chars);
    std::string BuildSFD(
        const std::string& jobname, const std::vector<SFDGlyph>& glyphs);

} // namespace Kage

#endif
