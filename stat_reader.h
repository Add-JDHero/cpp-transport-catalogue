#pragma once

#include "transport_catalogue.h"
#include <string_view>


namespace transport_catalogue {
    namespace output {
        void OutputData(const TransportCatalogue& catalog, std::ostream& out);
    }
}