#include <map>
#include <mutex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <pybind11/gil.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "export.h"
#include "kage.h"

namespace py = pybind11;

namespace {

    const char* fontName(Kage::KageFontType font) {
        switch(font) {
        case Kage::KAGEFONT_DEFAULT: return "DEFAULT";
        case Kage::KAGEFONT_MINCHO: return "MINCHO";
        case Kage::KAGEFONT_GOTHIC: return "GOTHIC";
        }
        return "UNKNOWN";
    }
    const char* strokeTypeName(Kage::StrokeType type) {
        switch(type) {
        case Kage::STROKE_SPECIAL: return "SPECIAL";
        case Kage::STROKE_STRAIGHT: return "STRAIGHT";
        case Kage::STROKE_CURVE: return "CURVE";
        case Kage::STROKE_BENDING: return "BENDING";
        case Kage::STROKE_BENDING_ROUND: return "BENDING_ROUND";
        case Kage::STROKE_BEZIER: return "BEZIER";
        case Kage::STROKE_VCURVE: return "VCURVE";
        case Kage::STROKE_REFERENCE: return "REFERENCE";
        }
        return "UNKNOWN";
    }

    const char* startTypeName(Kage::StartType start) {
        switch(start) {
        case Kage::START_OPEN: return "OPEN";
        case Kage::START_CONNECTING_H: return "CONNECTING_H";
        case Kage::START_UPPER_LEFT_CORNER: return "UPPER_LEFT_CORNER";
        case Kage::START_UPPER_RIGHT_CORNER: return "UPPER_RIGHT_CORNER";
        case Kage::START_CONNECTING_V: return "CONNECTING_V";
        case Kage::START_THIN: return "THIN";
        case Kage::START_ROOFED_THIN: return "ROOFED_THIN";
        case Kage::START_INTERNAL_FOR_BENDING: return "INTERNAL_FOR_BENDING";
        case Kage::START_INTERNAL_PLANE: return "INTERNAL_PLANE";
        case Kage::START_CONNECT_THIN: return "CONNECT_THIN";
        case Kage::START_CONNECTING_MANUAL: return "CONNECTING_MANUAL";
        }
        return "UNKNOWN";
    }

    const char* endTypeName(Kage::EndType end) {
        switch(end) {
        case Kage::END_OPEN: return "OPEN";
        case Kage::END_CONNECTING_H: return "CONNECTING_H";
        case Kage::END_CONNECTING_V: return "CONNECTING_V";
        case Kage::END_LOWER_LEFT_CORNER: return "LOWER_LEFT_CORNER";
        case Kage::END_LOWER_RIGHT_CORNER: return "LOWER_RIGHT_CORNER";
        case Kage::END_LOWER_LEFT_ZH_OLD: return "LOWER_LEFT_ZH_OLD";
        case Kage::END_LOWER_LEFT_ZH_NEW: return "LOWER_LEFT_ZH_NEW";
        case Kage::END_TURN_LEFT: return "TURN_LEFT";
        case Kage::END_LOWER_RIGHT_HT: return "LOWER_RIGHT_HT";
        case Kage::END_TURN_UPWARDS: return "TURN_UPWARDS";
        case Kage::END_LEFT_SWEEP: return "LEFT_SWEEP";
        case Kage::END_STOP: return "STOP";
        case Kage::END_INTERNAL_LAST_FILLED_CIRCLE:
            return "INTERNAL_LAST_FILLED_CIRCLE";
        case Kage::END_INTERNAL_TURN_LEFT: return "INTERNAL_TURN_LEFT";
        case Kage::END_INTERNAL_TURN_UPWARDS: return "INTERNAL_TURN_UPWARDS";
        }
        return "UNKNOWN";
    }

    const char* specialInt1Name(Kage::SpecialInt1 special) {
        switch(special) {
        case Kage::SPECIAL1_FLIP_UPDOWN: return "FLIP_UPDOWN";
        case Kage::SPECIAL1_FLIP_LEFTRIGHT: return "FLIP_LEFTRIGHT";
        case Kage::SPECIAL1_ROTATE: return "ROTATE";
        default: return "NONE";
        }
    }

    const char* specialInt2Name(Kage::SpecialInt2 special) {
        switch(special) {
        case Kage::SPECIAL2_ROTATE_90: return "ROTATE_90";
        case Kage::SPECIAL2_ROTATE_180: return "ROTATE_180";
        case Kage::SPECIAL2_ROTATE_270: return "ROTATE_270";
        default: return "NONE";
        }
    }

    std::string enumRepr(const char* name, unsigned value) {
        std::ostringstream output;
        output << name << "(" << value << ")";
        return output.str();
    }
    std::string pointRepr(const Kage::Point& point) {
        std::ostringstream output;
        output << "Point(x=" << point.x << ", y=" << point.y << ")";
        return output.str();
    }

    std::string strokeRepr(const Kage::Stroke& stroke) {
        std::ostringstream output;
        output << "Stroke(type=" << strokeTypeName(stroke.type);

        if(stroke.type == Kage::STROKE_REFERENCE) {
            output << ", buhin='" << stroke.buhin
                   << "', p1=" << pointRepr(stroke.p1)
                   << ", p2=" << pointRepr(stroke.p2)
                   << ", stretch1=" << pointRepr(stroke.s1)
                   << ", stretch2=" << pointRepr(stroke.s2);
        } else if(stroke.type == Kage::STROKE_SPECIAL) {
            output << ", special1=" << enumRepr(
                       specialInt1Name(stroke.spe1),
                       static_cast<unsigned>(stroke.spe1))
                   << ", special2=" << enumRepr(
                       specialInt2Name(stroke.spe2),
                       static_cast<unsigned>(stroke.spe2));
            if(stroke.spe1 != static_cast<Kage::SpecialInt1>(0)) {
                output << ", p1=" << pointRepr(stroke.p1)
                       << ", p2=" << pointRepr(stroke.p2);
            }
        } else {
            output << ", start=" << enumRepr(
                       startTypeName(stroke.start),
                       static_cast<unsigned>(stroke.start))
                   << ", end=" << enumRepr(
                       endTypeName(stroke.end),
                       static_cast<unsigned>(stroke.end))
                   << ", type_opt=" << stroke.typeOpt
                   << ", start_opt=" << stroke.startOpt
                   << ", end_opt=" << stroke.endOpt
                   << ", p1=" << pointRepr(stroke.p1)
                   << ", p2=" << pointRepr(stroke.p2);

            const bool hasP3 =
                stroke.type == Kage::STROKE_CURVE ||
                stroke.type == Kage::STROKE_BENDING ||
                stroke.type == Kage::STROKE_BENDING_ROUND ||
                stroke.type == Kage::STROKE_BEZIER ||
                stroke.type == Kage::STROKE_VCURVE ||
                (stroke.type == Kage::STROKE_STRAIGHT &&
                    stroke.start == Kage::START_CONNECTING_MANUAL);
            if(hasP3) output << ", p3=" << pointRepr(stroke.p3);
            if(stroke.type == Kage::STROKE_BEZIER ||
                stroke.type == Kage::STROKE_VCURVE)
                output << ", p4=" << pointRepr(stroke.p4);
        }

        output << ")";
        return output.str();
    }
    std::string boundingDataRepr(const Kage::BoundingData& boundingData) {
        std::ostringstream output;
        output << "BoundingData(min_x=" << boundingData.minX
               << ", min_y=" << boundingData.minY
               << ", max_x=" << boundingData.maxX
               << ", max_y=" << boundingData.maxY << ")";
        return output.str();
    }

    std::string contourRepr(Kage::Contour& contour) {
        std::ostringstream output;
        output << "Contour(points=" << contour.Size()
               << ", closed=" << (contour.isClosed() ? "True" : "False") << ")";
        return output.str();
    }

    std::string canvaRepr(Kage::Canva& canva) {
        std::ostringstream output;
        output << "Canva(contours=" << canva.Size() << ")";
        return output.str();
    }

    class Engine {
        Kage::Kage           _engine;
        std::recursive_mutex _mutex;
        py::object           _dbSearchCallback;
        Kage::KageFontType   _font;
        double               _size;

        Kage::Canva renderStrokes(const std::vector<Kage::Stroke>& strokes) {
            py::gil_scoped_release                release;
            std::lock_guard<std::recursive_mutex> lock(_mutex);
            Kage::Canva                           canva;
            _engine.MakeGlyph2(canva, strokes);
            return canva;
        }

        std::string canvaToSVG(Kage::Canva canva) const {
            py::gil_scoped_release release;
            return Kage::Canva2SVG(canva);
        }

        std::string canvaToSFD(Kage::Canva canva) const {
            py::gil_scoped_release release;
            return Kage::Canva2SFD(canva);
        }

    public:
        explicit Engine(
            Kage::KageFontType font = Kage::KAGEFONT_MINCHO, double size = 0):
            _engine(font, size), _font(font), _size(size) {}

        void setBuhin(const std::string& name, const std::string& data) {
            std::lock_guard<std::recursive_mutex> lock(_mutex);
            _engine.SetBuhin(name, data);
        }

        void setBuhinStrokes(
            const std::string& name, const std::vector<Kage::Stroke>& strokes) {
            std::lock_guard<std::recursive_mutex> lock(_mutex);
            _engine.SetBuhin(name, strokes);
        }

        void pushBuhin(const std::string& name, const std::string& data) {
            std::lock_guard<std::recursive_mutex> lock(_mutex);
            _engine.PushBuhin(name, data);
        }

        void pushBuhinStrokes(
            const std::string& name, const std::vector<Kage::Stroke>& strokes) {
            std::lock_guard<std::recursive_mutex> lock(_mutex);
            _engine.PushBuhin(name, strokes);
        }

        std::vector<Kage::Stroke> searchBuhin(const std::string& name) {
            std::lock_guard<std::recursive_mutex> lock(_mutex);
            return _engine.SearchBuhin(name);
        }

        bool deleteBuhin(const std::string& name) {
            std::lock_guard<std::recursive_mutex> lock(_mutex);
            return _engine.DeleteBuhin(name);
        }

        std::map<std::string, std::string> exportBuhins() {
            std::lock_guard<std::recursive_mutex> lock(_mutex);
            return _engine.ExportBuhins();
        }

        void setNotDefGlyph(const std::string& data) {
            std::lock_guard<std::recursive_mutex> lock(_mutex);
            _engine.SetNotDefGlyph(Kage::StrokesParse(data));
        }

        void setNotDefGlyphStrokes(const std::vector<Kage::Stroke>& strokes) {
            std::lock_guard<std::recursive_mutex> lock(_mutex);
            _engine.SetNotDefGlyph(strokes);
        }

        void setFont(Kage::KageFontType font, double size = 0) {
            std::lock_guard<std::recursive_mutex> lock(_mutex);
            _engine.SetFont(font, size);
            _font = font;
            _size = size;
        }

        void setDBSearchCallback(py::object callback) {
            std::lock_guard<std::recursive_mutex> lock(_mutex);
            if(callback.is_none()) {
                _dbSearchCallback = py::none();
                _engine.SetDBsearchCallback(Kage::DBsearchCallbackFunc());
                return;
            }
            if(!PyCallable_Check(callback.ptr()))
                throw std::invalid_argument(
                    "db search callback must be callable");

            _dbSearchCallback = std::move(callback);
            _engine.SetDBsearchCallback([this](std::string name) {
                py::gil_scoped_acquire acquire;
                py::object result = _dbSearchCallback(py::str(name));
                if(result.is_none()) return std::string();
                return result.cast<std::string>();
            });
        }

        Kage::Canva render(const std::string& name) {
            py::gil_scoped_release                release;
            std::lock_guard<std::recursive_mutex> lock(_mutex);
            Kage::Canva                           canva;
            _engine.MakeGlyph(canva, name);
            return canva;
        }

        Kage::Canva renderData(const std::string& data) {
            return renderStrokes(Kage::StrokesParse(data));
        }

        std::vector<std::vector<Kage::Canva>> renderDataSeparated(const std::string& data) {
            auto                                  strokes = Kage::StrokesParse(data);
            py::gil_scoped_release                release;
            std::lock_guard<std::recursive_mutex> lock(_mutex);
            return _engine.MakeGlyphSeparatedOut(strokes);
        }

        std::string renderSVG(const std::string& name) {
            return canvaToSVG(render(name));
        }

        std::string renderDataSVG(const std::string& data) {
            return canvaToSVG(renderData(data));
        }

        std::string renderSFD(const std::string& name) {
            return canvaToSFD(render(name));
        }

        std::string renderDataSFD(const std::string& data) {
            return canvaToSFD(renderData(data));
        }

        std::vector<Kage::Stroke> extract(const std::string& name) {
            py::gil_scoped_release                release;
            std::lock_guard<std::recursive_mutex> lock(_mutex);
            return _engine.ExtractGlyph(name);
        }

        std::vector<Kage::Stroke> extractData(const std::string& data) {
            auto                   strokes = Kage::StrokesParse(data);
            py::gil_scoped_release release;
            std::lock_guard<std::recursive_mutex> lock(_mutex);
            return _engine.ExtractGlyph2(strokes);
        }

        Kage::CheckGlyphState check(const std::string& name) {
            py::gil_scoped_release                release;
            std::lock_guard<std::recursive_mutex> lock(_mutex);
            return _engine.CheckGlyph(name);
        }

        Kage::CheckGlyphState checkData(const std::string& data) {
            auto                   strokes = Kage::StrokesParse(data);
            py::gil_scoped_release release;
            std::lock_guard<std::recursive_mutex> lock(_mutex);
            return _engine.CheckGlyph2(strokes);
        }

        std::string repr() {
            std::lock_guard<std::recursive_mutex> lock(_mutex);
            std::ostringstream output;
            output << "Engine(font=" << fontName(_font) << ", size=" << _size << ")";
            return output.str();
        }
    };

} // namespace

PYBIND11_MODULE(kage_cpp, module) {
    module.doc() = "Python bindings for the kage-cpp engine";

    py::enum_<Kage::KageFontType>(module, "FontType")
        .value("DEFAULT", Kage::KAGEFONT_DEFAULT)
        .value("MINCHO", Kage::KAGEFONT_MINCHO)
        .value("GOTHIC", Kage::KAGEFONT_GOTHIC)
        .export_values();

    py::enum_<Kage::CheckGlyphState>(module, "CheckGlyphState")
        .value("OK", Kage::CHECK_OK)
        .value("BUHIN_NOTFOUND", Kage::CHECK_BUHIN_NOTFOUND)
        .value("LOOPBACK_DETECTED", Kage::CHECK_LOOPBACK_DETECTED)
        .export_values();

    py::enum_<Kage::StrokeType>(module, "StrokeType")
        .value("SPECIAL", Kage::STROKE_SPECIAL)
        .value("STRAIGHT", Kage::STROKE_STRAIGHT)
        .value("CURVE", Kage::STROKE_CURVE)
        .value("BENDING", Kage::STROKE_BENDING)
        .value("BENDING_ROUND", Kage::STROKE_BENDING_ROUND)
        .value("BEZIER", Kage::STROKE_BEZIER)
        .value("VCURVE", Kage::STROKE_VCURVE)
        .value("REFERENCE", Kage::STROKE_REFERENCE)
        .export_values();

    py::enum_<Kage::SpecialInt1>(module, "SpecialInt1")
        .value("FLIP_UPDOWN", Kage::SPECIAL1_FLIP_UPDOWN)
        .value("FLIP_LEFTRIGHT", Kage::SPECIAL1_FLIP_LEFTRIGHT)
        .value("ROTATE", Kage::SPECIAL1_ROTATE)
        .export_values();

    py::enum_<Kage::SpecialInt2>(module, "SpecialInt2")
        .value("ROTATE_90", Kage::SPECIAL2_ROTATE_90)
        .value("ROTATE_180", Kage::SPECIAL2_ROTATE_180)
        .value("ROTATE_270", Kage::SPECIAL2_ROTATE_270)
        .export_values();

    py::enum_<Kage::StartType>(module, "StartType")
        .value("OPEN", Kage::START_OPEN)
        .value("CONNECTING_H", Kage::START_CONNECTING_H)
        .value("UPPER_LEFT_CORNER", Kage::START_UPPER_LEFT_CORNER)
        .value("UPPER_RIGHT_CORNER", Kage::START_UPPER_RIGHT_CORNER)
        .value("CONNECTING_V", Kage::START_CONNECTING_V)
        .value("THIN", Kage::START_THIN)
        .value("ROOFED_THIN", Kage::START_ROOFED_THIN)
        .value("INTERNAL_FOR_BENDING", Kage::START_INTERNAL_FOR_BENDING)
        .value("INTERNAL_PLANE", Kage::START_INTERNAL_PLANE)
        .value("CONNECT_THIN", Kage::START_CONNECT_THIN)
        .value("CONNECTING_MANUAL", Kage::START_CONNECTING_MANUAL)
        .export_values();

    py::enum_<Kage::EndType>(module, "EndType")
        .value("OPEN", Kage::END_OPEN)
        .value("CONNECTING_H", Kage::END_CONNECTING_H)
        .value("CONNECTING_V", Kage::END_CONNECTING_V)
        .value("LOWER_LEFT_CORNER", Kage::END_LOWER_LEFT_CORNER)
        .value("LOWER_RIGHT_CORNER", Kage::END_LOWER_RIGHT_CORNER)
        .value("LOWER_LEFT_ZH_OLD", Kage::END_LOWER_LEFT_ZH_OLD)
        .value("LOWER_LEFT_ZH_NEW", Kage::END_LOWER_LEFT_ZH_NEW)
        .value("TURN_LEFT", Kage::END_TURN_LEFT)
        .value("LOWER_RIGHT_HT", Kage::END_LOWER_RIGHT_HT)
        .value("TURN_UPWARDS", Kage::END_TURN_UPWARDS)
        .value("LEFT_SWEEP", Kage::END_LEFT_SWEEP)
        .value("STOP", Kage::END_STOP)
        .value("INTERNAL_LAST_FILLED_CIRCLE",
            Kage::END_INTERNAL_LAST_FILLED_CIRCLE)
        .value("INTERNAL_TURN_LEFT", Kage::END_INTERNAL_TURN_LEFT)
        .value("INTERNAL_TURN_UPWARDS", Kage::END_INTERNAL_TURN_UPWARDS)
        .export_values();

    py::class_<Kage::Point>(module, "Point")
        .def(py::init<>())
        .def(py::init<double, double>())
        .def_readwrite("x", &Kage::Point::x)
        .def_readwrite("y", &Kage::Point::y)
        .def("__repr__", [](const Kage::Point& point) { return pointRepr(point); });

    py::class_<Kage::Stroke>(module, "Stroke")
        .def(py::init<>())
        .def_readwrite("type", &Kage::Stroke::type)
        .def_readwrite("start", &Kage::Stroke::start)
        .def_readwrite("end", &Kage::Stroke::end)
        .def_readwrite("p1", &Kage::Stroke::p1)
        .def_readwrite("p2", &Kage::Stroke::p2)
        .def_readwrite("p3", &Kage::Stroke::p3)
        .def_readwrite("p4", &Kage::Stroke::p4)
        .def_readwrite("type_opt", &Kage::Stroke::typeOpt)
        .def_readwrite("start_opt", &Kage::Stroke::startOpt)
        .def_readwrite("end_opt", &Kage::Stroke::endOpt)
        .def_readwrite("buhin", &Kage::Stroke::buhin)
        .def_readwrite("s1", &Kage::Stroke::s1)
        .def_readwrite("s2", &Kage::Stroke::s2)
        .def_readwrite("special1", &Kage::Stroke::spe1)
        .def_readwrite("special2", &Kage::Stroke::spe2)
        .def("__repr__", [](const Kage::Stroke& stroke) { return strokeRepr(stroke); });

    py::class_<Kage::BoundingData>(module, "BoundingData")
        .def(py::init<>())
        .def_readwrite("min_x", &Kage::BoundingData::minX)
        .def_readwrite("min_y", &Kage::BoundingData::minY)
        .def_readwrite("max_x", &Kage::BoundingData::maxX)
        .def_readwrite("max_y", &Kage::BoundingData::maxY)
        .def("__repr__", [](const Kage::BoundingData& boundingData) {
            return boundingDataRepr(boundingData);
        });

    py::class_<Kage::Contour>(module, "Contour")
        .def(py::init<>())
        .def_property_readonly(
            "points", [](Kage::Contour& contour) { return contour.Points(); })
        .def_property_readonly("point_types",
            [](Kage::Contour& contour) { return contour.PointTypes(); })
        .def_property_readonly("closed", [](Kage::Contour& contour) {
        return contour.isClosed();
    }).def("size", &Kage::Contour::Size)
        .def("__repr__", [](Kage::Contour& contour) { return contourRepr(contour); });

    py::class_<Kage::Canva>(module, "Canva")
        .def(py::init<>())
        .def("clear", &Kage::Canva::Clear)
        .def("size", &Kage::Canva::Size)
        .def("contours", [](Kage::Canva& canva) { return canva.Contours(); })
        .def("__repr__", [](Kage::Canva& canva) { return canvaRepr(canva); });

    py::class_<Engine>(module, "Engine")
        .def(py::init<Kage::KageFontType, double>(),
            py::arg("font") = Kage::KAGEFONT_MINCHO, py::arg("size") = 0)
        .def("set_buhin", &Engine::setBuhin, py::arg("name"), py::arg("data"))
        .def("set_buhin_strokes", &Engine::setBuhinStrokes, py::arg("name"),
            py::arg("strokes"))
        .def("push_buhin", &Engine::pushBuhin, py::arg("name"), py::arg("data"))
        .def("push_buhin_strokes", &Engine::pushBuhinStrokes, py::arg("name"),
            py::arg("strokes"))
        .def("search_buhin", &Engine::searchBuhin, py::arg("name"))
        .def("delete_buhin", &Engine::deleteBuhin, py::arg("name"))
        .def("export_buhins", &Engine::exportBuhins)
        .def("set_notdef_glyph", &Engine::setNotDefGlyph, py::arg("data"))
        .def("set_notdef_glyph_strokes", &Engine::setNotDefGlyphStrokes,
            py::arg("strokes"))
        .def("set_font", &Engine::setFont, py::arg("font"), py::arg("size") = 0)
        .def("set_db_search_callback", &Engine::setDBSearchCallback,
            py::arg("callback"))
        .def("render", &Engine::render, py::arg("name"))
        .def("render_data", &Engine::renderData, py::arg("data"))
        .def("render_data_separated", &Engine::renderDataSeparated, py::arg("data"))
        .def("render_svg", &Engine::renderSVG, py::arg("name"))
        .def("render_data_svg", &Engine::renderDataSVG, py::arg("data"))
        .def("render_sfd", &Engine::renderSFD, py::arg("name"))
        .def("render_data_sfd", &Engine::renderDataSFD, py::arg("data"))
        .def("extract", &Engine::extract, py::arg("name"))
        .def("extract_data", &Engine::extractData, py::arg("data"))
        .def("check", &Engine::check, py::arg("name"))
        .def("check_data", &Engine::checkData, py::arg("data"))
        .def("__repr__", &Engine::repr);

    module.def("parse", &Kage::StrokesParse, py::arg("data"));
    module.def("serialize", [](const std::vector<Kage::Stroke>& strokes) {
        return Kage::StrokesToString(strokes);
    }, py::arg("strokes"));
    module.def("bounding_box", [](const std::vector<Kage::Stroke>& strokes) {
        return Kage::GetBoundingBox(strokes);
    }, py::arg("strokes"));
    module.def("stretch", &Kage::Stretch, py::arg("destination"),
        py::arg("source"), py::arg("position"), py::arg("minimum"),
        py::arg("maximum"));
    module.def("canva_to_svg", [](const Kage::Canva& canva) {
        py::gil_scoped_release release;
        return Kage::Canva2SVG(canva);
    }, py::arg("canva"));
    module.def("canva_to_sfd", [](const Kage::Canva& canva) {
        py::gil_scoped_release release;
        return Kage::Canva2SFD(canva);
    }, py::arg("canva"));
}
