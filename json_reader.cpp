#include "json_reader.h"
#include <queue>
#include <iostream>

namespace transport_catalogue {
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

            for (int i = 0; i < doc.size(); ++i) {
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

        void ParseInput(const json::Document& doc, TransportCatalogue& obj) {
            const json::Dict* requests = &doc.GetRoot().AsMap();
            const json::Array* base_req = &requests->at("base_requests").AsArray();
            
            FillDatabase(obj, *base_req);
        }

        void FillDatabase(TransportCatalogue& obj, const json::Array& base_request) {
            std::queue<int> request_queue;
            std::vector<domain::ActualDistance> distances;
            for (int i = 0; i < base_request.size(); ++i) {
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

            for (int i = 0; i < size; ++i) {
                const Stop* ptr = obj.FindStop((*stops)[i].AsString());
                route[i] = ptr;
                if (!unique_stops.count(ptr)) {
                    unique_stops.insert(ptr);
                }
            }

            return Bus(bus_num, std::move(route), std::move(unique_stops), is_round);
        }
    }
}