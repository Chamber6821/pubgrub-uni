project(pubgrub CXX)

#file(GLOB_RECURSE SOURCES pubgrub/*.cpp)
file(GENERATE OUTPUT null.cpp CONTENT "")
set(SOURCES null.cpp)

add_library(${PROJECT_NAME} STATIC ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC .)

add_subdirectory(../../neo-fun neo-fun)
target_link_libraries(${PROJECT_NAME} PUBLIC neo_fun)