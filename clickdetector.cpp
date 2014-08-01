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

#include "clickdetector.h"

using namespace std;

bool ClickDetector::touching_cond2(const WPP::Matrix<WPP::RGB> & img, WPP::Matrix<WPP::RGB> & view, double img_norm, double xtip, double ytip, double & sdmetric)
{
    const int arH = 6;
    const int arW = 30;
    const double aydelta = arH/2;
    const double axdelta = arW/2;

    if(ytip + arW - aydelta >= img.rows || xtip + arH - axdelta >= img.cols || ytip - aydelta < 1 || xtip - axdelta < 1)
        return false;

    double around_mean = 0;
    double around_mean2 = 0;
    int around_count = 0;
    int around_max = 0;

    for(int y = 0; y < arH; y++)
        for(int x = 0; x < arW; x++)
        {
            int yt = ytip+y-aydelta;
            int xt = xtip+x-axdelta;
            if(yt >= 0 && yt < img.rows && xt >= 0 && xt < img.cols)
            {
                around_count++;
                int val = WPP::SUM(img.at(yt,xt));
                if(val>around_max)
                    around_max = val;
                around_mean += val;
                around_mean2 += val*val;
            }
        }
    if(around_count>1)
    {
        around_mean /= around_count;
        around_mean2 /= around_count;
        double around_sdev = sqrt((around_mean2 - around_mean*around_mean)*around_count/(around_count-1.));
        sdmetric = (around_sdev/around_mean);

        return (touching?(sdmetric > .9*sd_low_+.1*sd_high_):(sdmetric > .4*sd_low_+.6*sd_high_));
    }
    else
    {
        sdmetric = 0./0.;
        return touching;
    }
}

bool ClickDetector::computeClicking(
        const WPP::Matrix<WPP::RGB> & img, //input image
        WPP::Matrix<WPP::RGB> & view, //feedback image
        double img_norm, //result from normalize()
        double xtip, //pen tip x
        double ytip, //pen tip y
        double sytip, //shadow tip y
        double drawx, //result from rectification
        double drawy)
{
    if(!(drawy >= 0 && drawy < 1 && drawx >= 0 && drawx < 1))
    {
        touching = false;
        update_sd_threshold_neutral();
    }
    else
    {
        double sdmetric;
        bool cond2 = touching_cond2(img, view, img_norm, xtip, ytip, sdmetric);
        cout << " " << sdmetric << '[' << sd_low_ << ',' << sd_high_ << "] ";
        touching = (sytip != -1 && sytip - ytip <= 7 && cond2);

        if(ytip != -1 && sdmetric==sdmetric /*avoiding NaN, just in case*/)
        {
            if(touching)
                update_sd_threshold_high(sdmetric);
            else
                update_sd_threshold_low(sdmetric);
        }
        else
            update_sd_threshold_neutral();
    }
    return touching;
}
