QT += core gui widgets network multimedia multimediawidgets
CONFIG += c++11
TEMPLATE = app
TARGET = SmartHomeMonitorClient

SOURCES += main.cpp \
           add_device_dialog.cpp \
           login_dialog.cpp \
           main_window.cpp \
           ptz_gear_control.cpp \
           protocol_client.cpp \
           recording_browser_dialog.cpp \
           video_grid_widget.cpp \
           video_tile_widget.cpp \
           video_decoder.cpp
HEADERS += add_device_dialog.h \
           client_theme.h \
           login_dialog.h \
           main_window.h \
           ptz_gear_control.h \
           protocol_client.h \
           recording_browser_dialog.h \
           video_grid_widget.h \
           video_tile_widget.h \
           video_decoder.h
RESOURCES += resources.qrc

win32 {
    RC_ICONS = app_icon.ico

    # A command-line assignment still takes priority.  Otherwise use an
    # environment variable, the SDK bundled with a version package, or the
    # workspace SDK link.  This keeps Qt Creator's extra arguments optional.
    isEmpty(FFMPEG_ROOT): FFMPEG_ROOT = $$(FFMPEG_ROOT)
    isEmpty(FFMPEG_ROOT) {
        bundledSdk = $$clean_path($$PWD/ffmpeg-sdk)
        exists($$bundledSdk/include/libavcodec/avcodec.h): FFMPEG_ROOT = $$bundledSdk
    }
    isEmpty(FFMPEG_ROOT) {
        workspaceSdk = $$clean_path($$PWD/../../ffmpeg_sdk_win64)
        exists($$workspaceSdk/include/libavcodec/avcodec.h): FFMPEG_ROOT = $$workspaceSdk
    }

    !isEmpty(FFMPEG_ROOT):exists($$FFMPEG_ROOT/include/libavcodec/avcodec.h) {
        DEFINES += SMART_CLIENT_HAS_FFMPEG=1
        INCLUDEPATH += $$FFMPEG_ROOT/include
        LIBS += -L$$FFMPEG_ROOT/lib -lavcodec -lavutil -lswscale
    } else {
        warning("FFMPEG_ROOT is unset; realtime decode is disabled")
    }
}
unix {
    packagesExist(libavcodec libavutil libswscale) {
        DEFINES += SMART_CLIENT_HAS_FFMPEG=1
        CONFIG += link_pkgconfig
        PKGCONFIG += libavcodec libavutil libswscale
    } else {
        warning("FFmpeg development libraries were not found; realtime decode is disabled")
    }
}
