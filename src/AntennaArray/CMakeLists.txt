add_executable(
    AntennaArray
    "Main.cpp"
    "AntennaArray.cpp")

target_include_directories(
    AntennaArray
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CS3910_INCLUDE_DIR})

target_link_libraries(
    AntennaArray
    PRIVATE
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:tbb>)
