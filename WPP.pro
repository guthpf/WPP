
#Copyright © 2014 Gustavo Thebit Pfeiffer / LCG-COPPE-UFRJ

#-------------------------------------------------
# Project created by QtCreator 2014-01-29T12:48:54
#-------------------------------------------------

###########
#
#    This file is part of WebcamPaperPen.
#
#    WebcamPaperPen is free software: you can redistribute it and/or modify it
#    under the terms of the GNU Lesser General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or (at your
#    option) any later version.
#
#    WebcamPaperPen is distributed in the hope that it will be useful, but
#    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
#    or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
#    License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with WebcamPaperPen. If not, see <http://www.gnu.org/licenses/>.
#
###########

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WPP
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
        mouse.cpp \
        imgprocthread.cpp \
    escapi.cpp \
    feedbackviewer.cpp \
    normalize.cpp \
    shadowtrack.cpp \
    rectification.cpp \
    calibrator.cpp \
    pentrack.cpp \
    wppvideocapture.cpp \
    clickdetector.cpp

unix {
LIBS += -lX11 -lopencv_core -lopencv_highgui
}

INCLUDEPATH += /usr/include/eigen3/

HEADERS  += mainwindow.h \
    mouse.h \
    common.h \
    imgprocthread.h \
    escapi.h \
    feedbackviewer.h \
    normalize.h \
    shadowtrack.h \
    rectification.h \
    calibrator.h \
    pentrack.h \
    wppmatrix.h \
    wpppoint.h \
    wppvideocapture.h \
    clickdetector.h

FORMS    += mainwindow.ui

OTHER_FILES += \
    resources.rc

RC_FILE = resources.rc

