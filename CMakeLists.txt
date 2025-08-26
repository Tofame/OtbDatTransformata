cmake_minimum_required(VERSION 3.28)
project(OtbDatTransformata)

set(CMAKE_CXX_STANDARD 20)

add_executable(OtbDatTransformata main.cpp
        Loaders/DatLoader.cpp
        Loaders/DatLoader.h
        ItemType.h
        Items.cpp
        Items.h
        Loaders/OtbLoader.cpp
        Loaders/OtbLoader.h
        Loaders/fileLoader.h
        Loaders/fileLoader.cpp
        Loaders/itemLoader_enums.h
        tools.h
        Loaders/ClientIdToServerIdMap.h
        Loaders/DatAttributes.h
        globals.h
)