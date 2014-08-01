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

#include "mouse.h"

#include <stdio.h>
#include <unistd.h>
#ifdef WIN32
#include <windows.h>
#else
#include <linux/input.h>
#endif
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <string.h>

#include <iostream>
#include <fstream>

#include <cstdlib>
#ifdef WIN32
#else
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif

#include <cmath>

#include <QBitmap>

#ifdef WIN32
#include <QCoreApplication>
#endif

using namespace std;

#ifdef WIN32
#else
//searchs in /proc/bus/input/devices for a mouse file
//returns a string with a number so that /dev/input/event<number> can be used to send mouse click events. (Mouse movements are performed instead with libX11)
string get_mouse_event_file_number()
{
    ifstream f("/proc/bus/input/devices");
    char chars[1000];
    while(true)
    {
        f.getline(chars, sizeof(chars));
        if(f.eof() || f.bad())
            break;

        string str(chars);
        if(chars[0] == 'H' && str.find("mouse")!=-1)
        {
            int p0 = str.find("event")+5;
            if(p0==-1)
                continue;
            int p1 = str.find(" ", p0);
            if(p1==-1)
                return str.substr(p0);
            else
                return str.substr(p0, p1 - p0);
        }
    }
    return "";
}
#endif

Mouse::Mouse(MainWindow * parent): QObject(parent)
{
    mw_ = parent;
    this->lostcursorcount_ = 3;
#ifdef WIN32
#else
    mouse_event_file_number_ = get_mouse_event_file_number();
#endif
    mode_ = MODE_FULLSCREEN;

    border_ = new QLabel;
#ifdef WIN32
    //border_->setAttribute(Qt::WA_ShowWithoutActivating);
    border_->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::WindowTransparentForInput);
#else
    border_->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
    border_->setWindowFlags(Qt::X11BypassWindowManagerHint);
#endif
    stdw_=0;
    stdh_=0;
    setFrameSize(640,480);
    x_ = stdw_/2;
    y_ = stdh_/2;
    no_mouse_move_ = false;

    connect(this,SIGNAL(sg_reset_()), this, SLOT(reset_()));
    connect(this,SIGNAL(sg_setPositionAndButton_(double,double,bool)), this, SLOT(setPositionAndButton_(double,double,bool)));
}

void Mouse::getFrameSize(int & w, int & h)
{
    w = stdw_;
    h = stdh_;
}

void Mouse::setFrameSize(int w, int h)
{
    if(w<1)w=1;
    if(h<1)h=1;
    if(w>2000)w=2000;
    if(h>2000)h=2000;
    if(w != stdw_ || h != stdh_)
    {
        border_->setGeometry(600,90,w+2,h+2);
        stdw_ = border_->geometry().width()-2;
        stdh_ = border_->geometry().height()-2;

        //set checked pattern
        QImage image(stdw_+2, stdh_+2, QImage::Format_ARGB32);
        for(int x = 0; x < stdw_+2; x++)
            for(int y = 0; y < stdh_+2; y++)
                image.setPixel(x,y,((x+y)%2)?0xff000000:0xffffffff);
        border_->setPixmap(QPixmap::fromImage(image));

        //set transparent region
        for(int x = 0; x < stdw_+2; x++)
            for(int y = 0; y < stdh_+2; y++)
                if(x==0||x==stdw_+1||y==0||y==stdh_+1)
                    image.setPixel(x,y,0);
                else
                    image.setPixel(x,y,0xffffff);
        border_->setMask(QBitmap::fromImage(image));
    }
}

void Mouse::hideBorder()
{
    border_->hide();
}

void Mouse::showBorder()
{
    if(mode_!=MODE_FULLSCREEN && !mw_->isHidden())
    {
#ifdef WIN32

        HWND focus_window = GetForegroundWindow(); //get focus window
        border_->show(); //show border
        QCoreApplication::processEvents(); //wait border get the focus
        if(focus_window != 0)
            SetForegroundWindow(focus_window); //Give the focus back to focus_window
#else
        border_->show();
#endif
    }
}

Mouse::~Mouse()
{
    delete border_;
}

void Mouse::reset_()
{
    lostcursorcount_++;
    if(clickinglast_)
        up_();
    clickinglast_ = false;

    if(lostcursorcount_>2 && mode_==MODE_TOUCHPADLIKE)
        hideBorder();

#ifdef WIN32
    int root_x, root_y;
    POINT pt;
    GetCursorPos(&pt);
    root_x = pt.x;
    root_y = pt.y;

    border_->setGeometry(root_x - x_-1, root_y - y_-1, stdw_+2, stdh_+2);

#else
    int root_x, root_y;
    Display * display = XOpenDisplay(0);
    Window root0 = DefaultRootWindow(display);
    Window root;
    Window child;
    int win_x, win_y;
    uint buttons;

    XQueryPointer(
        display,
        root0,
        &root,
        &child,
        &root_x,
        &root_y,
        &win_x,
        &win_y,
        &buttons);

    XCloseDisplay(display);

    border_->setGeometry(root_x - x_-1, root_y - y_-1, stdw_+2, stdh_+2);
#endif
}

void Mouse::setMode(Mode mode)
{
    if(mode_==MODE_FULLSCREEN)
    {
        //Puts window in a position so that the mouse is located at the center
        x_ = stdw_/2;
        y_ = stdh_/2;
        reset_();

        //Don't move the mouse back to the previous position when setPosition_ is called
        no_mouse_move_ = true;
    }
    mode_ = mode;
    if(mode==MODE_FULLSCREEN)
        hideBorder();
    else
        showBorder();
}

void Mouse::setPositionAndButton_(double x, double y, bool clicking)
{
    if(mode_==MODE_FULLSCREEN)
    {
#ifdef WIN32
        RECT desktop;
        GetWindowRect(GetDesktopWindow(), &desktop);
        int scrw = desktop.right;
        int scrh = desktop.bottom;
#else
        Display* disp = XOpenDisplay(NULL);
        Screen*  scrn = DefaultScreenOfDisplay(disp);
        int scrw = scrn->width;
        int scrh = scrn->height;
        XCloseDisplay(disp);
#endif
        x *= scrw;
        y *= scrh;
    }
    else
    {
        x *= stdw_;
        y *= stdh_;
    }
    if(!clicking && clickinglast_)
        up_();
    setPosition_(x, y);
    if(clicking && !clickinglast_)
        down_();
    clickinglast_ = clicking;
}

#include <QCursor>

void Mouse::setPosition_(double x, double y)
{

    if(lostcursorcount_ > 2 && mode_==MODE_TOUCHPADLIKE || //this helps stabilize the touchpadlike mode
            no_mouse_move_ && mode_ != MODE_FULLSCREEN //when exiting the fullscreen mode, this helps keep the mouse fixed and setting the border window in an appropriate position
            )
    {
        x_ = (int)(x+.5);
        y_ = (int)(y+.5);
        showBorder();
        no_mouse_move_ = false;
    }

    lostcursorcount_ = 0;

    //Histeresis
    int xnew = (abs(x-x_)<1.?x_:(int)(x+.5));
    int ynew = (abs(y-y_)<1.?y_:(int)(y+.5));

    //cout << "@@@" << x << "," << y << "|" << x_ << "," << y_ << "," << xnew << "," << ynew << "@@@";

    int root_x, root_y; //mouse position (acc. to OS)

#ifdef WIN32
    //get (global) mouse position
    POINT pt;
    GetCursorPos(&pt);
    root_x = pt.x;
    root_y = pt.y;

    //set new mouse position (global)
    if(mode_==MODE_FULLSCREEN)
    {
        int x = xnew;
        int y = ynew;

        INPUT Input={0};
        Input.type = INPUT_MOUSE;
        Input.mi.dx = (LONG)ceil(x*((double)0x10000)/GetSystemMetrics(SM_CXSCREEN));
        Input.mi.dy = (LONG)ceil(y*((double)0x10000)/GetSystemMetrics(SM_CYSCREEN));
        Input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
        SendInput(1,&Input,sizeof(INPUT));
    }
    else
    {
        if(!(xnew==x_ && ynew==y_))
        {
            //new mouse position (acc. to OS) = old mouse position (acc. to OS) + new mouse position (relative to the border window) - old mouse position (relative to the border window)
            int x = root_x + xnew - x_;
            int y = root_y + ynew - y_;

            INPUT Input={0};
            Input.type = INPUT_MOUSE;
            Input.mi.dx = (LONG)ceil(x*((double)0x10000)/GetSystemMetrics(SM_CXSCREEN));
            Input.mi.dy = (LONG)ceil(y*((double)0x10000)/GetSystemMetrics(SM_CYSCREEN));
            Input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
            SendInput(1,&Input,sizeof(INPUT));
        }
    }

#else
    //get (global) mouse position
    Display * display = XOpenDisplay(0);
    Window root0 = DefaultRootWindow(display);
    Window root;
    Window child;
    int win_x, win_y;
    uint buttons;
    XQueryPointer(
        display,
        root0,
        &root,
        &child,
        &root_x,
        &root_y,
        &win_x,
        &win_y,
        &buttons);

    //set new mouse position (global)
    if(mode_==MODE_FULLSCREEN)
        XWarpPointer(display, None, root0, 0, 0, 0, 0,
            xnew,
            ynew);
    else
    {
        if(!(xnew==x_ && ynew==y_))
        {
            //new mouse position (acc. to OS) = old mouse position (acc. to OS) + new mouse position (relative to the border window) - old mouse position (relative to the border window)
            XWarpPointer(display, None, root0, 0, 0, 0, 0,
                root_x + xnew - x_,
                root_y + ynew - y_);
        }
    }

    XCloseDisplay(display);

#endif


    //set border window position

    cout << "*" << x_ << " " << y_ << endl;
    if(mode_==MODE_FULLSCREEN)
        //If mode is fullscreen, then the border surface is invisible, but we'd like it to be in this position when we quit fullscreen mode
        border_->setGeometry(root_x - stdw_/2-1, root_y - stdh_/2-1, stdw_+2, stdh_+2);
    else
        border_->setGeometry(root_x - x_-1, root_y - y_-1, stdw_+2, stdh_+2);

    x_ = xnew;
    y_ = ynew;
}


int Mouse::up_()
{
#ifdef WIN32
    INPUT Input={0};
    Input.type = INPUT_MOUSE;
    Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
    SendInput(1,&Input,sizeof(INPUT));
    cout << "***MOUSEUP***" << endl;
#else
    struct input_event event1, event2, event_end;

    int fd = open(("/dev/input/event" + mouse_event_file_number_).c_str(), O_RDWR);
    if(!fd){
        std::cout << "**********************8" << std::endl;
        printf("Error open mouse:%s\n", strerror(errno));
        return -1;
    }
    memset(&event1, 0, sizeof(event1));
    memset(&event2, 0, sizeof(event2));
    memset(&event_end, 0, sizeof(event_end));

    gettimeofday(&event1.time, NULL);
    event2.time = event1.time;
    event_end.time = event1.time;

    cout << "***MOUSEUP***" << endl;

    event1.type = 4;
    event1.code = 4;
    event1.value = 0x00090001;

    event2.type = 1;
    event2.code = BTN_LEFT;
    event2.value = 0;

    event_end.type = EV_SYN;
    event_end.code = SYN_REPORT;
    event_end.value = 0;

    write(fd, &event1, sizeof(event1));
    write(fd, &event2, sizeof(event2));
    write(fd, &event_end, sizeof(event_end));
    close(fd);
#endif
    return 0;
}

int Mouse::down_()
{
#ifdef WIN32
    INPUT Input={0};
    Input.type = INPUT_MOUSE;
    Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    SendInput(1,&Input,sizeof(INPUT));
    cout << "***MOUSEDOWN***" << endl;
#else
    struct input_event event1, event2, event_end;

    int fd = open(("/dev/input/event" + mouse_event_file_number_).c_str(), O_RDWR);
    if(!fd){
        std::cout << "**********************8" << std::endl;
        printf("Error open mouse:%s\n", strerror(errno));
        return -1;
    }
    memset(&event1, 0, sizeof(event1));
    memset(&event2, 0, sizeof(event2));
    memset(&event_end, 0, sizeof(event_end));

    gettimeofday(&event1.time, NULL);
    event2.time = event1.time;
    event_end.time = event1.time;

    cout << "***MOUSEDOWN***" << endl;

    event1.type = 4;
    event1.code = 4;
    event1.value = 0x00090001;

    event2.type = 1;
    event2.code = BTN_LEFT;
    event2.value = 1;

    event_end.type = EV_SYN;
    event_end.code = SYN_REPORT;
    event_end.value = 0;

    write(fd, &event1, sizeof(event1));
    write(fd, &event2, sizeof(event2));
    write(fd, &event_end, sizeof(event_end));
    close(fd);
#endif
    return 0;
}

void Mouse::reset()
{
    emit sg_reset_();
}

void Mouse::setPositionAndButton(double x, double y, bool clicking)
{
    emit sg_setPositionAndButton_(x, y, clicking);
}

