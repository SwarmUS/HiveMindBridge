find_package(Threads REQUIRED)

set(EXAMPLE_SOURCES
        main.cpp)

set(EXAMPLE_HEADERS
        BasicLogger.h)

add_executable(hello_world_example ${EXAMPLE_SOURCES} ${EXAMPLE_HEADERS})

target_link_libraries(hello_world_example
        PUBLIC
        SwarmUS::HiveMind::Bridge
        SwarmUS::Propolis::Pheromones
        SwarmUS::Propolis::Cpp::Common
        Threads::Threads
        )