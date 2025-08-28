cmake_minimum_required(VERSION 3.28)
project(OtbDatTransformata)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++ -static")

include(FetchContent)

# Fetch toml++
FetchContent_Declare(
        tomlplusplus
        GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
        GIT_TAG v3.4.0
)
FetchContent_MakeAvailable(tomlplusplus)

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
        settings.h
        Compilers/DatCompiler.h
        Compilers/DatCompiler.cpp
        Compilers/MetadataWriter.h
        Compilers/MetadataWriter6.h
        Compilers/MetadataFlags6.h
        Compilers/ThingCategory.h
        Compilers/MetadataReader.h
        FrameGroup.h
)

target_include_directories(OtbDatTransformata PRIVATE ${tomlplusplus_SOURCE_DIR}/include)
# target_link_libraries(OtbDatTransformata PRIVATE )