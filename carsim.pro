QT += core gui widgets

CONFIG += c++17

TARGET = TrafficSim
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    simulationarea.cpp \
    trafficlight.cpp \
    vehicle.cpp \
    car.cpp \
    road.cpp

HEADERS += \
    mainwindow.h \
    simulationarea.h \
    trafficlight.h \
    vehicle.h \
    car.h \
    road.h

FORMS += \
    mainwindow.ui

RESOURCES += \
    resources.qrc
