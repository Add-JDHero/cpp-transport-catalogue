#include "map_renderer.h"

#include <unordered_set>
#include <utility>
#include <limits>
#include <cmath>
#include <iostream>

namespace transport_catalogue {
    namespace map_renderer {
        using namespace svg;
        namespace input {
            svg::Color ParseColor(const json::Node& color) {
                if (color.IsArray()) {
                    const auto& rgba = color.AsArray();
                    if (rgba.size() == 4) {
                        return svg::Rgba{
                            static_cast<uint8_t>(rgba[0].AsInt()),
                            static_cast<uint8_t>(rgba[1].AsInt()),
                            static_cast<uint8_t>(rgba[2].AsInt()),
                            rgba[3].AsDouble(),
                        };
                    }
                    return svg::Rgb{
                        static_cast<uint8_t>(rgba[0].AsInt()),
                        static_cast<uint8_t>(rgba[1].AsInt()),
                        static_cast<uint8_t>(rgba[2].AsInt()),
                    };
                }
                return color.AsString();
            }

            svg::Point ParsePoint(const json::Array &point) {
                return {point[0].AsDouble(), point[1].AsDouble()};
            }

            RenderSettings ParseRenderSettings(const json::Document& document) {
                const auto& settings = document.GetRoot().AsMap().at("render_settings"s).AsMap();
                std::vector<svg::Color> color_palette;
                for (const auto& color : settings.at("color_palette"s).AsArray()) {
                    color_palette.push_back(ParseColor(color));
                }
                return {
                    settings.at("width"s).AsDouble(),
                    settings.at("height"s).AsDouble(),
                    settings.at("padding"s).AsDouble(),
                    settings.at("line_width"s).AsDouble(),
                    settings.at("stop_radius"s).AsDouble(),
                    settings.at("bus_label_font_size"s).AsInt(),
                    ParsePoint(settings.at("bus_label_offset"s).AsArray()),
                    settings.at("stop_label_font_size"s).AsInt(),
                    ParsePoint(settings.at("stop_label_offset"s).AsArray()),
                    ParseColor(settings.at("underlayer_color"s)),
                    settings.at("underlayer_width"s).AsDouble(),
                    std::move(color_palette)
                };
            }
        }
        
        svg::Point SphereProjector::operator()(geo::Coordinates coords) const {
                return {
                    (coords.lng - min_lon_) * zoom_coeff_ + padding_,
                    (max_lat_ - coords.lat) * zoom_coeff_ + padding_
                };
        }
        
        MapRenderer::MapRenderer(RenderSettings settings) :
            settings_(std::move(settings)) {
        }

        const RenderSettings& MapRenderer::GetSettings() const {
            return settings_;
        }

        svg::Polyline MapRenderer::CreateRoutePolyline(const SphereProjector& projector, domain::BusPtr ptr, const svg::Color& color) const {
            svg::Polyline route;
            auto& stops = ptr->route_;
            auto add_point = [&route, &projector](const domain::StopPtr ptr){ 
                route.AddPoint(projector(ptr->coords_));
            };

            std::for_each(stops.begin(), stops.end(), add_point);


            if (!ptr->flag_) {
                std::for_each(++stops.rbegin(), stops.rend(), add_point);
            }

            FillPolylineColor(route, color);

            return route;
        }

        void MapRenderer::RenderRouteEndpointNames(const svg::Point& position, const svg::Color& color, const std::string& name, std::vector<svg::Text>& out_texts) const {
            using namespace svg;
                        
            out_texts.push_back(Text()
                .SetFillColor(settings_.underlayer_color)
                .SetStrokeColor(settings_.underlayer_color)
                .SetStrokeWidth(settings_.underlayer_width)
                .SetStrokeLineCap(StrokeLineCap::ROUND)
                .SetStrokeLineJoin(StrokeLineJoin::ROUND)
                .SetPosition(position)
                .SetOffset(settings_.bus_label_offset)
                .SetFontSize(settings_.bus_label_font_size)
                .SetFontFamily("Verdana"s)
                .SetFontWeight("bold"s)
                .SetData(name)
            );

            out_texts.push_back(Text()
                .SetFillColor(color)
                .SetPosition(position)
                .SetOffset(settings_.bus_label_offset)
                .SetFontSize(settings_.bus_label_font_size)
                .SetFontFamily("Verdana"s)
                .SetFontWeight("bold"s)
                .SetData(name)
            );
        }

        void MapRenderer::FillPolylineColor(svg::Polyline& route, const svg::Color& color) const {
                using namespace svg;
                route
                    .SetFillColor(NoneColor)
                    .SetStrokeColor(color)
                    .SetStrokeLineCap(StrokeLineCap::ROUND)
                    .SetStrokeLineJoin(StrokeLineJoin::ROUND)
                    .SetStrokeWidth(settings_.line_width);
        }


        void MapRenderer::RenderStopName(const Point& position, const std::string& name, Document& document) const {
            document.Add(Text()
                .SetFillColor(settings_.underlayer_color)
                .SetStrokeColor(settings_.underlayer_color)
                .SetStrokeWidth(settings_.underlayer_width)
                .SetStrokeLineCap(StrokeLineCap::ROUND)
                .SetStrokeLineJoin(StrokeLineJoin::ROUND)
                .SetPosition(position)
                .SetOffset(settings_.stop_label_offset)
                .SetFontSize(settings_.stop_label_font_size)
                .SetFontFamily("Verdana"s)
                .SetData(name)
            );

            document.Add(Text()
                .SetFillColor("black"s)
                .SetPosition(position)
                .SetOffset(settings_.stop_label_offset)
                .SetFontSize(settings_.stop_label_font_size)
                .SetFontFamily("Verdana"s)
                .SetData(name)
            );
        }
    }
}