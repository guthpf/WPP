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

#ifndef CLICKDETECT_H
#define CLICKDETECT_H

#include "wppmatrix.h"
#include "wpppoint.h"

//Adaptative hysteresis filter to detect clicking
class ClickDetector
{
    bool touching;

    double sd_low_, sd_high_;
    const double sd_low0_, sd_high0_;

public:

    ClickDetector():
        sd_low0_(.2),
        sd_high0_(.3)
    {
        sd_low_ = sd_low0_;
        sd_high_ = sd_high0_;
    }

    void reset()
    {
        touching = false;
    }

private:
    //Variance condition for clicking
    bool touching_cond2(const WPP::Matrix<WPP::RGB> & img,  WPP::Matrix<WPP::RGB> & view, double img_norm, double xtip, double ytip, double & sdmetric);

    //learning iteration for when the variance is considered "high"
    void update_sd_threshold_high(double sdmetric)
    {
        sd_high_ = .8 * sd_high_ + .2 * sdmetric;
    }

    //learning iteration for when the variance is considered "low"
    void update_sd_threshold_low(double sdmetric)
    {
        sd_low_ = .8 * sd_low_ + .2 * sdmetric;
    }

    //learning iteration for when the variance is unknown
    void update_sd_threshold_neutral()
    {
        sd_high_ = .95 * sd_high_ + .05 * sd_high0_;
        sd_low_ = .95 * sd_low_ + .05 * sd_low0_;
    }

public:

    //Returns true if a click is detected in the current frame
    bool computeClicking(
            const WPP::Matrix<WPP::RGB> & img, //input image
            WPP::Matrix<WPP::RGB> & view, //feedback image
            double img_norm, //result from normalize()
            double xtip, //pen tip x
            double ytip, //pen tip y
            double sytip, //shadow tip y
            double drawx, //result from rectification
            double drawy);

};

#endif // CLICKDETECT_H
