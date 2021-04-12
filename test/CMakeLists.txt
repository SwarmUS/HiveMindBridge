
#############
## Testing ##
#############

# MessageHandlerUnitTest
add_executable(MessageHandlerUnitTest
        test/MessageHandlerUnitTest.cpp
        )

add_test(NAME MessageHandlerUnitTest COMMAND MessageHandlerUnitTest)

target_link_libraries(MessageHandlerUnitTest
        ${catkin_LIBRARIES}
        ${LIB_NAME}
        SwarmUS::Propolis::Pheromones::HiveMind::Host
        gmock_main
        )

# HiveMindBridgeImplUnitTest
add_executable(HiveMindBridgeImplUnitTest
        test/HiveMindBridgeImplUnitTest.cpp
        )

add_test(NAME HiveMindBridgeImplUnitTest COMMAND HiveMindBridgeImplUnitTest)

target_link_libraries(HiveMindBridgeImplUnitTest
        ${catkin_LIBRARIES}
        ${LIB_NAME}
        SwarmUS::Propolis::Pheromones::HiveMind::Host
        gmock_main
        )

# Integration testing
add_executable(QueueAndSendIntegrationTestBridgeMain
        test/integration/QueueAndSendIntegrationTest/BridgeMain.cpp
        )

target_link_libraries(QueueAndSendIntegrationTestBridgeMain
        ${catkin_LIBRARIES}
        ${LIB_NAME}
        SwarmUS::Propolis::Pheromones::HiveMind::Host
        SwarmUS::HiveMind::Bridge
        )

add_executable(QueueAndSendIntegrationTest
        test/integration/QueueAndSendIntegrationTest/QueueAndSendIntegrationTest.cpp
        )

target_link_libraries(QueueAndSendIntegrationTest
        ${catkin_LIBRARIES}
        ${LIB_NAME}
        SwarmUS::Propolis::Pheromones::HiveMind::Host
        SwarmUS::HiveMind::Bridge
        )