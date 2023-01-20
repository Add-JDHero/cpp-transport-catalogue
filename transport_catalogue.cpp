#include "transport_catalogue.h"

namespace transport_catalogue {
    Stop::Stop(std::string_view stop_name,  const Coordinates& p)
        : stop_name_(stop_name), coords_(p)
    {}

    Bus::Bus(std::string_view bus_num, const std::vector<const Stop*>& route, const std::unordered_set<const Stop*>& unq, const bool flag) 
        : bus_num_(bus_num), route_(route), unique_stops(unq), flag_(flag)
    {}

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

        double route_length = ComputeRouteLength(bus_ptr->route_, is_circle);

        return BusInfo{bus_num, stops_on_route, unique_stops, route_length};
    }

    void TransportCatalogue::SetRouteDistances(const std::vector<const Stop*>& route, bool is_circle) {
        if (is_circle) {
            SetCircleRouteDistances(route);
        } else {
            SetNotCircleRouteDistances(route);
        }
    }

    double TransportCatalogue::ComputeRouteLength(const std::vector<const Stop*>& route, bool is_circle) const {
        double length = 0;
        for (auto it = route.begin(); it != route.end() - 1; ++it) {
            auto stop_ptr = *it;
            auto next_stop_ptr = *(it +1);
            length += distances_.at(std::make_pair(stop_ptr, next_stop_ptr));
        }

        return is_circle ? length : length * 2;
    }

    void TransportCatalogue::SetCircleRouteDistances(const std::vector<const Stop*>& route) {
        auto begin = route.begin();
        auto end = route.end();
        for (; begin != end - 1; ++begin) {
            auto stop_ptr = *begin;
            auto next_stop_ptr = *(begin + 1);
            std::pair<const Stop*, const Stop*> pair = std::make_pair(stop_ptr, next_stop_ptr);
            distances_[pair] = ComputeDistance(stop_ptr->coords_, next_stop_ptr->coords_);
        }

        auto end_it = *(end - 1);
        auto begin_it = *begin;
        std::pair<const Stop*, const Stop*> pair = std::make_pair(end_it, begin_it);
        distances_[pair] = ComputeDistance(end_it->coords_, begin_it->coords_);
    }

    void TransportCatalogue::SetNotCircleRouteDistances(const std::vector<const Stop*>& route) {
        auto begin = route.begin();
        auto end = route.end();
        for (; begin != end - 1; ++begin) {
            auto stop_ptr = *begin;
            auto next_stop_ptr = *(begin + 1);
            std::pair<const Stop*, const Stop*> pair1 = std::make_pair(stop_ptr, next_stop_ptr);
            std::pair<const Stop*, const Stop*> pair2 = std::make_pair(next_stop_ptr, stop_ptr);
            distances_[pair1] = ComputeDistance(stop_ptr->coords_, next_stop_ptr->coords_);
            distances_[pair2] = ComputeDistance(next_stop_ptr->coords_, stop_ptr->coords_);
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
}

