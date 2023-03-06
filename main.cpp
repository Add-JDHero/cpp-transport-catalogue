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

    /*
     * Примерная структура программы:
     *
     * Считать JSON из stdin
     * Построить на его основе JSON базу данных транспортного справочника
     * Выполнить запросы к справочнику, находящиеся в массиве "stat_requests", построив JSON-массив
     * с ответами.
     * Вывести в stdout ответы в виде JSON
     */
#ifdef MAIN_PROG
    using namespace transport_catalogue;
    using namespace map_renderer;
    transport_catalogue::TransportCatalogue map;

    const json::Document& document = json::Load(std::cin);

    transport_catalogue::ParseRequests(document, map);

    /*map_renderer::MapRenderer renderer(map_renderer::input::ParseRenderSettings(document));
    RequestHandler request_handler(map, renderer);
    request_handler.RenderMap().Render(std::cout);*/

#endif
}