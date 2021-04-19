set(TEST_NAME test-hivemind-bridge)

set(TEST_SOURCES
    HiveMindBridgeImplUnitTest.cpp
    #MessageHandlerUnitTest.cpp
    UserCallRequestManagerUnitTest.cpp
    UserCallbackMapUnitTest.cpp
)

set(TEST_HEADERS
    mocks/HiveMindHostDeserializerInterfaceMock.h
    mocks/HiveMindHostSerializerInterfaceMock.h
    mocks/MessageHandlerInterfaceMock.h
    mocks/TCPServerInterfaceMock.h
    mocks/ThreadSafeQueueInterfaceMock.h
    mocks/UserCallRequestManagerInterfaceMock.h
    mocks/UserCallbackMapInterfaceMock.h
)

add_executable(${TEST_NAME} ${TEST_SOURCES} ${TEST_HEADERS})

target_link_libraries(${TEST_NAME}
    PUBLIC
        gtest
        gtest_main
        gmock

        SwarmUS::HiveMind::Bridge
)

add_test(NAME ${TEST_NAME}
        COMMAND ${TEST_NAME})

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

add_executable(SendRequestIntegrationTest
        integration/SendRequestIntegrationTest.cpp
        )

add_test(NAME SendRequestIntegrationTest COMMAND SendRequestIntegrationTest)

target_link_libraries(SendRequestIntegrationTest
        SwarmUS::HiveMind::Bridge
        SwarmUS::Propolis::Pheromones
        SwarmUS::Propolis::Cpp::Common
        gmock_main
        )