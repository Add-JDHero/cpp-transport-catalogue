#pragma once 

#include "geo.h"
#include "detail.h"

#include <deque>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <set>

namespace transport_catalogue {
    using Stop = transport_catalogue::detail::Stop;
    using Bus = transport_catalogue::detail::Bus;
    using BusInfo = transport_catalogue::detail::BusInfo;
    using RouteLength = transport_catalogue::detail::RouteLength;
    using ActualDistance = transport_catalogue::detail::ActualDistance;
    using Requests = transport_catalogue::detail::Requests;

    class TransportCatalogue {
    public:
        void AddStop(const Stop& stop_data);
        const Stop* FindStop(std::string_view stop_name) const;

        void AddBus(const Bus& bus_data);
        const Bus* FindBus(std::string_view bus_num) const;
        const BusInfo GetBusInfo(std::string_view bus_num) const;
        const std::set<std::string> GetStopToBusInfo(std::string_view stop_name) const;
        void SetActualDistance(std::string_view from, std::string_view to, double distance);
        double GetDistanceBetweenStops(const Stop* from, const Stop* to, bool is_actual) const;

        size_t StopsCount() const noexcept;

    private:
        std::deque<Stop> stops_;
        std::unordered_map<std::string_view, const Stop*> stopname_to_stop_;
        
        std::deque<Bus> buses_;
        std::unordered_map<std::string_view, const Bus*> busname_to_bus_;

        std::unordered_map<std::pair<const Stop*, const Stop*>, double, transport_catalogue::detail::PointersHasher> distances_;
        std::unordered_map<std::pair<const Stop*, const Stop*>, double, transport_catalogue::detail::PointersHasher> actual_distances_;
        std::unordered_map<const Stop*, std::set<std::string>, transport_catalogue::detail::PointersHasher> stopname_to_bus_;

        const BusInfo CreateBusInfo(const Bus* bus_ptr) const;
        void SetRouteDistances(const std::vector<const Stop*>& route, bool is_circle);
        void SetCircleRouteDistances(const std::vector<const Stop*>& route);
        void SetNotCircleRouteDistances(const std::vector<const Stop*>& route);
        RouteLength ComputeCircleRouteLength(const std::vector<const Stop*>& route) const;
        RouteLength ComputeNotCircleRouteLength(const std::vector<const Stop*>& route) const;
        RouteLength ComputeRouteLength(const std::vector<const Stop*>& route, bool is_circle) const;
    };
}
