#include <iostream>

#include "json.h"
#include "transport_catalogue.h"
#include "json_reader.h"
#include "request_handler.h"
//#include "input_tests.h"
#include "map_renderer.h"

//#define TEST
//#define DURATION
#define MAIN_PROG


int main() {

#ifdef TEST
    transport_catalogue::input::tests::All(json::Load(std::cin));
#endif

#ifdef DURATION
    LogDuration log("main", std::cout);
#endif

#ifdef MAIN_PROG
    using namespace transport_catalogue;
    using namespace map_renderer;
    transport_catalogue::TransportCatalogue map;

    const json::Document& document = json::Load(std::cin);

    transport_catalogue::ParseRequests(document, map);
#endif
}