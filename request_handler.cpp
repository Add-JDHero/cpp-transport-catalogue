#include "request_handler.h"

/*
 * Здесь можно было бы разместить код обработчика запросов к базе, содержащего логику, которую не
 * хотелось бы помещать ни в transport_catalogue, ни в json reader.
 *
 * Если вы затрудняетесь выбрать, что можно было бы поместить в этот файл,
 * можете оставить его пустым.
 */

#include "json_reader.h"

#include <iostream>
#include <iomanip>
#include <queue>

//using std::string_literals::operator""sv;


namespace transport_catalogue {
    namespace output {
        json::Node StopInfo(const TransportCatalogue& catalog, const json::Node& request) {
            std::string_view stop_name = request.AsMap().at("name").AsString();
            json::Dict result;
            result["request_id"] = request.AsMap().at("id").AsInt();

            if (catalog.FindStop(stop_name)) {
                const std::set<std::string_view> info = catalog.GetStopToBusInfo(stop_name);
                json::Array buses;
                for (auto it = info.begin(); it != info.end(); ++it) {
                    buses.push_back(json::Node(std::string(*it)));
                }

                result["buses"] = std::move(buses);
            } else {
                result["error_message"] = "not found";
            }

            return result;
        }

        json::Node BusInfo(const TransportCatalogue& catalog, const json::Node& request) {
            std::string_view bus_num = request.AsMap().at("name").AsString();
            const Bus* ptr = catalog.FindBus(bus_num);
            json::Dict result;
            result["request_id"] = request.AsMap().at("id").AsInt();
            if (ptr != nullptr) {
                auto bus_info = catalog.GetBusInfo(bus_num);
                RouteLength route_length = catalog.ComputeRouteLength(ptr->route_, ptr->flag_);
                result["curvature"] = route_length.actual_length / route_length.length;
                result["route_length"] = route_length.actual_length;
                result["stop_count"] = bus_info.stops_on_route;
                result["unique_stop_count"] = bus_info.unique_stops;
            } else {
                result["error_message"] = "not found";
            }

            return result;
        }

        json::Node FoundInfo( const TransportCatalogue& catalog, const json::Node& request) {
            if (request.AsMap().at("type").AsString() == "Bus") {
                return BusInfo(catalog, request);
            } else {
                return StopInfo(catalog, request);
            }

        }

        void OutputData(std::ostream& out, const TransportCatalogue& catalog, const json::Array& doc) {
            std::queue<const json::Node*> request_queue = transport_catalogue::input::ReadRequests(doc);
            size_t n = request_queue.size();
            json::Array output_json(n);
            for (int i = 0; i < n; ++i) {
                const json::Node* request = request_queue.front();
                
                output_json[i] = FoundInfo(catalog, *request);
                request_queue.pop();
            }

            json::Print(json::Document(output_json), out);
        }
    }
}