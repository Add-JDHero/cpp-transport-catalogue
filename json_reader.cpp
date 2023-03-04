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

        /*std::string_view ParseАppellation(std::string_view& request) {
            request.remove_prefix(request.find_first_of(' '));
            request.remove_prefix(request.find_first_not_of(' '));

            std::string_view appellation;
            size_t pos = request.find_first_of(':');
            appellation = request.substr(0, pos);
            request.remove_prefix(pos + 1);
            
            return appellation;
        }

        Coordinates ParseCoords(std::string_view& request) {
            request.remove_prefix(request.find_first_not_of(' '));
            double x, y;
            size_t pos = request.find_first_of(',');
            std::string tmp = std::string(request.substr(0, pos));
            x = std::stod(std::string(request.substr(0, pos + 1)));
            request.remove_prefix(pos + 1);
            request.remove_prefix(request.find_first_not_of(' '));
            y = std::stod(std::string(request.substr(0, request.find_first_of(','))));
            pos = request.find_first_of(' ');
            if (pos == std::string_view::npos) {
                request = "";
            }
            request.remove_prefix(request.find_first_of(' '));
            
            return Coordinates{x, y};
        }

        std::vector<std::pair<double, std::string>> ParseAddInfo(std::string_view& stop_request) {
            std::vector<std::pair<double, std::string>> result;
            std::string_view distance;
            std::string_view destination;
            while (size_t pos = stop_request.find_first_not_of(' ')) {
                stop_request.remove_prefix(pos);
                std::string_view distance = stop_request.substr(0, stop_request.find_first_of('m'));
                stop_request.remove_prefix(stop_request.find_first_of('t') + 2);
                stop_request.remove_prefix(stop_request.find_first_not_of(' '));
                size_t position = stop_request.find_first_of(',');
                std::string_view destination = stop_request.substr(0, position);
                result.push_back({std::stod(std::string(distance)), std::string(destination)});
                stop_request.remove_prefix(position + 1);
            }

            return result;
        }

        Stop ParseStopRequest(std::string_view& stop_request) {
            std::string stop_name = std::string(ParseАppellation(stop_request));
            Coordinates coords = ParseCoords(stop_request);

            return Stop(stop_name, coords);
        }

        std::string_view ExtractStopName(std::string_view& bus_request, char separator) {
            std::string_view stop_name;
            bus_request.remove_prefix(bus_request.find_first_not_of(' '));
            size_t pos = bus_request.find(separator);
            if (pos == std::string_view::npos) {
                stop_name = bus_request;
                bus_request.remove_prefix(bus_request.size());
            } else {
                stop_name = bus_request.substr(0, pos);
                stop_name = stop_name.substr(0, stop_name.find_last_not_of(' ') + 1);                
                bus_request.remove_prefix(pos + 1);
            }

            return stop_name;
        }

        Bus ParseBusRequest(TransportCatalogue& obj, std::string_view bus_request) {
            std::string_view bus_num = ParseАppellation(bus_request);
            std::vector<const Stop*> route;
            std::unordered_set<const Stop*> unique_stops;
            bool flag = false;
            char separator = '-';
            if (bus_request.find('>') != std::string_view::npos) {
                separator = '>';
                flag = true;
            }

            while (!bus_request.empty()) {
                std::string_view stop_name = ExtractStopName(bus_request, separator);
                const Stop* ptr = obj.FindStop(stop_name);
                route.push_back(ptr);
                unique_stops.insert(ptr);
            }

            return domain::Bus{bus_num, std::move(route), std::move(unique_stops), flag};
        }

        void ParseInput(TransportCatalogue& obj) {
            int req_counter = input::ReadLineWithNumber();
            std::queue<std::string> request_queue;
            std::vector<domain::ActualDistance> distances;

            for (int i = 0; i < req_counter; ++i) {
                std::string request = ReadLine(std::cin);
                Requests req_type = request.substr(0, request.find_first_of(' ')) == "Bus" ? domain::Requests::BUS : domain::Requests::STOP;
                if (req_type == domain::Requests::BUS) {
                    request_queue.push(request);
                } else {
                    std::string_view view = request;
                    Stop bus_stop = ParseStopRequest(view);
                    if (view.find_first_of(' ') != std::string_view::npos) {
                        distances.push_back({bus_stop.stop_name_, ParseAddInfo(view)});
                    }
                    obj.AddStop(std::move(bus_stop));
                }
            }

            for (const auto& actual: distances) {
                std::string_view from = actual.from;
                for (const auto& [distance, to]: actual.dist) {
                    obj.SetActualDistance(from, to, distance);
                }
            }

            size_t size = request_queue.size();
            for (size_t i = 0; i < size; ++i) {
                std::string request = request_queue.front();
                Bus bus_obj = ParseBusRequest(obj, request);
                obj.AddBus(bus_obj);
                request_queue.pop();
            }
        }*/



        std::queue<const json::Node*> ReadRequests(const json::Array& doc) {
            std::queue<const json::Node*> queue;

            for (int i = 0; i < doc.size(); ++i) {
                const json::Node* ptr = &doc[i];
                queue.push(ptr);
            }

            return queue;
        }

    //  ---------- NEW SPRINT FUNCTIONS ----------
    
        /*void ParseBaseRequests(TransportCatalogue& obj, const json::Array& base);
        Stop ParseStopRequest(const json::Dict& stop);
        void ParseAddInfo(const json::Dict& stop, std::vector<domain::ActualDistance>& dist);
        void AddBuses(TransportCatalogue& obj, const json::Array& base_request, std::queue<int>& request_queue);
        void ParseInput(json::Document& doc, TransportCatalogue& obj);
        Bus ParseBusRequest(TransportCatalogue& obj, const json::Dict& bus_request);*/

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

/*

Bus :  name, route, unique_stops, flag




*/