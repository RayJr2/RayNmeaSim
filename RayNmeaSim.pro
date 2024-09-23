QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets serialport xml charts gui location websockets

CONFIG += c++17

# Define application version and some other constants
APP_VERSION = 0.1.0
APP_NAME = RayNmeaSim
APP_COMPANY = RFStateSide
APP_SECRET_KEY = ChangeMeIsAGoodIdea

# Add the constants to the preprocessor defines
DEFINES += APP_VERSION=\\\"$$APP_VERSION\\\"
DEFINES += APP_NAME=\\\"$$APP_NAME\\\"
DEFINES += APP_COMPANY=\\\"$$APP_COMPANY\\\"
DEFINES += APP_SECRET_KEY=\\\"$$APP_SECRET_KEY\\\"

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

TEMPLATE = app

# Platform-specific flags
win32 {
#    DEFINES += ACTISENSE
    INCLUDEPATH += C:/Programming/Qt/RayNmeaSim/src
    INCLUDEPATH += C:/Programming/Qt/NMEA2000/src
}

unix {
#    DEFINES += LINUX_CAN
    INCLUDEPATH += /home/pi/Programming/Qt/RayNmeaSim/src
    INCLUDEPATH += /home/pi/Programming/Qt/NMEA2000/src

}
# INCLUDEPATH += /home/pi/Programming/Qt/NMEA2000/src
INCLUDEPATH += /Programming/Qt/NMEA2000/src

SOURCES += \
../NMEA2000/src/ActisenseReader.cpp \
../NMEA2000/src/N2kDeviceList.cpp \
../NMEA2000/src/N2kGroupFunction.cpp \
../NMEA2000/src/N2kGroupFunctionDefaultHandlers.cpp \
../NMEA2000/src/N2kMaretron.cpp \
../NMEA2000/src/N2kMessages.cpp \
../NMEA2000/src/N2kMsg.cpp \
../NMEA2000/src/N2kStream.cpp \
../NMEA2000/src/N2kTimer.cpp \
../NMEA2000/src/NMEA2000.cpp \
../NMEA2000/src/Seasmart.cpp \
    src/backgrounditem.cpp \
    src/clickablelabel.cpp \
    src/compass.cpp \
    src/convert.cpp \
    src/dataenums.cpp \
    src/dialogsetup.cpp \
    src/helper.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/nmea2000_actisense.cpp \
    src/nmea2000handler.cpp

HEADERS += \
../NMEA2000/src/ActisenseReader.h \
../NMEA2000/src/N2kCANMsg.h \
../NMEA2000/src/N2kDef.h \
../NMEA2000/src/N2kDeviceList.h \
../NMEA2000/src/N2kGroupFunction.h \
../NMEA2000/src/N2kGroupFunctionDefaultHandlers.h \
../NMEA2000/src/N2kMaretron.h \
../NMEA2000/src/N2kMessages.h \
../NMEA2000/src/N2kMessagesEnumToStr.h \
../NMEA2000/src/N2kMsg.h \
../NMEA2000/src/N2kStream.h \
../NMEA2000/src/N2kTimer.h \
../NMEA2000/src/N2kTypes.h \
../NMEA2000/src/NMEA2000.h \
../NMEA2000/src/NMEA2000StdTypes.h \
../NMEA2000/src/NMEA2000_CAN.h \
../NMEA2000/src/NMEA2000_CompilerDefns.h \
../NMEA2000/src/RingBuffer.h \
../NMEA2000/src/RingBuffer.tpp \
../NMEA2000/src/Seasmart.h \
    src/backgrounditem.h \
    src/clickablelabel.h \
    src/compass.h \
    src/convert.h \
    src/dataenums.h \
    src/dialogsetup.h \
    src/helper.h \
    src/mainwindow.h \
    src/nmea2000_actisense.h \
    src/nmea2000handler.h

# Include NMEA2000_SocketCAN only for Unix (Rpi)
unix {
    SOURCES += ../NMEA2000/src/NMEA2000_SocketCAN.cpp
    HEADERS += ../NMEA2000/src/NMEA2000_SocketCAN.h
}

FORMS += \
    ui/dialogsetup.ui \
    ui/mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
