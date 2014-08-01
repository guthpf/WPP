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

#ifndef WPPVIDEOCAPTURE_H
#define WPPVIDEOCAPTURE_H

#include "wpppoint.h"
#include "wppmatrix.h"

#include "escapi.h"
#include <unistd.h>

#ifdef WIN32
#else
#include <opencv2/highgui/highgui.hpp>
#endif

namespace WPP
{
    //Provides access to webcam
    class VideoCapture
    {
    private:
        bool error_;
#ifdef WIN32
        struct SimpleCapParams capture_;
        int devices_;
#else
        cv::VideoCapture vid_;
#endif
        int device_;
        int w_, h_;

    public:
        VideoCapture(int w, int h);

        //Tries to change camera (device_ := device_ + 1 (mod #devices) )
        //Sets error_ to true if failed
        void changeCamera();

        bool isOpened()
        {
            return !error_;
        }

        //reads data from camera
        void operator>>(Matrix<RGB> & m);

        virtual ~VideoCapture();
    };
}

#endif // WPPVIDEOCAPTURE_H
