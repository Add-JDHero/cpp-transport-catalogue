#pragma once

#include <cstdint>
#include <iostream>
#include <sstream>
#include <memory>
#include <string>
#include <list>
#include <vector>
#include <optional>
#include <variant>

namespace svg {
    using namespace std::literals;

    struct Rgb {
        Rgb()= default;
        Rgb(uint8_t red, uint8_t green, uint8_t blue)
                : red(red)
                , green(green)
                , blue(blue) {
        }
        uint8_t red = 0;
        uint8_t green = 0;
        uint8_t blue = 0;
    };

    struct Rgba {
        Rgba()= default;
        Rgba(uint8_t red, uint8_t green, uint8_t blue, double opacity)
                : red(red)
                , green(green)
                , blue(blue)
                , opacity(opacity) {
        }
        uint8_t red = 0;
        uint8_t green = 0;
        uint8_t blue = 0;
        double opacity = 1.0;
    };

    using Color = std::variant<std::monostate, Rgb, Rgba, std::string>;
    inline const Color NoneColor{"none"};

    struct ColorPrinter {
        std::ostream& out;

        void operator()(std::monostate) const {
            out << "none"sv;
        }
        void operator()(std::string_view str) const {
            out << str;
        }
        void operator()(svg::Rgb rgb) const {
            out << "rgb("sv << static_cast<int>(rgb.red)
                << ","sv << static_cast<int>(rgb.green) << ","sv
                << static_cast<int>(rgb.blue) << ")"sv;
        }
        void operator()(svg::Rgba rgba) const {
            out << "rgba("sv << static_cast<int>(rgba.red) << ","sv
                << static_cast<int>(rgba.green) << ","sv
                << static_cast<int>(rgba.blue) << ","sv
                << rgba.opacity << ")"sv;
        }
    };

    enum class StrokeLineCap {
        BUTT,
        ROUND,
        SQUARE,
    };

    enum class StrokeLineJoin {
        ARCS,
        BEVEL,
        MITER,
        MITER_CLIP,
        ROUND,
    };

    inline std::ostream& operator<<(std::ostream& out, Color color) {
        std::visit(ColorPrinter{out}, color);
        return out;
    }

    inline std::ostream& operator<<(std::ostream& out, StrokeLineCap slp) {
        if (slp == StrokeLineCap::BUTT) {
            out << "butt"sv;
        } else if (slp == StrokeLineCap::ROUND) {
            out << "round"sv;
        } else if (slp == StrokeLineCap::SQUARE) {
            out << "square"sv;
        }
        return out;
    }

    inline std::ostream& operator<<(std::ostream& out, StrokeLineJoin slj) {
        if (slj == StrokeLineJoin::ARCS) {
            out << "arcs"sv;
        } else if (slj == StrokeLineJoin::BEVEL) {
            out << "bevel"sv;
        } else if (slj == StrokeLineJoin::MITER) {
            out << "miter"sv;
        } else if (slj == StrokeLineJoin::MITER_CLIP) {
            out << "miter-clip"sv;
        } else if (slj == StrokeLineJoin::ROUND) {
            out << "round"sv;
        }
        return out;
    }

    struct Point {
        Point() = default;
        Point(double x, double y)
                : x(x)
                , y(y) {
        }
        double x = 0;
        double y = 0;
    };

    /*
     * Вспомогательная структура, хранящая контекст для вывода SVG-документа с отступами.
     * Хранит ссылку на поток вывода, текущее значение и шаг отступа при выводе элемента
     */
    struct RenderContext {
        RenderContext(std::ostream& out)
                : out(out) {
        }

        RenderContext(std::ostream& out, int indent_step, int indent = 0)
                : out(out)
                , indent_step(indent_step)
                , indent(indent) {
        }

        RenderContext Indented() const {
            return {out, indent_step, indent + indent_step};
        }

        void RenderIndent() const {
            for (int i = 0; i < indent; ++i) {
                out.put(' ');
            }
        }

        std::ostream& out;
        int indent_step = 0;
        int indent = 0;
    };

    class Object {
    public:
        Object() = default;
        void Render(const RenderContext& context) const;

        virtual ~Object() = default;

    private:
        virtual void RenderObject(const RenderContext& context) const = 0;
    };

    class ObjectContainer {
    public:
        template <typename Obj>
        void Add(Obj obj) {
            AddPtr(std::make_unique<Obj>(std::move(obj)));
        }

        virtual void AddPtr(std::unique_ptr<Object>&& obj) = 0;

    protected:
        ~ObjectContainer() = default;
    };


    class Drawable {
    public:
        virtual void Draw(ObjectContainer& container) const = 0;
        virtual ~Drawable() = default;
    };

    template <typename Owner>
    class PathProps {
    public:
        Owner& SetFillColor(Color color) {
            fill_color_ = std::move(color);
            return AsOwner();
        }
        Owner& SetStrokeColor(Color color) {
            stroke_color_ = std::move(color);
            return AsOwner();
        }

        Owner& SetStrokeWidth(double width) {
            stroke_width_ = width;
            return AsOwner();
        }

        Owner& SetStrokeLineCap(StrokeLineCap lineCap) {
            stroke_linecamp_ = lineCap;
            return AsOwner();
        }

        Owner& SetStrokeLineJoin(StrokeLineJoin lineJoin) {
            stroke_linejoin_ = lineJoin;
            return AsOwner();
        }

    protected:
        ~PathProps() = default;

        void RenderAttrs(std::ostream&out) const {
            using namespace std::literals;

            if (fill_color_) {
                out << " fill=\""sv << *fill_color_ << "\""sv;
            }
            if (stroke_color_) {
                out << " stroke=\""sv << *stroke_color_ << "\""sv;
            }
            if (stroke_width_) {
                out << " stroke-width=\""sv << *stroke_width_ << "\""sv;
            }
            if (stroke_linecamp_) {
                out << " stroke-linecap=\""sv << *stroke_linecamp_ << "\""sv;
            }
            if (stroke_linejoin_) {
                out << " stroke-linejoin=\""sv << *stroke_linejoin_ << "\""sv;
            }
        }

    private:
        Owner& AsOwner() {
            // static_cast безопасно преобразует *this к Owner&,
            // если класс Owner — наследник PathProps
            return static_cast<Owner&>(*this);
        }

        std::optional<Color> fill_color_;
        std::optional<Color> stroke_color_;
        std::optional<double> stroke_width_;
        std::optional<StrokeLineCap> stroke_linecamp_;
        std::optional<StrokeLineJoin> stroke_linejoin_;
    };


     //  Класс Circle моделирует элемент <circle> для отображения круга
    class Circle final : public Object, public PathProps<Circle> {
    public:
        Circle& SetCenter(Point center);
        Circle& SetRadius(double radius);

    private:
        void RenderObject(const RenderContext& context) const override;

        Point center_ = {0.0, 0.0};
        double radius_ = 1.0;
    };

    //  https://developer.mozilla.org/en-US/docs/Web/SVG/Element/polyline
    class Polyline final : public Object, public PathProps<Polyline> {
    public:
        // Добавляет очередную вершину к ломаной линии
        Polyline& AddPoint(Point point);

    private:
        void RenderObject(const RenderContext& context) const override;
        std::vector<Point> points_;
    };

    //  Класс Text моделирует элемент <text> для отображения текста
    class Text final : public Object, public PathProps<Text>{
    public:
        // Задаёт координаты опорной точки (атрибуты x и y)
        Text& SetPosition(Point pos);

        // Задаёт смещение относительно опорной точки (атрибуты dx, dy)
        Text& SetOffset(Point offset);

        // Задаёт размеры шрифта (атрибут font-size)
        Text& SetFontSize(uint32_t size);

        // Задаёт название шрифта (атрибут font-family)
        Text& SetFontFamily(std::string font_family);

        // Задаёт толщину шрифта (атрибут font-weight)
        Text& SetFontWeight(std::string font_weight);

        // Задаёт текстовое содержимое объекта (отображается внутри тега text)
        Text& SetData(std::string data);

        // Прочие данные и методы, необходимые для реализации элемента <text>
    private:
        void RenderObject(const RenderContext& context) const override;

        Point pos_;
        Point offset_;
        uint32_t size_ = 1;
        std::string font_weight_;
        std::string font_family_;
        std::string data_;
    };

    class Document : public ObjectContainer {
    public:
        Document() = default;

        // Добавляет в svg-документ объект-наследник svg::Object
        void AddPtr(std::unique_ptr<Object>&& obj) override;

        // Выводит в ostream svg-представление документа
        void Render(std::ostream& out) const;
    private:
        std::vector<std::unique_ptr<Object>> objects_;
    };


}  // namespace svg