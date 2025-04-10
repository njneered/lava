cmake_minimum_required(VERSION 3.28)
project(the-floor-is-lava)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)
link_directories(lib)

add_executable(the-floor-is-lava
        main.cpp
        algorithms/AStar.cpp
        algorithms/AStar.h
        Map.h
        map.cpp
        include/tinyxml2-master/tinyxml2.cpp
)

target_link_libraries(the-floor-is-lava
        sfml-graphics
        sfml-window
        sfml-system
        sfml-main)

add_custom_command(TARGET the-floor-is-lava POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/bin/sfml-graphics-2.dll"
        "${CMAKE_SOURCE_DIR}/bin/sfml-window-2.dll"
        "${CMAKE_SOURCE_DIR}/bin/sfml-system-2.dll"
        $<TARGET_FILE_DIR:the-floor-is-lava>
)