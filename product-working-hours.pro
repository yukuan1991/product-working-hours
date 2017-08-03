#-------------------------------------------------
#
# Project created by QtCreator 2017-08-01T16:34:17
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG -= c++11
QMAKE_CXXFLAGS += -std=c++1z

TARGET = product-working-hours
TEMPLATE = app

DEFINES += PRODUCT_PATH=\\\"./product\\\"

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cc\
    Ribbon.cc \
    PwhMain.cc \
    PwhRibbon.cc \
    Pwh/PwhAnalysis.cc \
    Pwh/PwhContrast.cc \
    Pwh/PwhManagement.cc \
    Model/product_structure_model.cpp \
    Qt-Utils/encryption.cpp \
    Qt-Utils/tinyxml2.cpp \
    Qt-Utils/des.cpp \
    Model/product_info_model.cpp \
    Model/json_model.cpp \
    Pwh/production_management_button_tree_dlg.cpp \
    data_veryfication.cpp

HEADERS  += \
    Ribbon.h \
    PwhMain.h \
    PwhRibbon.h \
    Pwh/PwhAnalysis.h \
    Pwh/PwhContrast.h \
    Pwh/PwhManagement.h \
    Model/product_structure_model.h \
    arithmetic_resource.hpp \
    Qt-Utils/encryption.h \
    Qt-Utils/stl_extension.hpp \
    Qt-Utils/json.hpp \
    Qt-Utils/tinyxml2.h \
    Qt-Utils/qt.hpp \
    Qt-Utils/des.h \
    Model/product_info_model.h \
    Model/json_model.h \
    Pwh/production_management_button_tree_dlg.h \
    data_veryfication.h \
    Qt-Utils/krys_application.hpp

FORMS    += pwhmain.ui \
    Pwh/pwhanalysis.ui \
    Pwh/pwhcontrast.ui \
    Pwh/pwhmanagement.ui \
    Pwh/production_management_button_tree_dlg.ui

LIBS += -lboost_filesystem
LIBS += -lboost_system

DISTFILES +=
