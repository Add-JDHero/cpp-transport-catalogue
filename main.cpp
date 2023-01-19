#include <iostream>

#include "transport_catalogue.h"
#include "input_reader.h"
#include "stat_reader.h"
#include "Tests.h"
#include "log_duration.h"

//#define DEBUG
//#define DURATION

int main() {
#ifdef DEBUG
    using transport_catalogue::Tests::AllTests;
    AllTests();
#endif
#ifdef DURATION
    {   LOG_DURATION("map");
        transport_catalogue::TransportCatalogue map;
        transport_catalogue::input::ParseInput(map);
        transport_catalogue::output::OutputData(map, std::cout);
    }
#endif

    transport_catalogue::TransportCatalogue map;
    transport_catalogue::input::ParseInput(map);
    transport_catalogue::output::OutputData(map, std::cout);
}

