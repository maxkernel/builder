LIBS += -lmaxmodel -laul
DEFINES += "USE_BFD"

HEADERS += \
    mainwindow.h \
    graphicsview.h \
    main.h \
    block.h \
    blockinstance.h \
    ioentry.h \
    script.h \
    module.h

SOURCES += \
    mainwindow.cpp \
    graphicsview.cpp \
    main.cpp \
    blockinstance.cpp \
    ioentry.cpp

RESOURCES += \
    application.qrc
