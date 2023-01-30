#include <iostream>

#include "transport_catalogue.h"
#include "input_reader.h"
#include "stat_reader.h"
#include "Tests.h"
#include "log_duration.h"

//#define TEST
//#define DURATION

int main() {

#ifdef TEST
    transport_catalogue::Tests::AllTests();
#endif

#ifdef DURATION
    LogDuration log("main", std::cout);
#endif

    transport_catalogue::TransportCatalogue map;
    transport_catalogue::input::ParseInput(map);
    transport_catalogue::output::OutputData(map, std::cout);

}

