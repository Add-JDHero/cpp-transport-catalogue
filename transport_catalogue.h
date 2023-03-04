#pragma once 

#include "geo.h"
#include "domain.h"

#include <deque>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <set>

namespace transport_catalogue {
    using Stop = domain::Stop;
    using Bus = domain::Bus;
    using BusInfo = domain::BusInfo;
    using RouteLength = domain::RouteLength;
    using ActualDistance = domain::ActualDistance;
    using Requests = domain::Requests;
    using Coordinates = geo::Coordinates;
    using PointersHasher = transport_catalogue::domain::PointersHasher;

    class TransportCatalogue {
    public:
        void AddStop(const Stop& stop_data);
        void AddBus(const Bus& bus_data);

        const Stop* FindStop(std::string_view stop_name) const;
        const Bus* FindBus(std::string_view bus_num) const;

        const BusInfo GetBusInfo(std::string_view bus_num) const;
        const std::set<std::string_view> GetStopToBusInfo(std::string_view stop_name) const;
        double GetDistanceBetweenStops(const Stop* from, const Stop* to, bool is_actual) const;

        void SetActualDistance(std::string_view from, std::string_view to, double distance);

        RouteLength ComputeRouteLength(const std::vector<const Stop*>& route, bool is_circle) const;

        size_t StopsCount() const noexcept;

    private:
        std::deque<Stop> stops_;
        std::unordered_map<std::string_view, const Stop*> stopname_to_stop_;

        std::unordered_map<const Stop*, std::set<std::string_view>, PointersHasher> stopname_to_bus_;
        
        std::deque<Bus> buses_;
        std::unordered_map<std::string_view, const Bus*> busname_to_bus_;

        std::unordered_map<std::pair<const Stop*, const Stop*>, double, PointersHasher> distances_;
        std::unordered_map<std::pair<const Stop*, const Stop*>, double, PointersHasher> actual_distances_;
        

        const BusInfo CreateBusInfo(const Bus* bus_ptr) const;

        void SetGeoDistances(const std::vector<const Stop*>& route);

        RouteLength ComputeCircleRouteLength(const std::vector<const Stop*>& route) const;
        RouteLength ComputeNotCircleRouteLength(const std::vector<const Stop*>& route) const;
    };
}
