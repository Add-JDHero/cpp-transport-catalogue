#include "json_reader.h"
#include "transport_catalogue.h"


namespace transport_catalogue {
    namespace input {
        namespace tests {
            void AddBus(const json::Document doc);
            void ParseStopReq(const json::Document doc);
            void ParseBusReq(const json::Document doc);
            void FillCatalogDatabase(const json::Document doc);

            void All(const json::Document doc);
        }
    }
}