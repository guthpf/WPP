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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, "About WebcamPaperPen", QString::fromUtf8("WebcamPaperPen (beta)\nCopyright © 2014 Gustavo Thebit Pfeiffer / LCG-COPPE-UFRJ.\nAll rights reserved. Provided as is with no warranty."));
}

#include <QDir>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mouse_(this)
{

    ui->setupUi(this);
    if(mouse_.error())
    {
        QMessageBox::critical(this, "WebcamPaperPen - ERROR", "FATAL ERROR: No mouse device was found.");
        this->close();
        return;
    }

    setFixedSize(this->size());

    ui->button_accept->setVisible(false);
    ui->button_deny->setVisible(false);
    ui->button_recalibrate->setVisible(false);
    ui->label_accept_calibration->setVisible(false);
    ui->label_calibrating->setVisible(false);

    connect(ui->button_change_camera, SIGNAL(clicked()), this, SLOT(changeCameraClicked()));
    connect(ui->button_accept, SIGNAL(clicked()), this, SLOT(acceptClicked()));
    connect(ui->button_deny, SIGNAL(clicked()), this, SLOT(denyClicked()));
    connect(ui->button_recalibrate, SIGNAL(clicked()), this, SLOT(recalibrateClicked()));
    connect(ui->radio_fixedwindow, SIGNAL(toggled(bool)), this, SLOT(radioFixedWindow(bool)));
    connect(ui->radio_touchpadlike, SIGNAL(toggled(bool)), this, SLOT(radioTouchpadLike(bool)));
    connect(ui->radio_fullscreen, SIGNAL(toggled(bool)), this, SLOT(radioFullscreen(bool)));

    connect(ui->checkbox_lefthanded, SIGNAL(toggled(bool)), this, SLOT(changeLeftHanded(bool)));
    connect(ui->mrw, SIGNAL(textChanged()), this, SLOT(changeMouseFrameSize()));
    connect(ui->mrh, SIGNAL(textChanged()), this, SLOT(changeMouseFrameSize()));

    changeMouseFrameSize();

    thread_ = new ImgProcThread(this, ui->label, &mouse_);
    thread_->start();
}

void MainWindow::changeMouseFrameSize()
{
    mouse_.setFrameSize(ui->mrw->toPlainText().toInt(), ui->mrh->toPlainText().toInt());
    int w, h;
    mouse_.getFrameSize(w, h);
    if(ui->mrw->toPlainText().toInt() != w)
        ui->mrw->setPlainText(QString::number(w));
    if(ui->mrh->toPlainText().toInt() != h)
        ui->mrh->setPlainText(QString::number(h));
}

void MainWindow::radioFixedWindow(bool on)
{
    if(on)
        mouse_.setMode(Mouse::MODE_FIXED_WINDOW);
}

void MainWindow::radioTouchpadLike(bool on)
{
    if(on)
        mouse_.setMode(Mouse::MODE_TOUCHPADLIKE);
}

void MainWindow::radioFullscreen(bool on)
{
    if(on)
        mouse_.setMode(Mouse::MODE_FULLSCREEN);
}

void MainWindow::changeLeftHanded(bool on)
{
    thread_->lefthanded = on;
}

#include <iostream>
using namespace std;

void MainWindow::closeEvent(QCloseEvent * e)
{
    stopThread();
    mouse_.hideBorder();
}

void MainWindow::message(QString str)
{
    ui->label_accept_calibration->setVisible(false);
    ui->label_calibrating->setVisible(false);
    ui->button_accept->setVisible(false);
    ui->button_deny->setVisible(false);
    ui->button_recalibrate->setVisible(false);
    ui->label_starting->setVisible(true);
    ui->label_starting->setText(str);
}

void MainWindow::stopThread()
{
    if(thread_)
    {

        thread_->decideCalibration(false); //just in case
        thread_->askToQuit();
        if(!thread_->wait(5000))
        {
            thread_->terminate();
            thread_->wait();
        }

        delete thread_;
        thread_ = NULL;
    }
}

MainWindow::~MainWindow()
{
    stopThread();
    delete ui;
}

void MainWindow::enableRecalibrate()
{
    ui->button_recalibrate->setVisible(true);
    ui->label_calibrating->setVisible(false);
    mouse_.showBorder();
}

void MainWindow::startedCalibrating()
{
    ui->label_starting->setVisible(false);
    ui->label_calibrating->setVisible(true);
}

void MainWindow::startedCalibrated()
{
    ui->label_starting->setVisible(false);
    ui->button_recalibrate->setVisible(true);
    ui->label_calibrating->setVisible(false);
    mouse_.showBorder();
}

void MainWindow::enableAcceptDeny()
{
    ui->button_accept->setVisible(true);
    ui->button_deny->setVisible(true);
    ui->label_calibrating->setVisible(false);
    ui->label_accept_calibration->setVisible(true);
}

void MainWindow::changeCameraClicked()
{
    thread_->askToChangeCamera();
}

void MainWindow::recalibrateClicked()
{
    ui->button_recalibrate->setVisible(false);
    thread_->askToRecalibrate();
    ui->label_calibrating->setVisible(true);
    mouse_.hideBorder();
}

void MainWindow::acceptClicked()
{
    ui->button_accept->setVisible(false);
    ui->button_deny->setVisible(false);
    ui->label_accept_calibration->setVisible(false);
    ui->button_recalibrate->setVisible(true);
    mouse_.showBorder();
    thread_->decideCalibration(true);
}

void MainWindow::denyClicked()
{
    ui->button_accept->setVisible(false);
    ui->button_deny->setVisible(false);
    ui->label_accept_calibration->setVisible(false);
    ui->label_calibrating->setVisible(true);
    thread_->decideCalibration(false);
}

