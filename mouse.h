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

#ifndef MOUSE_H
#define MOUSE_H

#include <string>

#if QT_VERSION >= 0x050000
#include <QtWidgets/QLabel>
#else
#include <QLabel>
#endif

class MainWindow;

//Sets mouse position and peforms left button clicks. Also controls a border window delimitating controllable area, and handles interaction mode transitions.
class Mouse: public QObject
{
    Q_OBJECT

    int stdw_, stdh_;
    QLabel * border_;
    int x_, y_;
    int lostcursorcount_;
    bool no_mouse_move_;
    bool clickinglast_;
#ifdef WIN32
#else
    ::std::string mouse_event_file_number_;
#endif

    MainWindow * mw_;

    //methods called by setPositionAndButton_ and reset_. Moves mouse and sets border position.
    void setPosition_(double x, double y);
    int up_(); //left button up
    int down_(); //left button down

signals:

    void sg_reset_();
    void sg_setPositionAndButton_(double x, double y, bool clicking);

private slots:

    void reset_();
    void setPositionAndButton_(double x, double y, bool clicking);

public:
    Mouse(MainWindow * parent);

    //sets mouse position and left button state
    void setPositionAndButton(double x, double y, bool clicking);

    //this method should be called when the mouse cursor position cannot be computed (as opposed to setPositionAndButton)
    void reset();

    //operations on the dotted window
    void getFrameSize(int & w, int & h);
    void setFrameSize(int w, int h);
    void showBorder();
    void hideBorder();

    virtual ~Mouse();
    bool error()
    {
#ifdef WIN32
        return false;
#else
        return mouse_event_file_number_ == "";
#endif
    }

    enum Mode{
        MODE_FIXED_WINDOW,
        MODE_TOUCHPADLIKE,
        MODE_FULLSCREEN
    };

    void setMode(Mode mode);
    Mode getMode();

private:
    Mode mode_;
};

#include "mainwindow.h"

#endif // MOUSE_H
