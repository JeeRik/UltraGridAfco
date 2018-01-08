cmake_minimum_required(VERSION 3.6)
project(UltraGridAfco)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        src/BlockWarnBuffer.cpp
        src/BlockWarnBuffer.h
        src/ComHandler.cpp
        src/ComHandler.h
        src/Constants.h
        src/Main.cpp
        src/Main.h
        src/RingBuffer.cpp
        src/RingBuffer.h
        src/sandbox.cpp
        src/SocketHandler.cpp
        src/SocketHandler.h
        src/Stats.cpp
        src/Stats.h
        src/StatsAnalyzer.cpp
        src/StatsAnalyzer.h
        src/StatsBuffer.cpp
        src/StatsBuffer.h
        src/StatsCheck.cpp
        src/StatsCheck.h
        src/StatsParser.cpp
        src/StatsParser.h
        src/StringSocket.cpp
        src/StringSocket.h
        src/Tools.cpp
        src/Tools.h
        src/UgAdapter.cpp
        src/UgAdapter.h)

add_executable(UltraGridAfco ${SOURCE_FILES})