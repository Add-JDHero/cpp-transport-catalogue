#include "request_handler.h"
#include <algorithm>
#include <numeric>

namespace transport_catalogue {


    RequestHandler::RequestHandler(const TransportCatalogue& db, const map_renderer::MapRenderer& renderer) :
        catalogue_(db),
        renderer_(renderer) {
    }

    // Возвращает информацию о маршруте (запрос Bus)
    std::optional<BusInfo> RequestHandler::GetBusStat(const std::string_view& bus_name) const {
        if (const auto bus = catalogue_.FindBus(bus_name)) {
            return std::optional<BusInfo>{catalogue_.GetBusInfo(bus_name)};
        }

        return std::nullopt;
    }

    // Возвращает маршруты, проходящие через
    const std::set<std::string_view> RequestHandler::GetBusesByStop(const std::string_view& stop_name) const {
        return catalogue_.GetStopToBusInfo(stop_name);
    }

    svg::Document RequestHandler::RenderMap() const {
        std::vector<BusPtr> buses(catalogue_.BusesCount());
        const std::deque<Bus>& buses_order = catalogue_.GetBusesByOrder();
        std::transform(
            buses_order.begin(), buses_order.end(),
            buses.begin(),
            [](const auto& p) {
                return &p; });

        std::sort(begin(buses), end(buses), [](const auto& lhs, const auto& rhs) 
            { return std::lexicographical_compare(lhs->bus_num_.begin(), lhs->bus_num_.end(), rhs->bus_num_.begin(), rhs->bus_num_.end());}
        );

        return renderer_.RenderMap(buses.begin(), buses.end());
    }

}