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

#ifndef SHADOWTRACK_H
#define SHADOWTRACK_H

#include "wpppoint.h"
#include "wppmatrix.h"

#define SHADOW_MAX_H 50

//Finds and returns shadow tip y position given pen cap tip position
double shadowtrack(
        double pxtip, //pen tip position from pentrack()
        double pytip,
        double img_norm, //result from normalization
        const WPP::Matrix<WPP::RGB> & img, //input image
        WPP::Matrix<WPP::RGB> & view, //feedback image
        bool flefthanded //whether left-handed mode is on or not
        );

#endif // SHADOWTRACK_H
