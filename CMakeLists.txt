cmake_minimum_required(VERSION 3.0.2)
project(hive_mind_bridge)
set (CMAKE_CXX_STANDARD 17)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  std_msgs
)

catkin_package(
  INCLUDE_DIRS include
)

include(cmake/propolis.cmake)
propolis_fetch_populate()

###########
## Build ##
###########

include_directories(
        include
        ${catkin_INCLUDE_DIRS}
)

# SwarmUS::ROS::HiveMind::Bridge
set(LIB_NAME "swarmus-ros-hivemind-bridge")
set(LIB_ALIAS "SwarmUS::ROS::HiveMind::Bridge")

set(HIVEMIND_BRIDGE_SOURCES
        src/TCPServer.cpp
        src/MessageHandler.cpp
        src/MessageUtils.cpp
        src/HiveMindBridge.cpp
        src/HiveMindBridgeImpl.cpp)

add_library(${LIB_NAME} STATIC ${HIVEMIND_BRIDGE_SOURCES})
add_library(${LIB_ALIAS} ALIAS ${LIB_NAME})

target_link_libraries(${LIB_NAME}
        ${catkin_LIBRARIES}
        SwarmUS::Propolis::Pheromones::HiveMind::Host
        )

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
add_executable(SocketToMoveByIntegrationTest
        test/SocketToMoveByIntegrationTest.cpp
        )

target_link_libraries(SocketToMoveByIntegrationTest
        ${catkin_LIBRARIES}
        ${LIB_NAME}
        SwarmUS::Propolis::Pheromones::HiveMind::Host
)