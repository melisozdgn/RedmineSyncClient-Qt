QT       += core gui network widgets
CONFIG   += c++17


INCLUDEPATH += $$PWD/Controller \
               $$PWD/Model \
               $$PWD/View \
               $$PWD/Service \
               $$PWD/Utils

SOURCES += \
    main.cpp \
    Controller/TaskController.cpp \
    Service/RedmineManager.cpp \
    Service/TaskRepository.cpp \
    Utils/Logger.cpp \
    View/ProgressBar.cpp \
    View/ConflictDialog.cpp \
    View/GanttView.cpp \
    View/MainWindow.cpp \
    View/DescriptionDialog.cpp

HEADERS += \
    Controller/TaskController.h \
    Model/Config.h \
    Model/Task.h \
    Service/RedmineManager.h \
    Service/TaskRepository.h \
    Utils/Logger.h \
    View/ProgressBar.h \
    View/ConflictDialog.h \
    View/DescriptionDialog.h \
    View/GanttView.h \
    View/MainWindow.h

FORMS += \
    View/ConflictDialog.ui \
    View/MainWindow.ui \
    View/DescriptionDialog.ui

DISTFILES += \
    .gitignore \
    config.json

INCLUDEPATH += . Controller Model Service View Utils
