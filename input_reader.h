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

        std::queue<std::string> ReadBusRequest(std::istream& input);

        std::string ParseStop(std::string_view& stop_reuqest);
        Coordinates ParseCoords(std::string_view request);
        transport_catalogue::Stop ParseStopRequest(std::string_view stop_request);

        std::string_view ParseBusNum(std::string_view& bus_request);
        std::string_view ExtractStopName(std::string_view& bus_request, char separator);
        Bus ParseBusRequest(TransportCatalogue& obj, std::string_view bus_request);

        void ParseInput(transport_catalogue::TransportCatalogue& obj, std::istream& input);
    }

}