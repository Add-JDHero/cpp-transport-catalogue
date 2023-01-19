#include "Tests.h"
#include "transport_catalogue.h"
#include "input_reader.h"

#include <cassert>
#include <iostream>

namespace transport_catalogue {
    namespace Tests {
        void AddStop() {
            transport_catalogue::TransportCatalogue map;
            map.AddStop(Stop{"A", {0.2, 0.5}});

            assert(map.StopsCount() == 1);

            std::cout << "Add stop: OK\n";
        }

        void ParseStopRequest() {
            transport_catalogue::TransportCatalogue map;
            std::vector<std::string> Stops{
                "Tolstopaltsevo: 55.611087, 37.208290",
                "Marushkino: 55.595884, 37.209755",
                "Rasskazovka: 55.632761, 37.333324",
                "Biryulyovo Zapadnoye: 55.574371, 37.651700",
                "Biryusinka: 55.581065, 37.648390",
                " Universam: 55.587655, 37.645687",
                "  Biryulyovo Tovarnaya: 55.592028, 37.653656",
                "Biryulyovo Passazhirskaya   : 55.580999, 37.659164"
            };

            
            for (const std::string& str: Stops) {
                Stop tmp_stop = transport_catalogue::input::ParseStopRequest(str);
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
                "256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye",
                "750: Tolstopaltsevo - Marushkino - Rasskazovka"
            };

            std::vector<std::string> Stops{
                "Tolstopaltsevo: 55.611087, 37.208290",
                "Marushkino: 55.595884, 37.209755",
                "Rasskazovka: 55.632761, 37.333324",
                "Biryulyovo Zapadnoye: 55.574371, 37.651700",
                "Biryusinka: 55.581065, 37.648390",
                " Universam: 55.587655, 37.645687",
                "  Biryulyovo Tovarnaya: 55.592028, 37.653656",
                "Biryulyovo Passazhirskaya   : 55.580999, 37.659164"
            };

            
            for (const std::string& str: Stops) {
                Stop tmp_stop = transport_catalogue::input::ParseStopRequest(str);
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
            ParseStopRequest();
            ParseBusRequest();
            std::cout << "All tests: OK\n";
        }
    }
}
