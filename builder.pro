LIBS += -lmaxmodel -laul
DEFINES += "USE_BFD"

HEADERS += \
    mainwindow.h \
    graphicsview.h \
    main.h \
    block.h \
    blockinstance.h \
    script.h \
    module.h \
    link.h \
    entry.h

SOURCES += \
    mainwindow.cpp \
    graphicsview.cpp \
    main.cpp \
    blockinstance.cpp

RESOURCES += \
    application.qrc
