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

#include "imgprocthread.h"

using namespace std;

#include "calibrator.h"
#include "pentrack.h"
#include "clickdetector.h"
#include "shadowtrack.h"
#include "normalize.h"
#include "wppvideocapture.h"

int ImgProcThread::fmain()
{
    //Init video capture. If there are two cameras connected, pick the second one (the first one will probably be a webcam that comes inside the monitor, for note- and netbooks)

    WPP::VideoCapture vid(640, 480);
    if(!vid.isOpened())
        return -1;

    WPP::Matrix<WPP::RGB> cfr_raw;

    Calibrator calib;
    if(calib.isDefined())
        emit startedCalibrated();
    else
        emit startedCalibrating();

    ClickDetector clk;

    while(true)
    {
        //If mainwindow changed mode to left-handed, update
        bool flefthanded = lefthanded;

        if(command_change_camera_)
        {
            command_change_camera_ = false;
            vid.changeCamera();
            if(!vid.isOpened())
                return -3;
        }

        vid >> cfr_raw;
        if(!vid.isOpened())
            return -4;
        if(quit_)
            break;

        double img_norm = normalize(cfr_raw);

        WPP::Matrix<WPP::RGB> view = cfr_raw.clone();

        if(!calib.isDefined())
        {
            clk.reset();
            if(!calib.waitingAcceptance())
            {
                if(calib.findCrosses(img_norm, cfr_raw, view))
                {
                    decided_ = false;
                    emit enableAcceptDeny();
                }
            }
            else
            {
                if(!decided_)
                    calib.drawFoundCalibration(view);
                else
                {
                    calib.decide(accepted_);
                    if(accepted_)
                        emit enableRecalibrate();
                }
            }
        }
        else
        {
            double xtip, ytip;
            pentrack(img_norm, calib.rectification(), cfr_raw, view, flefthanded, xtip, ytip);

            double sytip = shadowtrack(xtip, ytip, img_norm, cfr_raw, view, flefthanded);

            WPP::Point px = calib.rectification().rectifyPoint(xtip, sytip);

            bool touching = clk.computeClicking(cfr_raw, view, img_norm, xtip, ytip, sytip, px.x, px.y);

            cout << endl;

            if(ytip==-1 || px.x < 0 || px.y < 0 || px.x >= 1 || px.y >= 1 || sytip == -1)
                mouse_->reset();
            else
                mouse_->setPositionAndButton(px.x, px.y, touching);
        }

        fbv_->setImage(view);

        if(command_recalibrate_)
        {
            command_recalibrate_ = false;
            calib.reset();
        }

    }
    return 0;
}

ImgProcThread::ImgProcThread(MainWindow * parent, FeedbackViewer * fbv, Mouse * mouse)
{
    parent_ = parent;
    fbv_ = fbv;
    mouse_ = mouse;
    quit_ = false;
    command_recalibrate_ = false;
    lefthanded = false;
    accepted_ = false;
    command_change_camera_ = false;
    decided_ = true;
}

void ImgProcThread::run()
{
    connect(this, SIGNAL(enableRecalibrate()), parent_, SLOT(enableRecalibrate()));
    connect(this, SIGNAL(enableAcceptDeny()), parent_, SLOT(enableAcceptDeny()));
    connect(this, SIGNAL(startedCalibrating()), parent_, SLOT(startedCalibrating()));
    connect(this, SIGNAL(startedCalibrated()), parent_, SLOT(startedCalibrated()));
    connect(this, SIGNAL(message(QString)), parent_, SLOT(message(QString)));
    switch(fmain())
    {
    case -1:
        emit message("FATAL ERROR:\nNo webcam connected.");
        break;
    case -2:
        emit message("FATAL ERROR:\nBad webcam resolution.");
        break;
    case -3:
        emit message("FATAL ERROR:\nCould not change webcam.");
        break;
    case -4:
        emit message("FATAL ERROR:\nCould not communicate with webcam.");
        break;
    }
}

void ImgProcThread::askToQuit()
{
    quit_ = true;
}

void ImgProcThread::askToRecalibrate()
{
    command_recalibrate_ = true;
}

void ImgProcThread::askToChangeCamera()
{
    command_change_camera_ = true;
}

void ImgProcThread::decideCalibration(bool accept)
{
    accepted_ = accept;
    decided_ = true;
}

/*
TODO:
- move completely to Qt5
- bug X11 mouseclck (must write on system files manually, which requires a mouse connected and sudo)
- bug windows when moving WPP's mainwindow using the pen (screen won't move for a few seconds)
- Mac version
*/
