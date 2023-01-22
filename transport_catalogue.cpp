#include "transport_catalogue.h"
#include "detail.h"
#include <algorithm>

namespace transport_catalogue {
    using Stop = transport_catalogue::detail::Stop;
    using Bus = transport_catalogue::detail::Bus;

    void TransportCatalogue::AddStop(const Stop& stop_data) {
        stops_.push_back(std::move(stop_data));
        stopname_to_stop_[stops_.back().stop_name_] = &stops_.back();
    }

    const Stop* TransportCatalogue::FindStop(std::string_view stop_name) const {
        if (stopname_to_stop_.count(stop_name) == 0) {
            return nullptr;
        }
        return stopname_to_stop_.at(stop_name);
    }

    void TransportCatalogue::AddBus(const Bus& bus_data) {
        buses_.push_back(bus_data);
        busname_to_bus_[buses_.back().bus_num_] = &buses_.back();
        SetRouteDistances(bus_data.route_, bus_data.flag_);
        for (auto stop_ptr: bus_data.unique_stops) {
            stopname_to_bus_[stop_ptr].insert(bus_data.bus_num_);
        }
    }

    const Bus* TransportCatalogue::FindBus(std::string_view bus_num) const {
        if (busname_to_bus_.count(bus_num) == 0) {
            return nullptr;
        }
        return busname_to_bus_.at(bus_num);
    }

    const BusInfo TransportCatalogue::GetBusInfo(std::string_view bus_num) const {
        const Bus* bus_ptr = FindBus(bus_num);
        if (bus_ptr) {
            return CreateBusInfo(bus_ptr);
        }
        
        return BusInfo{};
    }

    size_t TransportCatalogue::StopsCount() const noexcept {
        return stops_.size();
    }

    const BusInfo TransportCatalogue::CreateBusInfo(const Bus* bus_ptr) const{
        std::string_view bus_num = bus_ptr->bus_num_;
        int unique_stops = bus_ptr->unique_stops.size();
        int stops_on_route;
        bool is_circle = bus_ptr->flag_;
        if (is_circle) {
            stops_on_route = bus_ptr->route_.size();
        } else {
            stops_on_route = bus_ptr->route_.size() * 2 - 1;
        }

        RouteLength route_length = ComputeRouteLength(bus_ptr->route_, is_circle);

        return BusInfo{bus_num, stops_on_route, unique_stops, route_length};
    }

    void TransportCatalogue::SetRouteDistances(const std::vector<const Stop*>& route, bool is_circle) {
        if (is_circle) {
            SetCircleRouteDistances(route);
        } else {
            SetNotCircleRouteDistances(route);
        }
    }

    RouteLength TransportCatalogue::ComputeCircleRouteLength(const std::vector<const Stop*>& route) const {
        double length = 0;
        double actual_length = 0;
        for (auto it = route.begin(); it != route.end() - 1; ++it) {
            auto stop_ptr = *it;
            auto next_stop_ptr = *(it +1);
            auto pair = std::make_pair(stop_ptr, next_stop_ptr);
            double add = distances_.at(pair);
            length += add;
            add = GetDistanceBetweenStops(stop_ptr, next_stop_ptr, true);
            actual_length += add;
        }

        return {length, actual_length};
    }

    double TransportCatalogue::GetDistanceBetweenStops(const Stop* from, const Stop* to, bool is_actual) const {
        auto pair1 = std::make_pair(from, to);
        auto pair2 = std::make_pair(to, from);
        if ( (actual_distances_.count(pair1) || actual_distances_.count(pair2)) && is_actual ) {
            if (actual_distances_.count(pair1)) {
                return actual_distances_.at(pair1);
            } else {
                return actual_distances_.at(pair2);
            }
        } else {
            return distances_.at(pair1);
        }
    }

    RouteLength TransportCatalogue::ComputeNotCircleRouteLength(const std::vector<const Stop*>& route) const {
        double length = 0;
        double actual_length = 0;
        for (auto it = route.begin(); it != route.end() - 1; ++it) {
            auto stop_ptr = *it;
            auto next_stop_ptr = *(it + 1);
            double add1 = GetDistanceBetweenStops(stop_ptr, next_stop_ptr, false);
            double add2;
            length += add1 * 2;
            add1 = GetDistanceBetweenStops(stop_ptr, next_stop_ptr, true);
            add2 = GetDistanceBetweenStops(next_stop_ptr, stop_ptr, true);
            actual_length += add1 + add2;
        }

        return {length, actual_length};
    }


    RouteLength TransportCatalogue::ComputeRouteLength(const std::vector<const Stop*>& route, bool is_circle) const {
        RouteLength result = is_circle ? this->ComputeCircleRouteLength(route) : this->ComputeNotCircleRouteLength(route);
        return result;
    }

    void TransportCatalogue::SetCircleRouteDistances(const std::vector<const Stop*>& route) {
        auto begin = route.begin();
        auto end = route.end();
        for (; begin != end - 1; ++begin) {
            auto stop_ptr = *begin;
            auto next_stop_ptr = *(begin + 1);
            distances_[{stop_ptr, next_stop_ptr}] = ComputeDistance(stop_ptr->coords_, next_stop_ptr->coords_);
        }

        auto end_it = *(end - 1);
        auto begin_it = *begin;
        distances_[{end_it, begin_it}] = ComputeDistance(end_it->coords_, begin_it->coords_);
    }

    void TransportCatalogue::SetNotCircleRouteDistances(const std::vector<const Stop*>& route) {
        auto begin = route.begin();
        auto end = route.end();
        for (; begin != end - 1; ++begin) {
            auto stop_ptr = *begin;
            auto next_stop_ptr = *(begin + 1);
            double distance = ComputeDistance(stop_ptr->coords_, next_stop_ptr->coords_);
            distances_[{stop_ptr, next_stop_ptr}] = distance;
            distances_[{next_stop_ptr, stop_ptr}] = distance;
        }
    }

    const std::set<std::string> TransportCatalogue::GetStopToBusInfo(std::string_view stop_name) const {
        const Stop* stop_ptr = this->FindStop(stop_name);
        size_t buses = stopname_to_bus_.count(stop_ptr);
        if (buses != 0) {
            return stopname_to_bus_.at(this->FindStop(stop_name));
        } 
        return {};
    }

    void TransportCatalogue::SetActualDistance(std::string_view from, std::string_view to, double distance) {
        actual_distances_[{this->FindStop(from), this->FindStop(to) }] = distance;
    }
}

