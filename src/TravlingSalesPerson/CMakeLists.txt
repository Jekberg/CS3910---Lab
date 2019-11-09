add_executable(
    "RNG-TSP"
    "RNG-Main.cpp")

target_include_directories(
    "RNG-TSP"
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CS3910_INCLUDE_DIR})

add_executable(
    "Hill-TSP"
    "Hill-Main.cpp")

target_include_directories(
    "Hill-TSP"
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CS3910_INCLUDE_DIR})

add_executable(
    "ACO-TSP"
    "ACO-Main.cpp")

target_include_directories(
    "ACO-TSP"
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CS3910_INCLUDE_DIR})

target_link_libraries(
    "ACO-TSP"
    PRIVATE
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:tbb>)

add_executable(
    "EA-TSP"
    "EA-Main.cpp")

target_include_directories(
    "EA-TSP"
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CS3910_INCLUDE_DIR})
