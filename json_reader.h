#pragma once
#include <sstream>
#include <iostream>
#include <vector>
#include <queue>

#include "domain.h"
#include "geo.h"
#include "transport_catalogue.h"
#include "json.h"


namespace transport_catalogue {
    namespace input {
        std::string ReadLine(std::istream& input);
        int ReadLineWithNumber();

        std::queue<const json::Node*> ReadRequests(const json::Array& doc);

        void ParseInput(const json::Document& doc, TransportCatalogue& obj);
        void FillDatabase(TransportCatalogue& obj, const json::Array& base);

        Stop ParseStopRequest(const json::Dict& stop);
        void ParseAddInfo(const json::Dict& stop, std::vector<domain::ActualDistance>& dist);

        void AddBuses(TransportCatalogue& obj, const json::Array& base_request, std::queue<int>& request_queue);
        Bus ParseBusRequest(TransportCatalogue& obj, const json::Dict& bus_request);
    }

}