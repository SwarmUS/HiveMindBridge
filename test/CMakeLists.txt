# MessageHandlerUnitTest
add_executable(MessageHandlerUnitTest
        MessageHandlerUnitTest.cpp
        )

add_test(NAME MessageHandlerUnitTest COMMAND MessageHandlerUnitTest)

target_link_libraries(MessageHandlerUnitTest
        ${LIB_NAME}
        SwarmUS::Propolis::Pheromones
        gmock_main
        )

# HiveMindBridgeImplUnitTest
add_executable(HiveMindBridgeImplUnitTest
        HiveMindBridgeImplUnitTest.cpp
        )

add_test(NAME HiveMindBridgeImplUnitTest COMMAND HiveMindBridgeImplUnitTest)

target_link_libraries(HiveMindBridgeImplUnitTest
        ${LIB_NAME}
        SwarmUS::Propolis::Pheromones
        gmock_main
        )

# Integration testing
add_executable(SocketToMoveByIntegrationTest
        integration/SocketToMoveByIntegrationTest.cpp
        )

target_link_libraries(SocketToMoveByIntegrationTest
        ${LIB_NAME}
        SwarmUS::Propolis::Pheromones
        )

add_executable(QueueAndSendIntegrationTestBridgeMain
        integration/QueueAndSendIntegrationTest/BridgeMain.cpp
        )

target_link_libraries(QueueAndSendIntegrationTestBridgeMain
        ${LIB_NAME}
        SwarmUS::Propolis::Pheromones
        SwarmUS::HiveMind::Bridge
        )

add_executable(QueueAndSendIntegrationTest
        integration/QueueAndSendIntegrationTest/QueueAndSendIntegrationTest.cpp
        )

target_link_libraries(QueueAndSendIntegrationTest
        ${LIB_NAME}
        SwarmUS::Propolis::Pheromones
        SwarmUS::HiveMind::Bridge
        )

add_executable(FunctionDescriptionIntegrationTest
        integration/FunctionDescriptionIntegrationTest.cpp
        )

target_link_libraries(FunctionDescriptionIntegrationTest
        ${LIB_NAME}
        SwarmUS::Propolis::Pheromones
        SwarmUS::HiveMind::Bridge
        gmock_main
        )