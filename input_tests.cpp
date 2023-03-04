#include "input_tests.h"
#include <fstream>
#include <iomanip>
#include <cstdlib>
#include <iostream>
#include <cassert>
#include <queue>



namespace transport_catalogue {
    namespace input {

        /*class ReadJsonFromFile {
        public:
            ReadJsonFromFile(std::string path_to_file)
                : file_(path_to_file)
            {}

            ~ReadJsonFromFile() {
                file_.close();
            }

        private:
            std::ifstream file_;
        };*/
        namespace tests {

            bool CompareDouble(bool lhs, bool rhs, double eps) {
                return std::abs(lhs - rhs) < eps;
            }

            void ParseStopReq(const json::Document doc) {
                std::ifstream file("//home//hero//projects//cpp-transport-catalogue//test_stops_input.txt");

                json::Document requests = json::Load(file);                
                file.close();

                Stop stop = ParseStopRequest(requests.GetRoot().AsArray()[0].AsMap());
                Stop stop1 = ParseStopRequest(requests.GetRoot().AsArray()[1].AsMap());

                assert(stop.stop_name_ == "Ривьерский мост");
                assert(stop1.stop_name_ == "Морской вокзал");

                assert(CompareDouble(stop.coords_.lat, 43.587795, 0.0001));
                assert(CompareDouble(stop.coords_.lng, 39.716901, 0.0001));

                assert(CompareDouble(stop.coords_.lat, 43.581969, 0.0001));
                assert(CompareDouble(stop.coords_.lng, 39.719848, 0.0001));

                std::cerr << "Test ParseBusReq: OK\n";
            }

            void ParseBusReq(const json::Document doc) {
                TransportCatalogue map;
                transport_catalogue::input::ParseInput(doc, map);

                Stop stop1 = {"Ленинский проспект", {0, 0}};
                Stop stop2 = {"Октябрьский проспект", {0, 0}};
                Stop stop3 = {"Тестовый проспект", {0, 0}};
                
                map.AddStop(stop1);
                map.AddStop(stop2);
                map.AddStop(stop3);

                std::ifstream file("//home//hero//projects//cpp-transport-catalogue//test_buses_input.txt");
                json::Document requests = json::Load(file);                
                file.close();

                Bus bus1 = ParseBusRequest(map, requests.GetRoot().AsArray()[0].AsMap());
                Bus bus2 = ParseBusRequest(map, requests.GetRoot().AsArray()[1].AsMap());
                Bus bus3 = ParseBusRequest(map, requests.GetRoot().AsArray()[2].AsMap());
                Bus bus4 = ParseBusRequest(map, requests.GetRoot().AsArray()[3].AsMap());

                assert(bus1.bus_num_ == "0");
                assert(bus2.bus_num_ == "114");
                assert(bus3.bus_num_ == "115");
                assert(bus4.bus_num_ == "116");

                assert(bus1.route_.size() == 0);
                assert(bus2.route_.size() == 2);
                assert(bus3.route_.size() == 4);
                assert(bus4.route_.size() == 6);

                assert(bus1.unique_stops.size() == 0);
                assert(bus2.unique_stops.size() == 2);
                assert(bus3.unique_stops.size() == 3);
                assert(bus4.unique_stops.size() == 4);

                assert(bus1.unique_stops.size() == 0);
                assert(bus2.unique_stops.size() == 2);
                assert(bus3.unique_stops.size() == 3);
                assert(bus4.unique_stops.size() == 4);

                assert(bus1.flag_ == false);
                assert(bus2.flag_ == false);
                assert(bus3.flag_ == false);
                assert(bus4.flag_ == true);
                
                std::cerr << "Test ParseBusReq: OK\n";
            }

            void AddBus(const json::Document doc) {
                TransportCatalogue map;
                std::ifstream file("//home//hero//projects//cpp-transport-catalogue//test_add_buses.txt");
                json::Document requests = json::Load(file);
                file.close();

                ParseInput(requests, map);
                json::Array arr = requests.GetRoot().AsMap().at("base_requests").AsArray();
                std::queue<int> tmp;
                tmp.push(0);
                tmp.push(1);
                tmp.push(2);
                tmp.push(3);


                transport_catalogue::input::AddBuses(map, arr, tmp);

                map.FindBus("0");
                map.FindBus("114");
                map.FindBus("115");
                map.FindBus("116");

                std::cerr << "Test AddBus: OK\n";
            }

            void FillCatalogDatabase(const json::Document doc) {
                TransportCatalogue map;
                transport_catalogue::input::ParseInput(doc, map);
            }

            void All(const json::Document doc) {
                AddBus(doc);
                FillCatalogDatabase(doc);
                ParseBusReq(doc);
                ParseStopReq(doc);

            }
        }
    }
}