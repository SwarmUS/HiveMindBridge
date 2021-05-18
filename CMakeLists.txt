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

# Install destinations
set(main_lib_dest "lib/swarmus-hivemind-bridge")
set(include_dest "include/hivemind-bridge")

set(LIB_NAME "swarmus-hivemind-bridge")
set(LIB_ALIAS "SwarmUS::HiveMind::Bridge")

set(header_path "${PROJECT_SOURCE_DIR}/include/hivemind-bridge")
set(header
        ${header_path}/HiveMindBridge.h
        ${header_path}/HiveMindBridgeImpl.h
        ${header_path}/IHiveMindBridge.h
        ${header_path}/IMessageHandler.h
        ${header_path}/InboundRequestHandle.h
        ${header_path}/InboundResponseHandle.h
        ${header_path}/ITCPServer.h
        ${header_path}/IThreadSafeQueue.h
        ${header_path}/MessageHandler.h
        ${header_path}/MessageUtils.h
        ${header_path}/OutboundRequestHandle.h
        ${header_path}/TCPServer.h
        ${header_path}/ThreadSafeQueue.h
        ${header_path}/UserCallbackArgumentDescription.h
        ${header_path}/UserCallbackFunctionWrapper.h
        ${header_path}/Callback.h
        ${header_path}/IUserCallbackMap.h
        ${header_path}/IUserCallRequestHandler.h
        ${header_path}/UserCallbackMap.h
        ${header_path}/UserCallRequestHandler.h
        ${header_path}/HiveMindHostApiRequestHandler.h
        ${header_path}/IHiveMindHostApiRequestHandler.h
        ${header_path}/BytesAccumulator.h
        ${header_path}/IBytesAccumulator.h
        )

set(HIVEMIND_BRIDGE_SOURCES
        src/TCPServer.cpp
        src/MessageHandler.cpp
        src/MessageUtils.cpp
        src/HiveMindBridge.cpp
        src/HiveMindBridgeImpl.cpp
        src/UserCallbackFunctionWrapper.cpp
        src/UserCallbackArgumentDescription.cpp
        src/InboundRequestHandle.cpp
        src/InboundResponseHandle.cpp
        src/OutboundRequestHandle.cpp
        src/user-call/UserCallRequestHandler.cpp
        src/user-call/UserCallbackMap.cpp
        src/HiveMindHostApiRequestHandler.cpp
        src/BytesAccumulator.cpp
        )

add_library(${LIB_NAME} ${HIVEMIND_BRIDGE_SOURCES})
add_library(${LIB_ALIAS} ALIAS ${LIB_NAME})

target_link_libraries(${LIB_NAME}
        PRIVATE
        SwarmUS::Propolis::Pheromones
        SwarmUS::Propolis::Cpp::Common
        )

target_include_directories(${LIB_NAME}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>

        $<INSTALL_INTERFACE:${main_lib_dest}>
        )


### INSTALLATION ###
set(lib_dest ${main_lib_dest})

install(TARGETS ${LIB_NAME} EXPORT ${LIB_NAME} DESTINATION "${lib_dest}")

install(FILES ${header} DESTINATION ${include_dest})
install(FILES swarmus-hivemind-bridge-config.cmake DESTINATION "${lib_dest}")
install(EXPORT ${LIB_NAME} DESTINATION "${lib_dest}")

### UNINSTALLATION ###
add_custom_target(uninstall COMMAND xargs rm < install_manifest.txt)

### TESTING ###
include(cmake/googletest/common.cmake)
enable_testing()
googletest_fetch_populate()
add_subdirectory(test)