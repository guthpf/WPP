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

#include "shadowtrack.h"
#include "pentrack.h"
#include <vector>
using namespace std;

struct Ytype
{
    double y;
    bool reg;

    Ytype(){}

    Ytype(double y_, bool reg_)
    {
        y = y_;
        reg = reg_;
    }

    bool operator<(const Ytype & other) const
    {
        return y < other.y;
    }
};

double shadowtrack(
        double pxtip,
        double pytip,
        double img_norm,
        const WPP::Matrix<WPP::RGB> & img,
        WPP::Matrix<WPP::RGB> & view,
        bool flefthanded)
{
    double ytip = -1;

    if(pytip!=-1)
    {
        int ymin = pytip-10;
        int ymax = pytip+SHADOW_MAX_H;
        int xmin = pxtip-(flefthanded?4:65);
        int xmax = pxtip+(flefthanded?65:4);
        if(ymin<0)ymin=0;
        if(ymax>img.rows-1)ymax=img.rows-1;
        if(xmin<0)xmin=0;
        if(xmax>img.cols)xmax=img.cols;


        //STEP 1. DEFINE PAPER-WHITE THRESHOLD

        int ysample = pytip+15; //will sample pixels from line pytip+15
        if(ysample>=img.rows)
            ysample = img.rows-1;

        //Find maximum intensity of line ysample
        int wmax = 0;
        for(int x = xmin; x < xmax; x++)
        {
            int s = WPP::SUM(img.at(ysample,x));
            if(s>wmax)
                wmax=s;
        }

        //Get mean intensity of pixels above .75*wmax
        int wcount = 0;
        int wsum = 0;
        for(int x = xmin; x < xmax; x++)
        {
            int s = WPP::SUM(img.at(ysample,x));
            if(s>.75*wmax)
            {
                wsum += s;
                wcount++;
            }
        }
        //and set threshold to .75 of this mean
        int whitec = (wcount?wsum/wcount:wmax) * .75;
        cout << "W[" << whitec << "]";

        //Draw tracking Window
        for(int y = ymax-1; y >= ymin; y--)
        {
            to_white(view.at(y,xmin));
            to_white(view.at(y,xmax-1));
        }
        for(int x = xmin; x < xmax; x++)
        {
            to_white(view.at(ymin,x));
            to_white(view.at(ymax-1,x));
        }

        //draw pen-blue points in yellow
        int pbth = penblue_threshold(img_norm);
        for(int y = ymax-1; y >= ymin; y--)
            for(int x = xmin; x < xmax; x++)
                if(penblue(pbth, img.at(y,x)))
                    to_yellow(view.at(y,x));

        //Draw pen tip position
        for(int y = (int)pytip-1; y <= (int)pytip+1; y++)
            for(int x = (int)pxtip-1; x <= (int)pxtip+1; x++)
                to_yellow(view.at(y,x));

        //STEP 2. FIND SHADOW

        int ybeg = ymax-1;
        double glim = .7*(xmax-xmin); //threshold
        for(int y = ybeg /*ymax-1*/; y >= ymin; y--)
        {
            //count pixels greater than paper-white threshold
            double greater = 0;
            {
                const WPP::RGB * rgbptr = &img.at(y,xmin);
                for(int x = xmin; x < xmax; x++)
                {
                    if(WPP::SUM(*rgbptr) > whitec)
                        greater++;
                    rgbptr++;
                }
            }

            if(greater < glim)
            {
                //STEP 3. INTERPOLATION BY SORTING WITH GAMMA FILTER

                //put transition points in a list and sort
                const WPP::RGB * rgbptru = &img.at(y,xmin);
                const WPP::RGB * rgbptrd = &img.at(y+1,xmin);
                vector<Ytype> yv;
                double gamma = 2.2;
                for(int x = xmin; x < xmax; x++)
                {
                    int su = WPP::SUM(*rgbptru);
                    int sd = WPP::SUM(*rgbptrd);
                    if(su > whitec && sd <= whitec)
                        yv.push_back(Ytype((pow(su,gamma)-pow(whitec,gamma))/(double)(pow(su,gamma)-pow(sd,gamma)),false));
                    else if(su <= whitec && sd > whitec)
                        yv.push_back(Ytype((pow(su,gamma)-pow(whitec,gamma))/(double)(pow(su,gamma)-pow(sd,gamma)),true));
                    rgbptru++;
                    rgbptrd++;
                }
                sort(yv.begin(), yv.end());
                bool found = false;

                //search threshold cut
                for(int i = 0; i < yv.size(); i++)
                {
                    if(yv[i].reg)
                        greater++;
                    else
                        greater--;
                    if(greater>=glim)
                    {
                        ytip = y + yv[i].y;
                        found = true;
                        break;
                    }
                }
                if(!found)
                    ytip=-1;
                break;
            }
        }

        //If this is true, then probably there is no shadow in the image
        if(ytip < pytip - 3)
            ytip = -1;

        //draw shadow tip position (projected from pxtip)
        if(ytip!=-1)
            for(int y = (int)ytip-1; y <= (int)ytip+1; y++)
                for(int x = (int)pxtip-1; x <= (int)pxtip+1; x++)
                    to_magenta(view.at(y,x));
    }

    return ytip;
}
