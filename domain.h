/*
 * В этом файле вы можете разместить классы/структуры, которые являются частью предметной области (domain)
 * вашего приложения и не зависят от транспортного справочника. Например Автобусные маршруты и Остановки. 
 *
 * Их можно было бы разместить и в transport_catalogue.h, однако вынесение их в отдельный
 * заголовочный файл может оказаться полезным, когда дело дойдёт до визуализации карты маршрутов:
 * визуализатор карты (map_renderer) можно будет сделать независящим от транспортного справочника.
 *
 * Если структура вашего приложения не позволяет так сделать, просто оставьте этот файл пустым.
 *
 */

#pragma once

#include <string>
#include <utility>
#include <vector>
#include <unordered_set>
#include "geo.h"

namespace transport_catalogue {
    namespace domain {
        using Coordinates = geo::Coordinates;

        struct RouteLength {
            double length;
            double actual_length;
        };

        struct Stop {
            Stop(std::string_view stop_name, const Coordinates& p)
                : stop_name_(stop_name), coords_(p)
            {}

            std::string_view stop_name_;
            Coordinates coords_;
        };

        struct Bus {
            Bus(std::string_view bus_num, const std::vector<const Stop*>& route, const std::unordered_set<const Stop*>& unq, const bool flag)
                : bus_num_(bus_num), route_(route), unique_stops(unq), flag_(flag)
            {}
            
            std::string_view bus_num_;
            std::vector<const Stop*> route_;
            std::unordered_set<const Stop*> unique_stops;
            bool flag_;
        };

        struct BusInfo {
            std::string_view bus_num;
            int stops_on_route;
            int unique_stops;
        };

        struct ActualDistance {
            std::string_view from;
            std::vector<std::pair<double, std::string_view>> dist;
        };

        enum class Requests {
            BUS,
            STOP,
            BASEREQUEST,
            STATREQUEST,
        };

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
    }
}