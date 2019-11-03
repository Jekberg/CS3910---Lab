add_executable(
    HillClimbTSP
    "HillClimbMain.cpp")

target_include_directories(
    HillClimbTSP
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CS3910_INCLUDE_DIR})

add_executable(
    ACOTSP
    "ACOMain.cpp")

target_include_directories(
    ACOTSP
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CS3910_INCLUDE_DIR})

target_link_libraries(ACOTSP
    PRIVATE
    tbb)

add_executable(
    EvoTSP
    "EvoMain.cpp")

target_include_directories(
    EvoTSP
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CS3910_INCLUDE_DIR})