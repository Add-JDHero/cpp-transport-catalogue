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

void PrintUsage(std::ostream& stream = std::cerr) {
    stream << "Usage: transport_catalogue [make_base|process_requests]\n"sv;
}

int main(int argc, char* argv[]) {

#ifdef TEST
    transport_catalogue::input::tests::All(json::Load(std::cin));
#endif

#ifdef DURATION
    LogDuration log("main", std::cout);
#endif

#ifdef MAIN_PROG
    if (argc < 2) {
        PrintUsage();
        return 1;
    }

    const std::string_view mode(argv[1]);

    using namespace transport_catalogue;
    using namespace map_renderer;

    transport_catalogue::TransportCatalogue map;
    const json::Document& document = json::Load(std::cin);

    if (mode == "make_base"sv) {
        MakeBase(document, map);
    } else if (mode == "process_requests"sv) {
        ProcessRequests(document, map);
    } else {
        PrintUsage();
        return 1;
    }
#endif
}