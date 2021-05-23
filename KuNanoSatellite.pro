QT -= gui
QT += serialport

CONFIG += c++11 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        cpp/cameracontroller.cpp \
        cpp/gpscontroller.cpp \
        main.cpp \
        cpp/rtccontroller.cpp \
        cpp/satcontroller.cpp \
        cpp/transreceiver.cpp

HEADERS += \
    cpp/cameracontroller.h \
    cpp/gpscontroller.h \
    cpp/logger.h \
    cpp/rtccontroller.h \
    cpp/satcontroller.h \
    cpp/transreceiver.h

INCLUDEPATH += /usr/local/include/opencv4/
LIBS += -L/usr/local/lib64/ -lopencv_objdetect -lopencv_core -lopencv_imgproc -lopencv_imgcodecs -lopencv_highgui -lopencv_videoio -lopencv_dnn  # -ldlib

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


