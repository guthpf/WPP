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

#ifndef WPPPOINT_H
#define WPPPOINT_H

namespace WPP
{
    typedef unsigned char RGB[3];

    inline int SUM(const RGB & x)
    {
        return (int)x[0] + (int)x[1] + (int)x[2];
    }

    struct Point
    {
        double y;
        double x;
        Point(){}
        Point(double y_, double x_)
        {
            y=y_;
            x=x_;
        }
    };
}

template <typename T>
T inline sqr(T x)
{
    return x*x;
}

inline void to_white(WPP::RGB & rgb)
{
    rgb[0] = 0xFF;
    rgb[1] = 0xFF;
    rgb[2] = 0xFF;
}

inline void to_black(WPP::RGB & rgb)
{
    rgb[0] = 0;
    rgb[1] = 0;
    rgb[2] = 0;
}

inline void to_yellow(WPP::RGB & rgb)
{
    rgb[0] = 0;
    rgb[1] = 0xFF;
    rgb[2] = 0xFF;
}

inline void to_orange(WPP::RGB & rgb)
{
    rgb[0] = 0;
    rgb[1] = 0x80;
    rgb[2] = 0xFF;
}

inline void to_magenta(WPP::RGB & rgb)
{
    rgb[0] = 0xFF;
    rgb[1] = 0;
    rgb[2] = 0xFF;
}

inline void to_red(WPP::RGB & rgb)
{
    rgb[0] = 0;
    rgb[1] = 0;
    rgb[2] = 0xFF;
}

inline void darken_red(WPP::RGB & rgb)
{
    rgb[0] = 0;
    rgb[1] = 0;
}

inline void darken_yellow(WPP::RGB & rgb)
{
    rgb[0] = 0;
}

inline void darken_light_blue(WPP::RGB & rgb)
{
    rgb[1] /= 2;
    rgb[2] /= 2;
}

#endif // WPPPOINT_H
