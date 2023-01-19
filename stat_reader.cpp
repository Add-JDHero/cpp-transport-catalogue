#include "stat_reader.h"
#include "input_reader.h"

#include <iostream>
#include <iomanip>
#include <queue>

namespace transport_catalogue {
    namespace output {
        void OutputData(const TransportCatalogue& catalog, std::ostream& out) {
            std::queue<std::string> request_queue = transport_catalogue::input::ReadBusRequest(std::cin);
            size_t n = request_queue.size();
            for (int i = 0; i < n; ++i) {
                std::string_view bus_req = request_queue.front();
                std::string_view bus_num = bus_req.substr(bus_req.find_first_of(' ') + 1);
                out << bus_req << ": ";
                if (catalog.FindBus(bus_num)) {
                    auto bus_info = catalog.GetBusInfo(bus_num);
                    out << bus_info.stops_on_route << " stops on route, "
                        << bus_info.unique_stops << " unique stops, "
                        << std::setprecision(6) 
                        << bus_info.route_length << " route length\n";
                } else {
                    out << "not found\n";
                }

                request_queue.pop();
            }
        }
    }
}
