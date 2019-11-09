add_executable(
    "PSO-AAP"
    "PSO-Main.cpp"
    "AntennaArray.cpp")

target_include_directories(
    "PSO-AAP"
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CS3910_INCLUDE_DIR})

target_link_libraries(
    "PSO-AAP"
    PRIVATE
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:tbb>)
