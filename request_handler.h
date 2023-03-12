#pragma once

#include "domain.h"
#include "transport_catalogue.h"
#include "map_renderer.h"
#include "geo.h"

namespace transport_catalogue {

    class RequestHandler {
    public:
        RequestHandler() = default;
        RequestHandler(const TransportCatalogue& db, const map_renderer::MapRenderer& renderer);

        // Возвращает информацию о маршруте (запрос Bus)
        std::optional<BusInfo> GetBusStat(const std::string_view& bus_name) const;

        // Возвращает маршруты, проходящие через
        const std::set<std::string_view> GetBusesByStop(const std::string_view& stop_name) const;

        svg::Document RenderMap() const;

    private:
        // RequestHandler использует агрегацию объектов "Транспортный Справочник" и "Визуализатор Карты"
        const TransportCatalogue& catalogue_;
        const map_renderer::MapRenderer& renderer_;
    };

}