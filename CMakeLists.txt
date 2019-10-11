cmake_minimum_required(VERSION 3.7)
project("ld45")

set(CMAKE_CXX_STANDARD 17)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/tako/cmake)

add_subdirectory("dependencies/tako")
include(tako)
SET(EXECUTABLE ld45)
add_executable(${EXECUTABLE}
        "src/Main.cpp"
        "src/Entity.hpp"
        "src/Physics.hpp"
        "src/Player.hpp"
        "src/Rect.hpp"
        "src/Level.hpp"
        "src/Powerup.hpp"
)
configure_file("src/index.html" "./index.html")

tako_setup(${EXECUTABLE})
target_link_libraries(${EXECUTABLE} PRIVATE tako)

tako_assets_dir("${CMAKE_CURRENT_SOURCE_DIR}/Assets/@")
