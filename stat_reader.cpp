#include "stat_reader.h"
#include "input_reader.h"

#include <iostream>
#include <iomanip>
#include <queue>

//using std::string_literals::operator""sv;


namespace transport_catalogue {
    namespace output {
        void StopInfo(std::ostream& out, const TransportCatalogue& catalog, std::string_view appellation) {
            if (catalog.FindStop(appellation)) {
                const std::set<std::string> info = catalog.GetStopToBusInfo(appellation);
                if (!info.empty()) {
                    out << "buses ";
                    bool flag = false;
                    for (const auto& bus_num: info) {
                        if (flag) {
                            out << " ";
                        }
                        flag = true;
                        out << bus_num;
                    }
                    out << "\n";
                } else {
                    out << "no buses\n";
                }
            } else {
                out << "not found\n";
            }
        }

        void BusInfo(std::ostream& out, const TransportCatalogue& catalog, std::string_view appellation) {
            if (catalog.FindBus(appellation)) {
                auto bus_info = catalog.GetBusInfo(appellation);
                double length = bus_info.route_length.length;
                double actual_length = bus_info.route_length.actual_length;
                out << bus_info.stops_on_route << " stops on route, "
                    << bus_info.unique_stops << " unique stops, "
                    << std::setprecision(6) 
                    << actual_length << " route length, "
                    << actual_length / length << " curvature\n";
            } else {
                out << "not found\n";
            }
        }


        void FoundInfo(std::ostream& out, const TransportCatalogue& catalog, std::string_view request) {
            std::string_view tmp = request;
            std::string_view appellation = transport_catalogue::input::ParseАppellation(tmp);
            Requests req_type = request.substr(0, request.find_first_of(' ')) == "Bus" ? Requests::BUS : Requests::STOP;
            
            out << request << ": ";
            req_type == Requests::BUS ? BusInfo(out, catalog, appellation) : StopInfo(out, catalog, appellation);
        }

        void OutputData(const TransportCatalogue& catalog, std::ostream& out) {
            std::queue<std::string> request_queue = transport_catalogue::input::ReadRequests(std::cin);
            size_t n = request_queue.size();
            for (int i = 0; i < n; ++i) {
                std::string_view request = request_queue.front();
                
                FoundInfo(out, catalog, request);
                request_queue.pop();
            }
        }
    }
}
