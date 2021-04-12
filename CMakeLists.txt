cmake_minimum_required(VERSION 3.13)
project(HiveMindBridge)
set (CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Include external dependencies
include(cmake/propolis/propolis.cmake)
propolis_fetch_populate()

include_directories(
    include
)

include(clang-tools/clang-tidy)
include(clang-tools/clang-format)
include(doc/doxygen)

add_subdirectory(src)

### UNINSTALLATION ###
add_custom_target(uninstall COMMAND xargs rm < install_manifest.txt)

### TESTING ###
include(cmake/googletest/common.cmake)
enable_testing()
googletest_fetch_populate()
add_subdirectory(test)