cmake_minimum_required(VERSION 3.17)
project(telem_filter)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/DownloadJson.cmake")
find_package(MathGL2 REQUIRED FLTK)
find_package(FLTK)

add_executable(telem_filter
        src/main.cpp
        src/telem_data.cpp src/telem_data.h
        src/mgl_plotter.cpp src/mgl_plotter.h
        src/telem_data_json.cpp src/telem_data_json.h
        src/csv_writer.cpp src/csv_writer.h
        src/csv_reader.cpp src/csv_reader.h
        src/digital_filter.cpp src/digital_filter.h
        src/stage_1_plotter.cpp src/stage_1_plotter.h
        src/stage_2_plotter.cpp src/stage_2_plotter.h
        src/stage_3_plotter.cpp src/stage_3_plotter.h
        src/c11_binary_latch.cpp src/c11_binary_latch.h
        src/vector2d.cpp src/vector2d.h
        src/staged_mgl_plotter.h
        src/staged_telem_plotter.h)
target_link_libraries(telem_filter
        PRIVATE json
        PRIVATE ${MATHGL2_LIBRARIES}
        PRIVATE ${MATHGL2_FLTK_LIBRARIES}
        PRIVATE ${FLTK_LIBRARIES}
        PRIVATE pthread)
target_include_directories(telem_filter
        PRIVATE ${MATHGL2_INCLUDE_DIRS}
        PRIVATE ${MATHGL2_FLTK_INCLUDE_DIRS})
