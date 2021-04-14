# MessageHandlerUnitTest
add_executable(MessageHandlerUnitTest
        MessageHandlerUnitTest.cpp
        )

add_test(NAME MessageHandlerUnitTest COMMAND MessageHandlerUnitTest)

target_link_libraries(MessageHandlerUnitTest
        SwarmUS::HiveMind::Bridge
        SwarmUS::Propolis::Pheromones
        SwarmUS::Propolis::Cpp::Common
        gmock_main
        )

# HiveMindBridgeImplUnitTest
add_executable(HiveMindBridgeImplUnitTest
        HiveMindBridgeImplUnitTest.cpp
        )

add_test(NAME HiveMindBridgeImplUnitTest COMMAND HiveMindBridgeImplUnitTest)

target_link_libraries(HiveMindBridgeImplUnitTest
        SwarmUS::HiveMind::Bridge
        SwarmUS::Propolis::Pheromones
        SwarmUS::Propolis::Cpp::Common
        gmock_main
        )

# Integration testing
#add_executable(QueueAndSendIntegrationTestBridgeMain
#        integration/QueueAndSendIntegrationTest/BridgeMain.cpp
#        )

#target_link_libraries(QueueAndSendIntegrationTestBridgeMain
#        ${LIB_NAME}
#        SwarmUS::Propolis::Pheromones
#        SwarmUS::HiveMind::Bridge
#        )

#add_executable(QueueAndSendIntegrationTest
#        integration/QueueAndSendIntegrationTest/QueueAndSendIntegrationTest.cpp
#        )

#target_link_libraries(QueueAndSendIntegrationTest
#        ${LIB_NAME}
#        SwarmUS::Propolis::Pheromones
#        SwarmUS::HiveMind::Bridge
#        )

add_executable(FunctionDescriptionIntegrationTest
        integration/FunctionDescriptionIntegrationTest.cpp
        )

add_test(NAME FunctionDescriptionIntegrationTest COMMAND FunctionDescriptionIntegrationTest)

target_link_libraries(FunctionDescriptionIntegrationTest
        SwarmUS::HiveMind::Bridge
        SwarmUS::Propolis::Pheromones
        SwarmUS::Propolis::Cpp::Common
        gmock_main
        )

add_executable(UserCallbackIntegrationTest
        integration/UserCallbackIntegrationTest.cpp
        )

add_test(NAME UserCallbackIntegrationTest COMMAND UserCallbackIntegrationTest)

target_link_libraries(UserCallbackIntegrationTest
        SwarmUS::HiveMind::Bridge
        SwarmUS::Propolis::Pheromones
        SwarmUS::Propolis::Cpp::Common
        gmock_main
        )