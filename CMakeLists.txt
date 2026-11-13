cmake_minimum_required(VERSION 3.31)
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

target_compile_definitions(raylib PRIVATE SUPPORT_FILEFORMAT_JPG=1)

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
        Setting.cpp
        Setting.h
        IState.h
        Objects3D.cpp
        Objects3D.h
        UI2DAll.cpp
        UI2DAll.h
        InputManager.cpp
        InputManager.h
        ProfilerTIME.cpp
        ProfilerTIME.h
        HubPortal.cpp
        HubPortal.h
        Hub.cpp
        Hub.h
        GameAssets.cpp
        GameAssets.h
        HubConfig.h
)

add_custom_target(NASA_assets ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:NASA>/assets"
        VERBATIM
)

add_dependencies(NASA NASA_assets)

target_link_libraries(NASA PRIVATE raylib)