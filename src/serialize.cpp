#include "serialize.h"
#include <fstream>

namespace serialize {
    using namespace transport_catalogue;

    Serialize::Serialize(TransportCatalogue& catalogue,
                            RoutingSettings& route_settings,
                            map_renderer::RenderSettings& render_settings)
    : catalogue_(catalogue)
    , render_settings_(render_settings)
    , routing_settings_(route_settings) {
        catalogue_proto_ = transport_catalogue_proto::TransportCatalogue();
    }

    void Serialize::SetSerializationSettings(SerializationSettings&& settings) {
        catalogue_settings_.file_name = std::move(settings.file_name);
    }

    void Serialize::SerializeStops(std::unordered_map<std::string_view, uint32_t>& indexes) {
        const auto& stops = catalogue_.GetStops();
        int stop_index = 0;
        for (const auto& stop: stops) {
            transport_catalogue_proto::Stop* stop_proto = catalogue_proto_.add_stops();
            indexes[stop.stop_name_] = stop_index++;
            stop_proto->set_stop_name(std::string(stop.stop_name_));
            transport_catalogue_proto::Coords coords;
            coords.set_lat(stop.coords_.lat);
            coords.set_lng(stop.coords_.lng);
            stop_proto->set_id(stop_index);
            indexes[stop.stop_name_] = stop_index++;
            *(stop_proto->mutable_coords()) = coords;
        }
    }

    void Serialize::SerializeBuses(std::unordered_map<std::string_view, uint32_t>& indexes) {
        const auto& buses = catalogue_.GetBusesByOrder();
        int bus_index = 0;
        for (const auto& bus: buses) {
            transport_catalogue_proto::Bus* bus_proto = catalogue_proto_.add_buses();
            bus_proto->set_bus_num(bus.bus_num_);
            bus_proto->set_is_circle(bus.flag_);
            for (const auto& stop : bus.route_) {
				bus_proto->add_stop_indexes(indexes[stop->stop_name_]);
			}
        }
    }

    transport_catalogue_proto::Color Serialize::SerializeColor(const svg::Color &color) {
        transport_catalogue_proto::Color serialized_color;
        if (std::holds_alternative<svg::Rgb>(color)) {
            transport_catalogue_proto::RGB* rgb = serialized_color.mutable_rgb();
            rgb->set_red(std::get<svg::Rgb>(color).red);
            rgb->set_green(std::get<svg::Rgb>(color).green);
            rgb->set_blue(std::get<svg::Rgb>(color).blue);
        } else if (std::holds_alternative<svg::Rgba>(color)) {
            transport_catalogue_proto::RGBA* rgba = serialized_color.mutable_rgba();
            rgba->set_red(std::get<svg::Rgba>(color).red);
            rgba->set_green(std::get<svg::Rgba>(color).green);
            rgba->set_blue(std::get<svg::Rgba>(color).blue);
            rgba->set_opacity(std::get<svg::Rgba>(color).opacity);
        } else if (std::holds_alternative<std::string>(color)) {
            serialized_color.set_name_color(std::get<std::string>(color));
        } else {
            serialized_color.set_name_color("monostate"s);
        }
        
        return serialized_color;
    }

    void Serialize::SerializeRenderSettings() {
        transport_catalogue_proto::RenderSettings* serialized_render_settings_ = catalogue_proto_.mutable_render_settings();

        serialized_render_settings_->set_width(render_settings_.width);
        serialized_render_settings_->set_height(render_settings_.height);
        serialized_render_settings_->set_padding(render_settings_.padding);
        serialized_render_settings_->set_line_width(render_settings_.line_width);
        serialized_render_settings_->set_stop_radius(render_settings_.stop_radius);
        serialized_render_settings_->set_bus_label_font_size(render_settings_.bus_label_font_size);
        serialized_render_settings_->set_bus_label_offset_x(render_settings_.bus_label_offset.x);
        serialized_render_settings_->set_bus_label_offset_y(render_settings_.bus_label_offset.y);
        serialized_render_settings_->set_stop_label_font_size(render_settings_.stop_label_font_size);
        serialized_render_settings_->set_stop_label_offset_x(render_settings_.stop_label_offset.x);
        serialized_render_settings_->set_stop_label_offset_y(render_settings_.stop_label_offset.y);
        *serialized_render_settings_->mutable_underlayer_color() = SerializeColor(render_settings_.underlayer_color);
        serialized_render_settings_->set_underlayer_width(render_settings_.underlayer_width);

        for (const svg::Color& color : render_settings_.color_palette) {
            *serialized_render_settings_->add_color_palette() = SerializeColor(color);
        }
    }

    void Serialize::SerializeRoutingSettings() {
        transport_catalogue_proto::BusTimesSettings* serialized_routing_settings = catalogue_proto_.mutable_time_settings();
        serialized_routing_settings->set_bus_wait_time(routing_settings_.bus_wait_time);
        serialized_routing_settings->set_bus_velocity_m_m(routing_settings_.bus_velocity);
    }

    void Serialize::SerializeDistances(std::unordered_map<std::string_view, uint32_t>& indexes) {
        const auto& distances = catalogue_.GetDistances();
        for (const auto& [stops, distance] : distances) {
            transport_catalogue_proto::Distance* serialized_distance = catalogue_proto_.add_distances();
            serialized_distance->set_from(indexes[stops.first->stop_name_]);
            serialized_distance->set_to(indexes[stops.second->stop_name_]);
            serialized_distance->set_distance(distance);
        }
    }


    void Serialize::SerializeCatalogue(const std::string& output_file) {
        std::unordered_map<std::string_view, uint32_t> indexes;

        SetSerializationSettings({output_file});

        SerializeStops(indexes);
        SerializeBuses(indexes);
        SerializeDistances(indexes);
        SerializeRenderSettings();
        SerializeRoutingSettings();

        std::ofstream out(catalogue_settings_.file_name, std::ios::binary);
        if (!out.is_open()) {
            throw;
        }
        catalogue_proto_.SerializeToOstream(&out);
    }

    void Serialize::DeserializeCatalogue(const std::string& input_file) {
        SetSerializationSettings({input_file});
        std::unordered_map<uint32_t, std::string> indexes;

        std::ifstream in(catalogue_settings_.file_name, std::ios::binary);
        if (!catalogue_proto_.ParseFromIstream(&in)) {
            throw;
        }

        DeserializeStops(indexes);
        DeserializeBuses(indexes);
        DeserializeDistances(indexes);
        DeserializeRenderSettings();
        DeserializeRoutingSettings();
    }

    void Serialize::DeserializeStops(std::unordered_map<uint32_t, std::string>& indexes) {
        for (const auto& stop : catalogue_proto_.stops()) {
            catalogue_.AddStop(Stop{stop.stop_name(), geo::Coordinates{ stop.coords().lat(), stop.coords().lng()}});
            auto id = stop.id();
            indexes[id] = stop.stop_name();
        }
    }

    void Serialize::DeserializeBuses(std::unordered_map<uint32_t, std::string>& indexes) {
        for (const auto& bus : catalogue_proto_.buses()) {
            std::vector<StopPtr> route;
            std::unordered_set<StopPtr> unique_stops;
            bool is_circle = bus.is_circle();
            std::string bus_num = bus.bus_num();
            for (const auto& id : bus.stop_indexes()) {
                domain::StopPtr ptr = catalogue_.FindStop(indexes[id]);
                route.push_back(ptr);
                unique_stops.insert(ptr);
            }
            catalogue_.AddBus(Bus{std::move(bus_num), std::move(route), std::move(unique_stops), is_circle});
        }
    }

    void Serialize::DeserializeDistances(std::unordered_map<uint32_t, std::string>& indexes) {
        for (const auto& distance : catalogue_proto_.distances()) {
            const std::string from = indexes[distance.from()];
            const std::string to = indexes[distance.to()];
            catalogue_.SetActualDistance(from, to, distance.distance());
        }
    }

    void Serialize::DeserializeRoutingSettings() {
        routing_settings_.bus_wait_time = catalogue_proto_.time_settings().bus_wait_time();
        routing_settings_.bus_velocity = catalogue_proto_.time_settings().bus_velocity_m_m();
    }

    svg::Color Serialize::DeserializeColor(const transport_catalogue_proto::Color& serialized_color) {
        if (serialized_color.has_rgb()) {
            svg::Rgb rgb;
            rgb.red = serialized_color.rgb().red();
            rgb.green = serialized_color.rgb().green();
            rgb.blue = serialized_color.rgb().blue();
            return rgb;
        } else if (serialized_color.has_rgba()) {
            svg::Rgba rgba;
            rgba.red = serialized_color.rgba().red();
            rgba.green = serialized_color.rgba().green();
            rgba.blue = serialized_color.rgba().blue();
            rgba.opacity = serialized_color.rgba().opacity();
            return rgba;
        } else if (serialized_color.name_color() != "monostate"s) {
            return serialized_color.name_color();
        }

        return std::monostate();
    }

    void Serialize::DeserializeRenderSettings() {
        transport_catalogue_proto::RenderSettings* serialized_render_settings = catalogue_proto_.mutable_render_settings();

        render_settings_.width = serialized_render_settings->width();
        render_settings_.height = serialized_render_settings->height();
        render_settings_.padding = serialized_render_settings->padding();
        render_settings_.line_width = serialized_render_settings->line_width();
        render_settings_.stop_radius = serialized_render_settings->stop_radius();
        render_settings_.bus_label_font_size = serialized_render_settings->bus_label_font_size();
        render_settings_.bus_label_offset = {serialized_render_settings->bus_label_offset_x(), serialized_render_settings->bus_label_offset_y() };
        render_settings_.stop_label_font_size = serialized_render_settings->stop_label_font_size();
        render_settings_.stop_label_offset = {serialized_render_settings->stop_label_offset_x(), serialized_render_settings->stop_label_offset_y() };
        render_settings_.underlayer_color = DeserializeColor(serialized_render_settings->underlayer_color());
        render_settings_.underlayer_width = serialized_render_settings->underlayer_width();

        for (transport_catalogue_proto::Color serialized_color : serialized_render_settings->color_palette()) {
            render_settings_.color_palette.push_back(DeserializeColor(serialized_color));
        }
    }
}