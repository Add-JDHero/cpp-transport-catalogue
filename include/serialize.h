#pragma once
#include <transport_catalogue.pb.h>
#include "transport_catalogue.h"
#include "transport_router.h"
#include "map_renderer.h"
#include "domain.h"

namespace serialize {

    struct SerializationSettings {
        std::string file_name;
    };

    class Serialize {
    public:
        Serialize() = default;
        Serialize(transport_catalogue::TransportCatalogue& catalogue,
                                transport_catalogue::RoutingSettings& route_settings,
                                transport_catalogue::map_renderer::RenderSettings& render_settings);
        
        void SetSerializationSettings(SerializationSettings&& set);
        void SerializeStops(std::unordered_map<std::string_view, uint32_t>& indexes);
        void SerializeBuses(std::unordered_map<std::string_view, uint32_t>& indexes);
        transport_catalogue_proto::Color SerializeColor(const svg::Color& color);
        void SerializeRenderSettings();
        void SerializeRoutingSettings();
        void SerializeCatalogue(const std::string& file_name);
        void SerializeDistances(std::unordered_map<std::string_view, uint32_t>& indexes);
        

        void DeserializeCatalogue(const std::string& input_file);
        void DeserializeStops(std::unordered_map<uint32_t, std::string>& indexes);
        void DeserializeBuses(std::unordered_map<uint32_t, std::string>& indexes);
        void DeserializeDistances(std::unordered_map<uint32_t, std::string>& indexes);
        void DeserializeRoutingSettings();
        void DeserializeRenderSettings();

        const transport_catalogue::map_renderer::RenderSettings& GetRenderSettings() { return render_settings_; }
        const transport_catalogue::RoutingSettings& GetRoutingSettings() { return routing_settings_; }
        svg::Color DeserializeColor(const transport_catalogue_proto::Color& serialized_color);
    private:
        SerializationSettings catalogue_settings_;
        transport_catalogue_proto::TransportCatalogue catalogue_proto_;
        transport_catalogue::TransportCatalogue& catalogue_;
        transport_catalogue::map_renderer::RenderSettings& render_settings_;
        transport_catalogue::RoutingSettings& routing_settings_;
    };
}