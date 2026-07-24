set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CLIENT_RESOURCES resources.qrc)
if(WIN32)
    list(APPEND CLIENT_RESOURCES app_icon.rc)
endif()

add_executable(SmartHomeMonitorClient WIN32
    main.cpp
    add_device_dialog.cpp
    login_dialog.cpp
    main_window.cpp
    ptz_gear_control.cpp
    protocol_client.cpp
    recording_browser_dialog.cpp
    video_grid_widget.cpp
    video_tile_widget.cpp
    video_decoder.cpp
    add_device_dialog.h
    client_theme.h
    login_dialog.h
    main_window.h
    ptz_gear_control.h
    protocol_client.h
    recording_browser_dialog.h
    video_grid_widget.h
    video_tile_widget.h
    video_decoder.h
    ${CLIENT_RESOURCES}
)
target_link_libraries(SmartHomeMonitorClient PRIVATE
    Qt5::Widgets Qt5::Network Qt5::Multimedia Qt5::MultimediaWidgets
)
if(FFMPEG_FOUND AND FFMPEG_SWSCALE_FOUND)
    target_link_libraries(SmartHomeMonitorClient PRIVATE PkgConfig::FFMPEG PkgConfig::FFMPEG_SWSCALE)
    target_compile_definitions(SmartHomeMonitorClient PRIVATE SMART_CLIENT_HAS_FFMPEG=1)
endif()
target_include_directories(SmartHomeMonitorClient PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
