//Copyright © 2014 Gustavo Thebit Pfeiffer / LCG-COPPE-UFRJ
/*
    This file is part of WebcamPaperPen.

    WebcamPaperPen is free software: you can redistribute it and/or modify it
    under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or (at your
    option) any later version.

    WebcamPaperPen is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
    or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
    License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with WebcamPaperPen. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef FMAIN_H
#define FMAIN_H

#include <QThread>
#include <QImage>

#include "feedbackviewer.h"

class MainWindow;

class Mouse;

class ImgProcThread : public QThread
{
    Q_OBJECT
private:
    MainWindow * parent_;

    //MainWindow will set these variables on as input to ImgProcThread via the methods below
    bool quit_;
    bool command_recalibrate_;
    bool command_change_camera_;
    bool accepted_;
    bool decided_;

    FeedbackViewer * fbv_;

    Mouse * mouse_;

signals:

    //These signals are sent to MainWindow in order to change the user interface.
    void enableRecalibrate();
    void enableAcceptDeny();
    void startedCalibrating();
    void startedCalibrated();
    void message(QString);

public:

    ImgProcThread(MainWindow * parent, FeedbackViewer * fbv, Mouse * mouse);

    //MainWindow may call these methods as input to ImgProcThread
    void askToQuit();
    void askToRecalibrate();
    void askToChangeCamera();
    void makeEnableRecalibrate();
    void decideCalibration(bool accept);

    //"Main" function of this thread. Called by run()
    int fmain();

    void run();

    //MainWindow may set this on/off. Result will only affect the next frame
    bool lefthanded;
};

#include "mouse.h"
#include "mainwindow.h"

#endif // FMAIN_H
