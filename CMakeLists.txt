cmake_minimum_required(VERSION 3.28)
project(the-floor-is-lava)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)
include_directories(algorithms)
include_directories(UI-UX)
link_directories(lib)

add_executable(the-floor-is-lava
        main.cpp
        UI-UX/Map.h
        UI-UX/map.cpp
        include/tinyxml2-master/tinyxml2.cpp
        UI-UX/NaviGator.h
        UI-UX/navigator.cpp
        UI-UX/Title.h
        UI-UX/title.cpp
        UI-UX/Background.h
        UI-UX/background.cpp
        UI-UX/Soundtrack.h
        UI-UX/soundtrack.cpp
        UI-UX/UX.h
        UI-UX/ux.cpp
        UI-UX/GPS.h
        UI-UX/gps.cpp
        algorithms/Algorithms.h
        algorithms/algorithms.cpp
)

target_link_libraries(the-floor-is-lava
        sfml-graphics
        sfml-window
        sfml-system
        sfml-main
        sfml-audio)

add_custom_command(TARGET the-floor-is-lava POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/bin/sfml-graphics-2.dll"
        "${CMAKE_SOURCE_DIR}/bin/sfml-window-2.dll"
        "${CMAKE_SOURCE_DIR}/bin/sfml-system-2.dll"
        "${CMAKE_SOURCE_DIR}/bin/sfml-audio-2.dll"
        "${CMAKE_SOURCE_DIR}/bin/openal32.dll"
        $<TARGET_FILE_DIR:the-floor-is-lava>
)