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
        HubOutline.cpp
        HubOutline.h
        LightConfig.cpp
        LightConfig.h
        ScaneLight.cpp
        ScaneLight.h
        GreenGarden.cpp
        GreenGarden.h
        AudioManager.cpp
        AudioManager.h
        RadiationBase.cpp
        RadiationBase.h
        PauseMenu.cpp
        PauseMenu.h
        Mission.cpp
        Mission.h
        MissionPanel.cpp
        MissionPanel.h
        HubMission.cpp
        HubMission.h
        FontManager.cpp
        FontManager.h
        UseInterFont.h
        GuideSptire.cpp
        GuideSptire.h
        MainMenuConfig.h
        GardenArt.h
        GardenArt.cpp
        RadiationArt.cpp
        RadiationArt.h
        EnergyArt.cpp
        EnergyArt.h
        PipeDesign.cpp
        PipeDesign.h
        PauseArt.cpp
        PauseArt.h
        DialogueArt.cpp
        MissionArt.cpp
        MissionArt.h
)

add_custom_target(NASA_assets ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:NASA>/assets"
        VERBATIM
)

if (MSVC)
    target_compile_options(NASA PRIVATE
            "/FI${CMAKE_CURRENT_SOURCE_DIR}/UseInterFont.h"
    )
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(NASA PRIVATE
            -include "${CMAKE_CURRENT_SOURCE_DIR}/UseInterFont.h"
    )
endif()

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_dependencies(NASA NASA_assets)

target_link_libraries(NASA PRIVATE raylib)