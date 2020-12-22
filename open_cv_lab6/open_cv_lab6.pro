TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += opencv4
}

SOURCES += \
        main.cpp
