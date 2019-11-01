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