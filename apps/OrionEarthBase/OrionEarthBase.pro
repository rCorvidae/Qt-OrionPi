#-------------------------------------------------
#
# Project created by QtCreator 2017-09-24T13:43:08
#
#-------------------------------------------------

QT       += core gui network gamepad

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QMAKE_CFLAGS_ISYSTEM = -I
TARGET = OrionEarthBase
DESTDIR = $$(ORION_BUILDPATH)
TEMPLATE = app
CONFIG += c++11


# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    DriveFeedbackWidget.cpp \
    ../protos/earthBaseToRoverComm.pb.cc \
    ../protos/roverToEarthBaseComm.pb.cc \
    ../protos/protocolEnums.pb.cc \
    NetworkSettings.cpp

HEADERS  += mainwindow.h \
    DriveFeedbackWidget.h \
    ../protos/earthBaseToRoverComm.pb.h \
    ../protos/roverToEarthBaseComm.pb.h \
    ../protos/protocolEnums.pb.h \
    NetworkSettings.h

FORMS    += mainwindow.ui \
    DriveFeedbackWidget.ui \
    NetworkSettings.ui

INCLUDEPATH += \
    ../protos/

unix:!macx: LIBS += -L$$(ORION_BUILDPATH_LIBS) -lnetwork
INCLUDEPATH += $$PWD/../../network
DEPENDPATH += $$PWD/../../network


IS_PROTOBUF_STATIC = $$(PROTOBUF_STATIC)
equals( IS_PROTOBUF_STATIC , true ) {
    message("STATIC protobuf libs")
    LIBS += $$(PROTOBUF)/lib/libprotobuf.a
} else {
    message("Dynamically loaded protobuf libs")
    LIBS += -L$$(PROTOBUF)/lib -lprotobuf
}

INCLUDEPATH += $$(PROTOBUF)/include
DEPENDPATH += $$(PROTOBUF)/include
 
 

DISTFILES += \
    ../protos/earthBaseToRoverComm.proto \
    ../protos/roverToEarthBaseComm.proto \
    ../protos/protocolEnums.proto
