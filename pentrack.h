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

#ifndef PENTRACK_H
#define PENTRACK_H

#include "rectification.h"

//Theshold for "penblue" filter, given image norm
inline int penblue_threshold(double img_norm)
{
    return 60 * img_norm;
}

//Classifies an image point as pen or non-pen, given the result from penblue_threshold
inline bool penblue(int threshold, const WPP::RGB & rgb)
{
/*	return rgb[0] > 1.4 * rgb[1] &&
           rgb[0] > 1.4 * rgb[2] &&
           rgb[0] > 50;*/
    int b5 = 5*(int)rgb[0];
    return b5 > 8 * (int)rgb[1] &&
           b5 > 8 * (int)rgb[2] &&
           rgb[0] > threshold;
}

//Tries to find the pen tip from an image
void pentrack(
        double img_norm, //normalization result
        const Rectification & crosses, //calibration result
        const WPP::Matrix<WPP::RGB> & img, //input image
        WPP::Matrix<WPP::RGB> & view, //feedback image
        bool flefthanded, //whether the left-handed mode is on or not
        double & xtip, //(out) pen-tip position in pixels
        double & ytip);

#endif // PENTRACK_H
