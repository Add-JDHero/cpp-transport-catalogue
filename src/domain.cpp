#include "domain.h"

/*
 * В этом файле вы можете разместить классы/структуры, которые являются частью предметной области
 * (domain) вашего приложения и не зависят от транспортного справочника. Например Автобусные
 * маршруты и Остановки.
 *
 * Их можно было бы разместить и в transport_catalogue.h, однако вынесение их в отдельный
 * заголовочный файл может оказаться полезным, когда дело дойдёт до визуализации карты маршрутов:
 * визуализатор карты (map_renderer) можно будет сделать независящим от транспортного справочника.
 *
 * Если структура вашего приложения не позволяет так сделать, просто оставьте этот файл пустым.
 *
 */

namespace transport_catalogue {
    namespace domain {

        std::vector<StopPtr> MakeRoute(BusPtr bus) {
            std::vector<StopPtr> stops;
            MakeRoute(bus->route_.begin(), bus->route_.end(), stops, bus->flag_);
            return stops;
        }

        std::vector<StopPtr> MakeRoute(const Bus& bus) {
            std::vector<StopPtr> stops;
            MakeRoute(bus.route_.begin(), bus.route_.end(), stops, bus.flag_);
            return stops;
        }

    }
}