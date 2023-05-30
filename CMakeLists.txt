cmake_minimum_required(VERSION 3.25)
project(PackageResolver CXX)

set(CMAKE_CXX_STANDARD 20)
add_compile_options(-Wno-unknown-attributes)

include(FetchContent)

FetchContent_Declare(
        Reactions
        GIT_REPOSITORY https://github.com/Chamber6821/Reactions.git
        GIT_TAG 0.0.2
)

FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG 3.2.0
)

add_subdirectory(src)

add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PUBLIC pubgrub)

FetchContent_MakeAvailable(Reactions)
target_link_libraries(${PROJECT_NAME} PUBLIC Reactions::Reactions)

FetchContent_MakeAvailable(nlohmann_json)
target_link_libraries(${PROJECT_NAME} PUBLIC nlohmann_json::nlohmann_json)
