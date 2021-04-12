include(CMakeFindDependencyMacro)

find_dependency(swarmus-propolis-pheromones)
find_dependency(swarmus-propolis-cpp-common)

get_filename_component(SELF_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
include(${SELF_DIR}/swarmus-hivemind-bridge.cmake)
