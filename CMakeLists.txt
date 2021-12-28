cmake_minimum_required(VERSION 3.21)
project(Client)

set(CMAKE_CXX_STANDARD 11)

#include_directories(${/home/noabeldoch/boost_1_77_0/boost})
#link_directories(${/home/noabeldoch/boost_1_77_0/stage/lib}/x64)

add_executable(Client src/echoClient.cpp src/connectionHandler.cpp include/connectionHandler.h include/echoClient.h src/readFromSocketThread.cpp include/readFromSocketThread.h)

#target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})

set(Boost_USE_STATIC_LIBS   ON)
set(Boost_USE_MULTITHREADED   ON)
set(Boost_USE_STATIC_RUNTIME   OFF)
find_package(Boost REQUIRED COMPONENTS system)
include_directories(include)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")



target_link_libraries(Client Boost::system)
