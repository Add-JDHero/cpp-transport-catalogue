cmake_minimum_required(VERSION 3.0.0)
project(cpp-transport-catalogue VERSION 0.1.0)

include(CTest)
enable_testing()

add_executable(cpp-transport-catalogue main.cpp 
    json.cpp json.h 
    json_reader.cpp json_reader.h 
    transport_catalogue.cpp transport_catalogue.h 
    geo.h geo.cpp 
    domain.cpp domain.h 
    request_handler.cpp request_handler.h 
    svg.cpp svg.h
    map_renderer.cpp map_renderer.h 
)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)