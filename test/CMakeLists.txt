
#############
## Testing ##
#############

# MessageHandlerUnitTest
add_executable(MessageHandlerUnitTest
        MessageHandlerUnitTest.cpp
        )

add_test(NAME MessageHandlerUnitTest COMMAND MessageHandlerUnitTest)

target_link_libraries(MessageHandlerUnitTest
        ${catkin_LIBRARIES}
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
        ${catkin_LIBRARIES}
        ${LIB_NAME}
        SwarmUS::Propolis::Pheromones
        gmock_main
        )

# Integration testing
add_executable(QueueAndSendIntegrationTestBridgeMain
        integration/QueueAndSendIntegrationTest/BridgeMain.cpp
        )

target_link_libraries(QueueAndSendIntegrationTestBridgeMain
        ${catkin_LIBRARIES}
        ${LIB_NAME}
        SwarmUS::Propolis::Pheromones
        SwarmUS::HiveMind::Bridge
        )

add_executable(QueueAndSendIntegrationTest
        integration/QueueAndSendIntegrationTest/QueueAndSendIntegrationTest.cpp
        )

target_link_libraries(QueueAndSendIntegrationTest
        ${catkin_LIBRARIES}
        ${LIB_NAME}
        SwarmUS::Propolis::Pheromones
        SwarmUS::HiveMind::Bridge
        )