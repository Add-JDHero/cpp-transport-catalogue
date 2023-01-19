#pragma once

#include "transport_catalogue.h"
#include <string_view>


namespace transport_catalogue {
    namespace output {
        void BusInfo(std::ostream& out, const TransportCatalogue& catalog, std::string_view appellation);
        void FoundInfo(std::ostream& out, const TransportCatalogue& catalog, std::string_view request);
        void OutputData(const TransportCatalogue& catalog, std::ostream& out);
    }
}