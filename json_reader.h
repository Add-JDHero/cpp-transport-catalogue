#pragma once
#include <sstream>
#include <iostream>
#include <vector>
#include <queue>

#include "domain.h"
#include "map_renderer.h"
#include "request_handler.h"
#include "geo.h"
#include "transport_catalogue.h"
#include "json.h"


namespace transport_catalogue {
    void ParseRequests(const json::Document& doc, TransportCatalogue& obj);

    namespace input {
        std::string ReadLine(std::istream& input);
        int ReadLineWithNumber();

        std::queue<const json::Node*> ReadRequests(const json::Array& doc);

        void FillDatabase(TransportCatalogue& obj, const json::Array& base);

        Stop ParseStopRequest(const json::Dict& stop);
        void ParseAddInfo(const json::Dict& stop, std::vector<domain::ActualDistance>& dist);

        void AddBuses(TransportCatalogue& obj, const json::Array& base_request, std::queue<int>& request_queue);
        Bus ParseBusRequest(TransportCatalogue& obj, const json::Dict& bus_request);
    }

    namespace output {
        json::Node StopInfo(const TransportCatalogue& catalog, const json::Node& request);
        json::Node BusInfo(const TransportCatalogue& catalog, const json::Node& request);
        json::Node FoundInfo( const TransportCatalogue& catalog, const json::Node& request, const RequestHandler& req_handler);

        json::Node MapRequest(const json::Node& req, const RequestHandler& req_handler);

        void OutputData(std::ostream& out, const TransportCatalogue& catalog, const json::Array& doc, const RequestHandler& req_handler);
    }

}