//Copyright © 2014, 2015 Gustavo Thebit Pfeiffer / LCG-COPPE-UFRJ
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#if QT_VERSION >= 0x050000
#include <QtWidgets/QMainWindow>
//#include <QtWidgets/QLabel>
#else
#include <QMainWindow>
//#include <QLabel>
#endif

namespace Ui {
class MainWindow;
}

class ImgProcThread;

#include "mouse.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    void closeEvent(QCloseEvent * e);
    ~MainWindow();

public slots:

    //Input from UI
    void changeCameraClicked();
    void recalibrateClicked();
    void acceptClicked();
    void denyClicked();
    void radioFixedWindow(bool);
    void radioTouchpadLike(bool);
    void radioFullscreen(bool);
    void changeLeftHanded(bool);
    void changeMouseFrameSize();

    //Input from ImgProcThread
    void enableRecalibrate();
    void enableAcceptDeny();
    void startedCalibrating();
    void startedCalibrated();
    void message(QString);

private slots:
    void on_actionAbout_triggered();

private:
    Ui::MainWindow *ui;
    Mouse mouse_;
    ImgProcThread * thread_;

    void stopThread();
    void loadCfg();
    void saveCfg();
};

#include "imgprocthread.h"

#endif // MAINWINDOW_H

