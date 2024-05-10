cmake_minimum_required(VERSION 3.17)

project (metrics-app)

include_directories(
    ${CMAKE_SOURCE_DIR}/common
    ${prometheus-cpp_INCLUDE_DIR}
)

find_package(yaml-cpp 0.6.2 REQUIRED)

add_executable(metrics-app main.cpp MetricsCore.cpp HealthCore.cpp CheckinHistory.cpp )

target_link_libraries(metrics-app Threads::Threads yaml-cpp prometheus-cpp::pull)

# Install target
install(TARGETS metrics-app DESTINATION bin)
