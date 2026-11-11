cmake_minimum_required(VERSION 4.0)
project(NASA)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG        master
        GIT_SHALLOW    ON
)
FetchContent_MakeAvailable(raylib)

add_executable(NASA
        main.cpp
        Init.cpp
        GameCamera.cpp
        GameManager.cpp
        GameManager.h
        IGame.h
        MainMenu.cpp
        MainMenu.h
        Game1.cpp
        Game1.h
        Game2.cpp
        Game2.h
        Game3.cpp
        Game3.h
        Game4.cpp
        Game4.h
)

target_link_libraries(NASA PRIVATE raylib)