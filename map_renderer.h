#pragma once

#include "svg.h"
#include "domain.h"
#include "geo.h"
#include "json.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <deque>
#include <set>

using namespace std::literals;

namespace transport_catalogue {

    namespace map_renderer {

        struct RenderSettings {
            double width;
            double height;

            double padding;

            double line_width;
            double stop_radius;

            int bus_label_font_size;
            svg::Point bus_label_offset;

            int stop_label_font_size;
            svg::Point stop_label_offset;

            svg::Color underlayer_color;
            double underlayer_width;

            std::vector<svg::Color> color_palette;
        };

        namespace input {

            svg::Point ParsePoint(const json::Array &point);

            svg::Color ParseColor(const json::Node& color);

            RenderSettings ParseRenderSettings(const json::Document& document);
        }  // namespace input


        inline const double EPSILON = 1e-6;
        inline bool IsZero(double value) {
            return std::abs(value) < EPSILON;
        }


        class SphereProjector {
        public:
            // points_begin и points_end задают начало и конец интервала элементов geo::Coordinates
            template <typename PointInputIt>
            SphereProjector(PointInputIt points_begin, PointInputIt points_end,
                            double max_width, double max_height, double padding);

            // Проецирует широту и долготу в координаты внутри SVG-изображения
            svg::Point operator()(geo::Coordinates coords) const;

        private:
            double padding_;
            double min_lon_ = 0;
            double max_lat_ = 0;
            double zoom_coeff_ = 0;
        };

        class MapRenderer {
        public:
            explicit MapRenderer(RenderSettings settings);

            const RenderSettings& GetSettings() const;

            template<typename BusIterator>
            svg::Document RenderMap(BusIterator first, BusIterator last) const;

        private:
            RenderSettings settings_;

            template<typename BusIterator>
            void RenderRoutePolylines(const SphereProjector& projector, BusIterator first, BusIterator last, svg::Document& document) const;

            svg::Polyline CreateRoutePolyline(const SphereProjector& projector, domain::BusPtr ptr, const svg::Color& color) const;

            void FillPolylineColor(svg::Polyline& route, const svg::Color& color) const;

            void RenderRouteEndpointNames(const svg::Point& position, const svg::Color& color,
                                          const std::string& name, std::vector<svg::Text>& out_texts) const;
            template<typename StopIterator>
            void RenderStops(StopIterator first, StopIterator last, const SphereProjector& projector, svg::Document& document) const;
            void RenderStopName(const svg::Point& position, const std::string& name, svg::Document& document) const;
        };
    }
}


//  ---------- definitions of template functions ----------

namespace transport_catalogue {

    namespace map_renderer {

        template <typename PointInputIt>
        SphereProjector::SphereProjector(PointInputIt points_begin, PointInputIt points_end,
                        double max_width, double max_height, double padding)
            : padding_(padding) //
        {
            // Если точки поверхности сферы не заданы, вычислять нечего
            if (points_begin == points_end) {
                return;
            }

            // Находим точки с минимальной и максимальной долготой
            const auto [left_it, right_it] = std::minmax_element(
                points_begin, points_end,
                [](auto lhs, auto rhs) { return lhs.lng < rhs.lng; });
            min_lon_ = left_it->lng;
            const double max_lon = right_it->lng;

            // Находим точки с минимальной и максимальной широтой
            const auto [bottom_it, top_it] = std::minmax_element(
                points_begin, points_end,
                [](auto lhs, auto rhs) { return lhs.lat < rhs.lat; });
            const double min_lat = bottom_it->lat;
            max_lat_ = top_it->lat;

            // Вычисляем коэффициент масштабирования вдоль координаты x
            std::optional<double> width_zoom;
            if (!IsZero(max_lon - min_lon_)) {
                width_zoom = (max_width - 2 * padding) / (max_lon - min_lon_);
            }

            // Вычисляем коэффициент масштабирования вдоль координаты y
            std::optional<double> height_zoom;
            if (!IsZero(max_lat_ - min_lat)) {
                height_zoom = (max_height - 2 * padding) / (max_lat_ - min_lat);
            }

            if (width_zoom && height_zoom) {
                // Коэффициенты масштабирования по ширине и высоте ненулевые,
                // берём минимальный из них
                zoom_coeff_ = std::min(*width_zoom, *height_zoom);
            } else if (width_zoom) {
                // Коэффициент масштабирования по ширине ненулевой, используем его
                zoom_coeff_ = *width_zoom;
            } else if (height_zoom) {
                // Коэффициент масштабирования по высоте ненулевой, используем его
                zoom_coeff_ = *height_zoom;
            }
        }

        template<typename BusIterator>
        void MapRenderer::RenderRoutePolylines(const SphereProjector& projector, BusIterator first, BusIterator last, svg::Document& document) const {
            size_t index = 0;
            std::vector<svg::Text> route_names;
            size_t nums = settings_.color_palette.size();

            for (BusIterator it = first; it != last; ++it) {
                domain::BusPtr bus = *it;
                const svg::Color color = settings_.color_palette[index++ % nums];

                svg::Polyline route = CreateRoutePolyline(projector, bus, color);
                document.Add(std::move(route));

                RenderRouteEndpointNames(projector(bus->route_.front()->coords_), color, std::string(bus->bus_num_), route_names);
                if (!bus->flag_ && bus->route_.front() != bus->route_.back()) {
                    RenderRouteEndpointNames(projector(bus->route_.back()->coords_), color, std::string(bus->bus_num_), route_names);
                }
            }

            for (const auto& name : route_names) {
                document.Add(name);
            }

        }

        template<typename StopIterator>
        void MapRenderer::RenderStops(StopIterator first, StopIterator last, const SphereProjector& projector, svg::Document& document) const {
            using namespace svg;
            using namespace std;

            for (auto it = first; it != last; ++it) {
                domain::StopPtr stop = *it;
                document.Add(Circle()
                    .SetCenter(projector(stop->coords_))
                    .SetRadius(settings_.stop_radius)
                    .SetFillColor("white"s));
            }

            for (auto it = first; it != last; ++it) {
                domain::StopPtr stop = *it;
                RenderStopName(projector(stop->coords_), std::string(stop->stop_name_), document);
            }
        }

        template<typename BusIterator>
        svg::Document MapRenderer::RenderMap(BusIterator first, BusIterator last) const {
            std::set<domain::StopPtr, domain::StopComparator> stops;
            for (auto it = first; it != last; ++it) {
                stops.insert((*it)->route_.begin(), (*it)->route_.end());
            }

            std::vector<geo::Coordinates> points(stops.size());
            transform(
                stops.begin(), stops.end(),
                points.begin(),
                [](const domain::StopPtr stop) {
                    return stop->coords_;
                }
            );

            SphereProjector projector(
                points.begin(), points.end(), settings_.width,
                settings_.height, settings_.padding
            );

            svg::Document document;
            RenderRoutePolylines(projector, first, last, document);
            RenderStops(stops.begin(), stops.end(), projector, document);
            return document;
        }

    } // namespace map_renderer
} // namespace transport_catalogue