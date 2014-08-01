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

#include "wppvideocapture.h"
using namespace WPP;

VideoCapture::VideoCapture(int w, int h)
{
#ifdef WIN32
    devices_ = setupESCAPI();
    device_ = devices_ - 1;

    error_ = (devices_ == 0);

    capture_.mWidth = w;
    capture_.mHeight = h;
    capture_.mTargetBuf = new int[capture_.mWidth * capture_.mHeight];

    error_ = error_ || (initCapture(device_, &capture_) == 0);
#else
    for(device_=1; device_>=0; device_--)
    {
        vid_.open(device_);
        if(vid_.isOpened())
            break;
    }
    error_ = (device_==-1);
    if(error_)
        return;

    w_ = w;
    h_ = h;
    vid_.set(CV_CAP_PROP_FRAME_WIDTH, w_);
    vid_.set(CV_CAP_PROP_FRAME_HEIGHT, h_);
#endif
}

void VideoCapture::changeCamera()
{
#ifdef WIN32
    deinitCapture(device_);
    devices_ = countCaptureDevices();
    error_ = (devices_ == 0);
    device_++;
    if(device_>=devices_)
        device_ = 0;
    error_ = error_ || (initCapture(device_, &capture_) == 0);
#else
    device_++;
    vid_.open(device_);
    if(!vid_.isOpened())
    {
        device_=0;
        vid_.open(device_);
        if(!vid_.isOpened())
            error_ = true;
    }
#endif
}

void VideoCapture::operator>>(Matrix<RGB> & m)
{
#ifdef WIN32
    doCapture(device_);
    int count = 0;
    while (isCaptureDone(device_) == 0)
    {
        count++;
        usleep(1000);
        if(count>1000)
        {
            changeCamera();
            doCapture(device_);
            count = 0;
            while (isCaptureDone(device_) == 0)
            {
                count++;
                usleep(1000);
                if(count>1000)
                {
                    error_ = true;
                    return;
                }
            }
            break;
        }
    }

    int rows = capture_.mHeight;
    int cols = capture_.mWidth;
    m = Matrix<RGB>(rows, cols);
    unsigned char * p1 = &(m.at(0,0)[0]);
    unsigned char * p2 = (unsigned char *) capture_.mTargetBuf;
    int n = rows * cols;
    for(int i = 0; i < n; i++)
    {
        *(p1++) = *(p2++);
        *(p1++) = *(p2++);
        *(p1++) = *(p2++);
        p2++;
    }
#else
    cv::Mat tmp;
    vid_ >> tmp;

    int rows = tmp.rows;
    int cols = tmp.cols;
    ::std::cout << "W(" <<  rows << " " << cols << ") ";
    m = Matrix<RGB>(h_, w_);
    for(int i = 0; i < h_; i++)
    {
        unsigned char * p1 = &(m.at(i+(rows-h_)/2,(cols-w_)/2)[0]);
        unsigned char * p2 = &(tmp.at<RGB>(i,0)[0]);
        for(int j = 0; j < w_; j++)
        {
            *(p1++) = *(p2++);
            *(p1++) = *(p2++);
            *(p1++) = *(p2++);
        }
    }
#endif
}

VideoCapture::~VideoCapture()
{
#ifdef WIN32
    deinitCapture(device_);
    delete[] capture_.mTargetBuf;
#endif
}
