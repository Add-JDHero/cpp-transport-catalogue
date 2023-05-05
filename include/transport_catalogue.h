#pragma once 

#include "ranges.h"
#include "geo.h"
#include "domain.h"

#include <deque>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <set>

namespace transport_catalogue {
    using namespace domain;
    using PointersHasher = transport_catalogue::domain::PointersHasher;
    using distances_container = std::unordered_map<std::pair<StopPtr, StopPtr>, double, PointersHasher>;

    class TransportCatalogue {
    public:
        void AddStop(const Stop& stop_data);
        void AddBus(const Bus& bus_data);

        void SetActualDistance(std::string_view from, std::string_view to, double distance);

        StopPtr FindStop(std::string_view stop_name) const;
        BusPtr FindBus(std::string_view bus_num) const;

        const BusInfo GetBusInfo(std::string_view bus_num) const;
        const std::set<std::string_view> GetStopToBusInfo(std::string_view stop_name) const;
        double GetDistanceBetweenStops(StopPtr from, StopPtr to, bool is_actual = true) const;

        RouteLength ComputeRouteLength(const std::vector<StopPtr>& route, bool is_circle) const;

        const std::deque<Bus>& GetBusesByOrder() const;
        const std::deque<Stop>& GetStops() const;
        
        const distances_container& GetDistances() const;

        size_t StopsCount() const noexcept;
        size_t BusesCount() const noexcept;

        ranges::Range<std::deque<Stop>::const_iterator> GetStopsRange() const;
        ranges::Range<std::deque<Bus>::const_iterator> GetBusesRange() const;

    private:
        std::deque<Stop> stops_;
        std::unordered_map<std::string_view, StopPtr> stopname_to_stop_;

        std::unordered_map<StopPtr, std::set<std::string_view>, PointersHasher> stopname_to_bus_;
        
        std::deque<Bus> buses_;
        std::unordered_map<std::string_view, BusPtr> busname_to_bus_;

        distances_container distances_;
        distances_container actual_distances_;

        void SetGeoDistances(const std::vector<StopPtr>& route);

        const BusInfo CreateBusInfo(BusPtr bus_ptr) const;

        RouteLength ComputeCircleRouteLength(const std::vector<StopPtr>& route) const;
        RouteLength ComputeNotCircleRouteLength(const std::vector<StopPtr>& route) const;
    };
}


/*

    ! not bad idea to calc route length !

auto coord_distance = transform_reduce(
                    std::next(stops.begin()), stops.end(),
                    stops.begin(),
                    0.0,
                    std::plus<>{},
                    [](const auto* curr, const auto* prev){
                        return geo::ComputeDistance(prev->coordinates, curr->coordinates);
                    });
            auto distance = transform_reduce(
                    std::next(stops.begin()), stops.end(),
                    stops.begin(),
                    0.0,
                    std::plus<>{},
                    [this](const auto* curr, const auto* prev){
                        return db_.GetDistance(*prev, *curr);
                    });

*/