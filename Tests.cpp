#include "Tests.h"
#include "transport_catalogue.h"
#include "input_reader.h"

#include <cassert>
#include <iostream>

namespace transport_catalogue {
    namespace Tests {
        void AddStop() {
            transport_catalogue::TransportCatalogue map;
            map.AddStop(Stop{"A", {0.1, 0.5}});
            map.AddStop(Stop{"B", {0.2, 0.5}});
            map.AddStop(Stop{"C", {0.3, 0.5}});
            map.AddStop(Stop{"D", {0.4, 0.5}});

            assert(map.StopsCount() == 4);

            std::cout << "Add stop: OK\n";
        }

        bool operator==(const std::vector<std::pair<double, std::string>>& lhs, const std::vector<std::pair<double, std::string>>& rhs) {
            size_t size = lhs.size();
            if (rhs.size() != size) { return false; }
            bool flag = true;
            int i = 0;
            while (flag && (i < size)) {
                flag = (lhs[i].first == rhs[i].first) && (lhs[i].second == rhs[i].second);
                ++i; 
            }

            return flag;
        }

        bool operator==(const ActualDistance& lhs, const ActualDistance& rhs) {

            return (lhs.from == rhs.from) && lhs.dist == rhs.dist;
        }

        void ParseAddInfo() {
            transport_catalogue::TransportCatalogue map;
            std::vector<std::string_view> Stops{
                "Stop Tolstopaltsevo: 55.611087, 37.20829, 3900m to Marushkino",
                "Stop Marushkino: 55.595884, 37.209755, 9900m to Rasskazovka, 100m to Marushkino",
                "Stop Rasskazovka: 55.632761, 37.333324, 9500m to Marushkino",
                "Stop Biryulyovo Zapadnoye: 55.574371, 37.6517, 7500m to Rossoshanskaya ulitsa, 1800m to Biryusinka, 2400m to Universam",
                "Stop Biryusinka: 55.581065, 37.64839, 750m to Universam",
                "Stop Universam: 55.587655, 37.645687, 5600m to Rossoshanskaya ulitsa, 900m to Biryulyovo Tovarnaya",
                "Stop Biryulyovo Tovarnaya: 55.592028, 37.653656, 1300m to Biryulyovo Passazhirskaya",
                "Stop Biryulyovo Passazhirskaya: 55.580999, 37.659164, 1200m to Biryulyovo Zapadnoye",
                "Stop Rossoshanskaya ulitsa: 55.595579, 37.605757",
                "Stop Prazhskaya: 55.611678, 37.603831",
            };

            std::vector<ActualDistance> buffer;
            for (auto stop_request: Stops) {
                const Stop stop = transport_catalogue::input::ParseStopRequest(stop_request);
                if (size_t pos = stop_request.find_first_not_of(' ')) {
                    ActualDistance actual{stop.stop_name_, transport_catalogue::input::ParseAddInfo(stop_request)};
                    buffer.push_back(std::move(actual));
                }
            }

            ActualDistance tmp{"Biryulyovo Zapadnoye", {{7500, "Rossoshanskaya ulitsa"}, {1800, "Biryusinka"}, {2400, "Universam"}}};
            assert(buffer[3] == tmp);
            tmp = {"Universam", {{5600, "Rossoshanskaya ulitsa"}, {900, "Biryulyovo Tovarnaya"}}};
            assert(buffer[5] == tmp);
            tmp = {"Universam", {{5600, "Rossoshanskaya ulitsa"}}};
            assert(!(buffer[5] == tmp));
            assert(!(buffer[6] == tmp));

            std::cout << "ParseAddInfo: OK\n";
        }

        void ParseАppellation() {
            using namespace std::literals;
            transport_catalogue::TransportCatalogue map;
            std::vector<std::string> Stops{
                "Stop Tolstopaltsevo: 55.611087, 37.208290",
                "StopMarushkino: 55.595884, 37.209755",
                "StopRasskazovka: 55.632761, 37.333324",
                "StopBiryulyovo Zapadnoye: 55.574371, 37.651700",
                "StopBiryusinka: 55.581065, 37.648390",
                "Stop Universam: 55.587655, 37.645687",
                "Stop  Biryulyovo Tovarnaya: 55.592028, 37.653656",
                "Stop Biryulyovo Passazhirskaya: 55.580999, 37.659164"
            };

            std::string_view tmp = "Stop  Biryulyovo Tovarnaya: 55.592028, 37.653656";
            assert(transport_catalogue::input::ParseАppellation(tmp) == "Biryulyovo Tovarnaya"sv);
            tmp = "Stop Biryulyovo Passazhirskaya: 55.580999, 37.659164";
            assert(transport_catalogue::input::ParseАppellation(tmp) == "Biryulyovo Passazhirskaya"sv);
        }

        void ParseStopRequest() {
            transport_catalogue::TransportCatalogue map;
            std::vector<std::string> Stops{
                "Stop Tolstopaltsevo: 55.611087, 37.208290",
                "Stop Marushkino: 55.595884, 37.209755",
                "Stop Rasskazovka: 55.632761, 37.333324",
                "Stop Biryulyovo Zapadnoye: 55.574371, 37.651700",
                "Stop Biryusinka: 55.581065, 37.648390",
                "Stop  Universam: 55.587655, 37.645687",
                "Stop  Biryulyovo Tovarnaya: 55.592028, 37.653656",
                "Stop Biryulyovo Passazhirskaya: 55.580999, 37.659164"
            };
            
            for (const std::string& str: Stops) {
                std::string_view tmp = str;
                Stop tmp_stop = transport_catalogue::input::ParseStopRequest(tmp);
                map.AddStop(tmp_stop);
            }
            
            assert(map.StopsCount() == Stops.size());

            assert(map.FindStop("Biryulyovo Passazhirskaya") != nullptr);
            assert(map.FindStop("Biryulyovo Tovarnaya") != nullptr);

            std::cout << "Parse stop request: OK\n";
        }

        void ParseBusRequest() {
            transport_catalogue::TransportCatalogue map;
            std::vector<std::string> Buses{
                "Bus 256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye",
                "Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka"
            };

            std::vector<std::string> Stops{
                "Stop Tolstopaltsevo: 55.611087, 37.208290",
                "Stop  Marushkino: 55.595884, 37.209755",
                "Stop Rasskazovka: 55.632761, 37.333324",
                "Stop Biryulyovo Zapadnoye: 55.574371, 37.651700",
                "Stop Biryusinka: 55.581065, 37.648390",
                "Stop  Universam: 55.587655, 37.645687",
                "Stop  Biryulyovo Tovarnaya: 55.592028, 37.653656",
                "Stop Biryulyovo Passazhirskaya: 55.580999, 37.659164"
            };

            
            for (std::string& str: Stops) {
                std::string_view tmp = str;
                Stop tmp_stop = transport_catalogue::input::ParseStopRequest(tmp);
                map.AddStop(tmp_stop);
            }

            for (const std::string& str: Buses) {
                Bus tmp_bus = transport_catalogue::input::ParseBusRequest(map, str);
                map.AddBus(tmp_bus);
            }

            std::cout << "Parse bus request: OK\n";
        }

        void AllTests() {
            AddStop();
            ParseAddInfo();
            ParseАppellation();
            ParseStopRequest();
            ParseBusRequest();
            std::cout << "All tests: OK\n\n";
        }
    }
}
