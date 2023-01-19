#pragma once 

#include "geo.h"

#include <deque>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace transport_catalogue { 
    struct Stop {
        Stop(std::string_view stop_name, const Coordinates& p);

        Coordinates coords_;
        std::string stop_name_;
    };

    struct Bus {
        Bus(std::string_view bus_num, const std::vector<const Stop*>& route, const std::unordered_set<const Stop*>& unq, const bool flag);

        std::string bus_num_;
        std::vector<const Stop*> route_;
        std::unordered_set<const Stop*> unique_stops;
        bool flag_;
    };

    struct BusInfo {
        std::string_view bus_num;
        int stops_on_route;
        int unique_stops;
        double route_length;
    };

    class TransportCatalogue {
        class PointersHasher {
        public:
            std::size_t operator()(const std::pair<const Stop*, const Stop*> pair) const noexcept {
                return hasher_(static_cast<const void*>(pair.first)) * 41 + hasher_(static_cast<const void*>(pair.second));
            }

            std::size_t operator()(const Stop* stop) const noexcept {
                return hasher_(static_cast<const void*>(stop)) * 41;
            }
        private:
            std::hash<const void*> hasher_;
        };

    public:
        void AddStop(const Stop& stop_data);
        const Stop* FindStop(std::string_view stop_name) const;

        void AddBus(const Bus& bus_data);
        const Bus* FindBus(std::string_view bus_num) const;
        const BusInfo GetBusInfo(std::string_view bus_num) const;

        size_t StopsCount() const noexcept;

    private:
        std::deque<Stop> stops_;
        std::unordered_map<std::string_view, const Stop*> stopname_to_stop;
        
        std::deque<Bus> buses_;
        std::unordered_map<std::string_view, const Bus*> busname_to_bus;

        std::unordered_map<std::pair<const Stop*, const Stop*>, double, PointersHasher> distances_;

        const BusInfo CreateBusInfo(const Bus* bus_ptr) const;
        void SetRouteDistances(const std::vector<const Stop*>& route, bool is_circle);
        void SetCircleRouteDistances(const std::vector<const Stop*>& route);
        void SetNotCircleRouteDistances(const std::vector<const Stop*>& route);
        double ComputeRouteLength(const std::vector<const Stop*>& route, bool is_circle) const;
    };
}
