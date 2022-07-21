cmake_minimum_required(VERSION 3.20)
set(PROJECT fxr_reloader_agent)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_CXX_FLAGS "-O3 -s")
set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG=2")


set(SOURCES
        src/entry.cpp src/entry.h
        src/logging.cpp src/logging.h
        src/packets.cpp src/packets.h
        src/fxr.cpp src/fxr.h
        src/pipe_server.cpp src/pipe_server.h
)

include_directories(src)

project(${PROJECT})

add_library(${PROJECT} SHARED ${SOURCES})
target_link_libraries(${PROJECT} wsock32 ws2_32)
target_link_options(${PROJECT} PRIVATE -static -Wno-multichar -Wl,--image-base,0x190000)