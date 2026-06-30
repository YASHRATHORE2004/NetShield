cmake_minimum_required(VERSION 3.16)
project(PacketAnalyzer VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Source files
set(SOURCES
    src/main.cpp
    src/pcap_reader.cpp
    src/packet_parser.cpp
)

# Header files location
include_directories(${CMAKE_SOURCE_DIR}/include)

# Find Threads (Required for the HTTP Server)
find_package(Threads REQUIRED)

# Create the executable
add_executable(packet_analyzer ${SOURCES})

# Link pcap and threading
target_link_libraries(packet_analyzer pcap Threads::Threads)