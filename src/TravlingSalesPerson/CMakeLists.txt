add_executable(
    HillClimbTSP
    "HillClimbMain.cpp")

target_link_directories(
    HillClimbTSP
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR})