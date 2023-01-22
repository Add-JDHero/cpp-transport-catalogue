#pragma once
#include <sstream>
#include <iostream>
#include <vector>
#include <queue>
#include "transport_catalogue.h"


namespace transport_catalogue {
    namespace input {
        std::string ReadLine(std::istream& input);
        int ReadLineWithNumber();

        std::queue<std::string> ReadRequests(std::istream& input);

        std::vector<std::pair<double, std::string>> ParseAddInfo(std::string_view& stop_request);
        std::string_view ParseАppellation(std::string_view& reuqest);
        Coordinates ParseCoords(std::string_view& request);
        transport_catalogue::Stop ParseStopRequest(std::string_view& stop_request);

        std::string_view ExtractStopName(std::string_view& bus_request, char separator);
        Bus ParseBusRequest(TransportCatalogue& obj, std::string_view bus_request);

        void ParseInput(transport_catalogue::TransportCatalogue& obj);
    }

}