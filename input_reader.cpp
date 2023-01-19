#include "input_reader.h"
#include "geo.h"
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

        std::queue<std::string> ReadRequests(std::istream& input) {
            int n = transport_catalogue::input::ReadLineWithNumber();
            std::queue<std::string> queue;

            for (int i = 0; i < n; ++i) {
                std::string tmp_str = transport_catalogue::input::ReadLine(input);
                queue.push(std::move(tmp_str));
            }

            return queue;
        }

        /*std::string_view ParseАppellation(std::string_view& bus_request) {
            bus_request.remove_prefix(bus_request.find_first_not_of(' '));

            std::string_view bus_num;
            size_t pos = bus_request.find_first_of(':');
            bus_num = bus_request.substr(0, pos);
            bus_request.remove_prefix(pos + 1);
            
            return bus_num;
        }*/

        std::string_view ParseАppellation(std::string_view& bus_request) {
            bus_request.remove_prefix(bus_request.find_first_of(' '));
            bus_request.remove_prefix(bus_request.find_first_not_of(' '));

            std::string_view bus_num;
            size_t pos = bus_request.find_first_of(':');
            bus_num = bus_request.substr(0, pos);
            bus_request.remove_prefix(pos + 1);
            
            return bus_num;
        }

        Coordinates ParseCoords(std::string_view request) {
            std::istringstream i{std::string(request)};

            double x, y;
            char comma;
            i >> x >> comma >> y;

            return Coordinates{x, y};
        }

        Stop ParseStopRequest(std::string_view stop_request) {
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

            return Bus{bus_num, std::move(route), std::move(unique_stops), flag};
        }

        void ParseInput(TransportCatalogue& obj) {
            int req_counter = input::ReadLineWithNumber();
            std::queue<std::string> request_queue;

            for (int i = 0; i < req_counter; ++i) {
                std::string request = ReadLine(std::cin);
                Requests req_type = request.substr(0, request.find_first_of(' ')) == "Bus" ? Requests::BUS : Requests::STOP;
                if (req_type == Requests::BUS) {
                    request_queue.push(request);
                } else {
                    Stop bus_stop = ParseStopRequest(request);
                    obj.AddStop(std::move(bus_stop));
                }
            }

            size_t size = request_queue.size();
            for (size_t i = 0; i < size; ++i) {
                std::string request = request_queue.front();
                Bus bus_obj = ParseBusRequest(obj, request);
                obj.AddBus(bus_obj);
                request_queue.pop();
            }
        }
    }
}
