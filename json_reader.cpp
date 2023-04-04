#include "json_reader.h"
#include "map_renderer.h"
#include "request_handler.h"
#include "json_builder.h"

#include <queue>
#include <iostream>
#include <sstream>

namespace transport_catalogue {

    void ParseRequests(const json::Document& doc, TransportCatalogue& obj) {
        const json::Dict* requests = &doc.GetRoot().AsMap();
        const json::Array* base_req = &requests->at("base_requests").AsArray();
        const json::Array& stat_request = doc.GetRoot().AsMap().at("stat_requests").AsArray();

        input::FillDatabase(obj, *base_req);

        map_renderer::MapRenderer renderer(map_renderer::input::ParseRenderSettings(doc));
        TransportRouter route_manager(ParseRoutingSettings(doc), obj);
        RequestHandler request_handler(obj, renderer, route_manager);

        output::OutputData(std::cout, obj, stat_request, request_handler);
    }

    transport_catalogue::RoutingSettings ParseRoutingSettings(const json::Document& document) {
        const auto& settings = document.GetRoot().AsMap().at("routing_settings"s).AsMap();
        return {
            settings.at("bus_wait_time"s).AsDouble(),
            settings.at("bus_velocity"s).AsDouble(),
        };
    }

    namespace input {
        
        std::string ReadLine(std::istream& input) {
            std::string s;
            std::getline(input, s);
            
            return s;
        }   

        int ReadLineWithNumber () {
            int result;
            std::cin >> result;
            ReadLine(std::cin);

            return result;
        }

        std::queue<const json::Node*> ReadRequests(const json::Array& doc) {
            std::queue<const json::Node*> queue;

            for (size_t i = 0; i < doc.size(); ++i) {
                const json::Node* ptr = &doc[i];
                queue.push(ptr);
            }

            return queue;
        }

        /*
        * Sets the "real" distances between stops in the transport directory
        */
        void SetDistances(TransportCatalogue& obj, std::vector<domain::ActualDistance>& distances) {
            for (const auto& actual: distances) {
                std::string_view from = actual.from;
                for (const auto& [distance, to]: actual.dist) {
                    obj.SetActualDistance(from, to, distance);
                }
            }
        }

        void AddBuses(TransportCatalogue& obj, const json::Array& base_request, std::queue<int>& request_queue) {
            size_t size = request_queue.size();
            for (size_t i = 0; i < size; ++i) {
                int index = request_queue.front();
                const json::Dict* request = &base_request[index].AsMap();
                obj.AddBus(ParseBusRequest(obj, *request));
                request_queue.pop();
            }
        }

        void FillDatabase(TransportCatalogue& obj, const json::Array& base_request) {
            std::queue<int> request_queue;
            std::vector<domain::ActualDistance> distances;
            for (size_t i = 0; i < base_request.size(); ++i) {
                const json::Dict* request = &base_request[i].AsMap();
                if (request->at("type") == "Bus") {
                    request_queue.push(i);
                } else {
                    obj.AddStop(ParseStopRequest(*request));
                    ParseAddInfo(*request, distances);
                }
            }

            SetDistances(obj, distances);

            AddBuses(obj, base_request, request_queue);
        }

        Stop ParseStopRequest(const json::Dict& stop_request) {
            std::string_view stop_name = stop_request.at("name").AsString();

            Coordinates coords;
            coords.lat = stop_request.at("latitude").AsDouble();
            coords.lng = stop_request.at("longitude").AsDouble();

            return Stop(stop_name, coords);
        }

        void ParseAddInfo(const json::Dict& stop_request, std::vector<ActualDistance>& dist) {
            if (stop_request.count("road_distances")) {
                const json::Dict* distances = &stop_request.at("road_distances").AsMap();
                domain::ActualDistance dist_buff;
                dist_buff.from = stop_request.at("name").AsString();
                
                for (auto it = distances->begin(); it != distances->end(); ++it) {
                    dist_buff.dist.push_back({it->second.AsDouble(), it->first});
                }

                dist.push_back(std::move(dist_buff));
            }
        }

        Bus ParseBusRequest(TransportCatalogue& obj, const json::Dict& bus_request) {
            std::string_view bus_num = bus_request.at("name").AsString();
            const json::Array* stops = &bus_request.at("stops").AsArray();
            size_t size = stops->size();
            std::vector<const Stop*> route(size);
            std::unordered_set<const Stop*> unique_stops;
            bool is_round = bus_request.at("is_roundtrip").AsBool();

            for (size_t i = 0; i < size; ++i) {
                const Stop* ptr = obj.FindStop((*stops)[i].AsString());
                route[i] = ptr;
                if (!unique_stops.count(ptr)) {
                    unique_stops.insert(ptr);
                }
            }

            return Bus(bus_num, std::move(route), std::move(unique_stops), is_round);
        }
    }

    namespace output {
        json::Node StopInfo(const TransportCatalogue& catalog, const json::Node& request) {
            std::string_view stop_name = request.AsMap().at("name").AsString();
            json::Builder result;
            result.StartDict()
                .Key("request_id").Value(request.AsMap().at("id").AsInt());

            if (catalog.FindStop(stop_name)) {
                const std::set<std::string_view> info = catalog.GetStopToBusInfo(stop_name);
                json::Array buses;
                for (auto it = info.begin(); it != info.end(); ++it) {
                    buses.push_back(json::Node(std::string(*it)));
                }

                result.Key("buses").Value(std::move(buses));
            } else {
                result.Key("error_message").Value("not found");
            }

            return result.EndDict().Build().AsMap();
        }

        json::Node BusInfo(const TransportCatalogue& catalog, const json::Node& request) {
            std::string_view bus_num = request.AsMap().at("name").AsString();
            const Bus* ptr = catalog.FindBus(bus_num);
            json::Builder result;
            result.StartDict().Key("request_id").Value(request.AsMap().at("id").AsInt());
            
            if (ptr != nullptr) {
                auto bus_info = catalog.GetBusInfo(bus_num);
                RouteLength route_length = catalog.ComputeRouteLength(ptr->route_, ptr->flag_);
                result.Key("curvature").Value(route_length.actual_length / route_length.length);
                result.Key("route_length").Value(route_length.actual_length);
                result.Key("stop_count").Value(bus_info.stops_on_route);
                result.Key("unique_stop_count").Value(bus_info.unique_stops);
            } else {
                
                result.Key("error_message").Value("not found");
            }

            return result.EndDict().Build().AsMap();
        }

        json::Node FoundInfo( const TransportCatalogue& catalog, const json::Node& request, const RequestHandler& req_handler) {
            const std::string& rec_type = request.AsMap().at("type").AsString();
            if (rec_type == "Bus") {
                return BusInfo(catalog, request);
            } else if (rec_type == "Stop") {
                return StopInfo(catalog, request);
            } else if (rec_type == "Map") {
                return MapRequest(request, req_handler);
            } else if (rec_type == "Route") {
                return ParseOutputRouteRequest(req_handler, request);
            }
            return rec_type == "Bus" ? BusInfo(catalog, request) : StopInfo(catalog, request);
        }

        void OutputData(std::ostream& out, const TransportCatalogue& catalog, const json::Array& doc, const RequestHandler& req_handler) {
            std::queue<const json::Node*> request_queue = transport_catalogue::input::ReadRequests(doc);
            size_t n = request_queue.size();
            json::Array output_json(n);
            for (size_t i = 0; i < n; ++i) {
                const json::Node* request = request_queue.front();
                
                output_json[i] = FoundInfo(catalog, *request, req_handler);
                request_queue.pop();
            }

            json::Print(json::Document(output_json), out);
        }

        json::Node MapRequest(const json::Node& req, const RequestHandler& req_handler) {
            std::ostringstream out;
            req_handler.RenderMap().Render(out);

            json::Node result{
                json::Builder{}
                .StartDict()
                    .Key("request_id"s).Value(req.AsMap().at("id"s).AsInt())
                    .Key("map").Value(out.str())
                .EndDict()
                .Build().AsMap()
            };

            return result;
        }

        json::Node ParseOutputRouteRequest(const RequestHandler& req_handler, const json::Node& req) {
            const auto from = req.AsMap().at("from"s).AsString();
            const auto to = req.AsMap().at("to"s).AsString();

            if (auto result = req_handler.BuildRoute(from, to)) {
                const auto [total_time, route_items] = *result;

                json::Builder itemsBuilder;
                auto arrayBuilder = itemsBuilder.StartArray();
                for (const auto& item : route_items) {
                    if (item.type == RouteItemType::Wait) {
                        arrayBuilder.Value(json::Builder{}
                            .StartDict()
                                .Key("type"s).Value("Wait"s)
                                .Key("stop_name"s).Value(item.stop_name)
                                .Key("time"s).Value(item.time)
                            .EndDict()
                            .Build()
                            .AsMap()
                        );
                    } else {
                        arrayBuilder.Value(json::Builder{}
                            .StartDict()
                                .Key("type"s).Value("Bus"s)
                                .Key("bus"s).Value(item.bus_name)
                                .Key("span_count"s).Value(item.span_count)
                                .Key("time"s).Value(item.time)
                            .EndDict()
                            .Build()
                            .AsMap()
                        );
                    }
                }
                arrayBuilder.EndArray();

                return json::Builder{}
                    .StartDict()
                        .Key("request_id"s).Value(req.AsMap().at("id"s).AsInt())
                        .Key("total_time"s).Value(total_time)
                        .Key("items"s).Value(itemsBuilder.Build().AsArray())
                    .EndDict()
                    .Build();
            } else {
                return json::Builder{}
                    .StartDict()
                        .Key("request_id"s).Value(req.AsMap().at("id"s).AsInt())
                        .Key("error_message"s).Value("not found"s)
                    .EndDict()
                    .Build();
            }
        }
    }
}