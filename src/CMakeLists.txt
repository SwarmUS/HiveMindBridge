# Install destinations
set(main_lib_dest "lib/swarmus-hivemind-bridge")
set(include_dest "include/HiveMindBridge")

set(LIB_NAME "swarmus-hivemind-bridge")
set(LIB_ALIAS "SwarmUS::HiveMind::Bridge")

set(header_path "${PROJECT_SOURCE_DIR}/include/HiveMindBridge")
set(header
        ${header_path}/HiveMindBridge.h
        ${header_path}/Callback.h
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
        )

set(HIVEMIND_BRIDGE_SOURCES
        TCPServer.cpp
        MessageHandler.cpp
        MessageUtils.cpp
        HiveMindBridge.cpp
        HiveMindBridgeImpl.cpp
        UserCallbackFunctionWrapper.cpp
        UserCallbackArgumentDescription.cpp
        InboundRequestHandle.cpp
        InboundResponseHandle.cpp
        OutboundRequestHandle.cpp)

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