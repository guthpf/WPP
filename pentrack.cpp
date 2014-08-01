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

#include "pentrack.h"
#include "shadowtrack.h"

//Returns a smooth (bilinearly interpolated) tilted (arctan(.5)) sobel filter result for a right-handed person
inline int smooth_sobel_rh(const WPP::Matrix<WPP::RGB> & img, double y, double x)
{
    const WPP::RGB & rgb1a = img.at(floor(y-1),floor(x-.5));
    const WPP::RGB & rgb1b = img.at(floor(y-1),ceil(x-.5));
    const WPP::RGB & rgb1c = img.at(ceil(y-1),floor(x-.5));
    const WPP::RGB & rgb1d = img.at(ceil(y-1),ceil(x-.5));
    const WPP::RGB & rgb2a = img.at(floor(y-.5),floor(x-1.5));
    const WPP::RGB & rgb2b = img.at(floor(y-.5),ceil(x-1.5));
    const WPP::RGB & rgb2c = img.at(ceil(y-.5),floor(x-1.5));
    const WPP::RGB & rgb2d = img.at(ceil(y-.5),ceil(x-1.5));
    const WPP::RGB & rgb3a = img.at(floor(y-1.5),floor(x+.5));
    const WPP::RGB & rgb3b = img.at(floor(y-1.5),ceil(x+.5));
    const WPP::RGB & rgb3c = img.at(ceil(y-1.5),floor(x+.5));
    const WPP::RGB & rgb3d = img.at(ceil(y-1.5),ceil(x+.5));
    const WPP::RGB & rgb4a = img.at(floor(y+1),floor(x+.5));
    const WPP::RGB & rgb4b = img.at(floor(y+1),ceil(x+.5));
    const WPP::RGB & rgb4c = img.at(ceil(y+1),floor(x+.5));
    const WPP::RGB & rgb4d = img.at(ceil(y+1),ceil(x+.5));
    const WPP::RGB & rgb5a = img.at(floor(y+.5),floor(x+1.5));
    const WPP::RGB & rgb5b = img.at(floor(y+.5),ceil(x+1.5));
    const WPP::RGB & rgb5c = img.at(ceil(y+.5),floor(x+1.5));
    const WPP::RGB & rgb5d = img.at(ceil(y+.5),ceil(x+1.5));
    const WPP::RGB & rgb6a = img.at(floor(y+1.5),floor(x-.5));
    const WPP::RGB & rgb6b = img.at(floor(y+1.5),ceil(x-.5));
    const WPP::RGB & rgb6c = img.at(ceil(y+1.5),floor(x-.5));
    const WPP::RGB & rgb6d = img.at(ceil(y+1.5),ceil(x-.5));

    return
        2*((int)rgb4a[0] - (int)rgb1a[0] +
        (int)rgb4a[1] - (int)rgb1a[1] +
        (int)rgb4a[2] - (int)rgb1a[2]) +
        (int)rgb5a[0] - (int)rgb3a[0] +
        (int)rgb5a[1] - (int)rgb3a[1] +
        (int)rgb5a[2] - (int)rgb3a[2] +
        (int)rgb6a[0] - (int)rgb2a[0] +
        (int)rgb6a[1] - (int)rgb2a[1] +
        (int)rgb6a[2] - (int)rgb2a[2] +
        2*((int)rgb4b[0] - (int)rgb1b[0] +
        (int)rgb4b[1] - (int)rgb1b[1] +
        (int)rgb4b[2] - (int)rgb1b[2]) +
        (int)rgb5b[0] - (int)rgb3b[0] +
        (int)rgb5b[1] - (int)rgb3b[1] +
        (int)rgb5b[2] - (int)rgb3b[2] +
        (int)rgb6b[0] - (int)rgb2b[0] +
        (int)rgb6b[1] - (int)rgb2b[1] +
        (int)rgb6b[2] - (int)rgb2b[2] +
        2*((int)rgb4c[0] - (int)rgb1c[0] +
        (int)rgb4c[1] - (int)rgb1c[1] +
        (int)rgb4c[2] - (int)rgb1c[2]) +
        (int)rgb5c[0] - (int)rgb3c[0] +
        (int)rgb5c[1] - (int)rgb3c[1] +
        (int)rgb5c[2] - (int)rgb3c[2] +
        (int)rgb6c[0] - (int)rgb2c[0] +
        (int)rgb6c[1] - (int)rgb2c[1] +
        (int)rgb6c[2] - (int)rgb2c[2] +
        2*((int)rgb4d[0] - (int)rgb1d[0] +
        (int)rgb4d[1] - (int)rgb1d[1] +
        (int)rgb4d[2] - (int)rgb1d[2]) +
        (int)rgb5d[0] - (int)rgb3d[0] +
        (int)rgb5d[1] - (int)rgb3d[1] +
        (int)rgb5d[2] - (int)rgb3d[2] +
        (int)rgb6d[0] - (int)rgb2d[0] +
        (int)rgb6d[1] - (int)rgb2d[1] +
        (int)rgb6d[2] - (int)rgb2d[2];
}

//Returns a smooth (bilinearly interpolated) tilted (arctan(.5)) sobel filter result for a left-handed person
inline int smooth_sobel_lh(const WPP::Matrix<WPP::RGB> & img, double y, double x)
{
    const WPP::RGB & rgb1a = img.at(floor(y-1),floor(x+.5));
    const WPP::RGB & rgb1b = img.at(floor(y-1),ceil(x+.5));
    const WPP::RGB & rgb1c = img.at(ceil(y-1),floor(x+.5));
    const WPP::RGB & rgb1d = img.at(ceil(y-1),ceil(x+.5));
    const WPP::RGB & rgb2a = img.at(floor(y-.5),floor(x+1.5));
    const WPP::RGB & rgb2b = img.at(floor(y-.5),ceil(x+1.5));
    const WPP::RGB & rgb2c = img.at(ceil(y-.5),floor(x+1.5));
    const WPP::RGB & rgb2d = img.at(ceil(y-.5),ceil(x+1.5));
    const WPP::RGB & rgb3a = img.at(floor(y-1.5),floor(x-.5));
    const WPP::RGB & rgb3b = img.at(floor(y-1.5),ceil(x-.5));
    const WPP::RGB & rgb3c = img.at(ceil(y-1.5),floor(x-.5));
    const WPP::RGB & rgb3d = img.at(ceil(y-1.5),ceil(x-.5));
    const WPP::RGB & rgb4a = img.at(floor(y+1),floor(x-.5));
    const WPP::RGB & rgb4b = img.at(floor(y+1),ceil(x-.5));
    const WPP::RGB & rgb4c = img.at(ceil(y+1),floor(x-.5));
    const WPP::RGB & rgb4d = img.at(ceil(y+1),ceil(x-.5));
    const WPP::RGB & rgb5a = img.at(floor(y+.5),floor(x-1.5));
    const WPP::RGB & rgb5b = img.at(floor(y+.5),ceil(x-1.5));
    const WPP::RGB & rgb5c = img.at(ceil(y+.5),floor(x-1.5));
    const WPP::RGB & rgb5d = img.at(ceil(y+.5),ceil(x-1.5));
    const WPP::RGB & rgb6a = img.at(floor(y+1.5),floor(x+.5));
    const WPP::RGB & rgb6b = img.at(floor(y+1.5),ceil(x+.5));
    const WPP::RGB & rgb6c = img.at(ceil(y+1.5),floor(x+.5));
    const WPP::RGB & rgb6d = img.at(ceil(y+1.5),ceil(x+.5));

    return
        2*((int)rgb4a[0] - (int)rgb1a[0] +
        (int)rgb4a[1] - (int)rgb1a[1] +
        (int)rgb4a[2] - (int)rgb1a[2]) +
        (int)rgb5a[0] - (int)rgb3a[0] +
        (int)rgb5a[1] - (int)rgb3a[1] +
        (int)rgb5a[2] - (int)rgb3a[2] +
        (int)rgb6a[0] - (int)rgb2a[0] +
        (int)rgb6a[1] - (int)rgb2a[1] +
        (int)rgb6a[2] - (int)rgb2a[2] +
        2*((int)rgb4b[0] - (int)rgb1b[0] +
        (int)rgb4b[1] - (int)rgb1b[1] +
        (int)rgb4b[2] - (int)rgb1b[2]) +
        (int)rgb5b[0] - (int)rgb3b[0] +
        (int)rgb5b[1] - (int)rgb3b[1] +
        (int)rgb5b[2] - (int)rgb3b[2] +
        (int)rgb6b[0] - (int)rgb2b[0] +
        (int)rgb6b[1] - (int)rgb2b[1] +
        (int)rgb6b[2] - (int)rgb2b[2] +
        2*((int)rgb4c[0] - (int)rgb1c[0] +
        (int)rgb4c[1] - (int)rgb1c[1] +
        (int)rgb4c[2] - (int)rgb1c[2]) +
        (int)rgb5c[0] - (int)rgb3c[0] +
        (int)rgb5c[1] - (int)rgb3c[1] +
        (int)rgb5c[2] - (int)rgb3c[2] +
        (int)rgb6c[0] - (int)rgb2c[0] +
        (int)rgb6c[1] - (int)rgb2c[1] +
        (int)rgb6c[2] - (int)rgb2c[2] +
        2*((int)rgb4d[0] - (int)rgb1d[0] +
        (int)rgb4d[1] - (int)rgb1d[1] +
        (int)rgb4d[2] - (int)rgb1d[2]) +
        (int)rgb5d[0] - (int)rgb3d[0] +
        (int)rgb5d[1] - (int)rgb3d[1] +
        (int)rgb5d[2] - (int)rgb3d[2] +
        (int)rgb6d[0] - (int)rgb2d[0] +
        (int)rgb6d[1] - (int)rgb2d[1] +
        (int)rgb6d[2] - (int)rgb2d[2];
}

// Taylor-series approximation of sinc(pi * x/k), where sinc(z) = sin(z)/z
inline double sinc(double x, int k)
{
    if(x==0)
        return 1;
    else
    {
        double u = M_PI * x / k;
        //return sin(u)/u;
        double u2 = u*u;
        return 1-u2/6*(1-u2/20);
    }
}

//lanczos filter
/*inline double lanczos(double x, int k1, int k2)
{
    return sinc(x, k1) * sinc(x, k2);
}*/

void pentrack(
        double img_norm,
        const Rectification & crosses,
        const WPP::Matrix<WPP::RGB> & img,
        WPP::Matrix<WPP::RGB> & view,
        bool flefthanded,
        double & xtip,
        double & ytip)
{
    int pbth = penblue_threshold(img_norm);

    xtip=0;
    ytip=-1;

    //STEP 1. BLUE SEARCH
    {
        WPPASSERT(crosses.size()==4)

        //Bounding box
        int ymin = img.rows-1;
        int ymax = 0;
        for(int i = 0; i < 4; i++)
        {
            ymax = ((crosses[i].y > ymax)?crosses[i].y:ymax);
            ymin = ((crosses[i].y < ymin)?crosses[i].y:ymin);
        }

        {
            int yend = ymin - SHADOW_MAX_H;
            if(yend < 4)
                yend = 4;

            int xpara = 0; //xpara further delimitates search domain since we are minimizing 2*y ± x. xpara is a minimum if right-handed and a maximum otherwise.
            if(flefthanded)
                xpara = img.rows;
            for(int y = ymax; y >= yend; y-=3) //To save time, we only analyse 1 of every 3 lines
            {
                int xcount = 0;
                if(ytip!=-1)
                {
                    if(flefthanded)
                        xpara -= 6;
                    else
                        xpara += 6;
                }


                //       ________
                //     /|        |\
                //    / |________| \
                //   /_/__________\_\
                //  | /            \ |
                //  |/______________\|
                //
                //Compute xmax and xmin of the intersection of the following 12 lines: the 4 lines linking the 4 crosses, these 4 lines raised SHADOW_MAX_H each, and the 4 vertical lines linking each cross to the correspondent raised line.

                int xmax = 0;        //start with an invalid set
                int xmin = img.cols;
                for(int i = 0; i < 4; i++) //for cross pairs (0,1), (1,2), (2,3) and (3,0)
                {
                    int j = (i+1)%4;
                    int x;

                    //Line in rectification
                    if(crosses[i].y < crosses[j].y)
                    {
                        if(y >= crosses[i].y && y <= crosses[j].y)
                        {
                            x = (y*(crosses[i].x - crosses[j].x) - (crosses[i].x * crosses[j].y - crosses[j].x*crosses[i].y))/(crosses[i].y - crosses[j].y);
                            if(x>xmax)xmax=x;
                            if(x<xmin)xmin=x;
                        }
                    }
                    else if(crosses[i].y > crosses[j].y)
                    {
                        if(y <= crosses[i].y && y >= crosses[j].y)
                        {
                            x = (y*(crosses[i].x - crosses[j].x) - (crosses[i].x * crosses[j].y - crosses[j].x*crosses[i].y))/(crosses[i].y - crosses[j].y);
                            if(x>xmax)xmax=x;
                            if(x<xmin)xmin=x;
                        }
                    }
                    else
                    {
                        if(y == crosses[i].y)
                        {
                            if(crosses[i].x>xmax)xmax=x;
                            if(crosses[i].x<xmin)xmin=x;
                            if(crosses[j].x>xmax)xmax=x;
                            if(crosses[j].x<xmin)xmin=x;
                        }
                    }

                    //Line in rectification, raised SHADOW_MAX_H
                    if(crosses[i].y < crosses[j].y)
                    {
                        if(y >= crosses[i].y - SHADOW_MAX_H && y <= crosses[j].y - SHADOW_MAX_H)
                        {
                            x = (y*(crosses[i].x - crosses[j].x) - (crosses[i].x * (crosses[j].y-SHADOW_MAX_H) - crosses[j].x*(crosses[i].y-SHADOW_MAX_H)))/(crosses[i].y - crosses[j].y);
                            if(x>xmax)xmax=x;
                            if(x<xmin)xmin=x;
                        }
                    }
                    else if(crosses[i].y > crosses[j].y)
                    {
                        if(y <= crosses[i].y - SHADOW_MAX_H && y >= crosses[j].y - SHADOW_MAX_H)
                        {
                            x = (y*(crosses[i].x - crosses[j].x) - (crosses[i].x * (crosses[j].y-SHADOW_MAX_H) - crosses[j].x*(crosses[i].y-SHADOW_MAX_H)))/(crosses[i].y - crosses[j].y);
                            if(x>xmax)xmax=x;
                            if(x<xmin)xmin=x;
                        }
                    }
                    else
                    {
                        if(y == crosses[i].y - SHADOW_MAX_H)
                        {
                            if(crosses[i].x>xmax)xmax=x;
                            if(crosses[i].x<xmin)xmin=x;
                            if(crosses[j].x>xmax)xmax=x;
                            if(crosses[j].x<xmin)xmin=x;
                        }
                    }

                    //vertical line
                    if(y <= crosses[i].y && y >= crosses[i].y - SHADOW_MAX_H)
                    {
                        x = crosses[i].x;
                        if(x>xmax)xmax=x;
                        if(x<xmin)xmin=x;
                    }
                }

                if(xmin<4)
                    xmin=4;

                if(flefthanded)
                {
                    //apply xpara restriction
                    int xend = xmax;
                    if(xend>xpara)
                        xend = xpara;

                    const WPP::RGB * rgb = &img.at(y,xmin);
                    for(int x = xmin; x < xend; x++)
                    {
                        /*if(penblue(pbth, *rgb)
                            && penblue(pbth, img.at(y-1,x-1))
                            && penblue(pbth, img.at(y-2,x-2))
                            && penblue(pbth, img.at(y-3,x-3))
                            && penblue(pbth, img.at(y-4,x-4))
                            && penblue(pbth, img.at(y-5,x-5))
                            && penblue(pbth, img.at(y-6,x-6))
                            && penblue(pbth, img.at(y-7,x-7))
                            && penblue(pbth, img.at(y-8,x-8)))*/
                        if(penblue(pbth, *rgb)
                            && penblue(pbth, img.at(y-8,x+8))
                            && penblue(pbth, img.at(y-4,x+4))
                            && penblue(pbth, img.at(y-6,x+6))
                            && penblue(pbth, img.at(y-2,x+2))
                            && penblue(pbth, img.at(y-7,x+7))
                            && penblue(pbth, img.at(y-5,x+5))
                            && penblue(pbth, img.at(y-3,x+3))
                            && penblue(pbth, img.at(y-1,x+1)))
                        {
                            ytip = y;
                            xpara = xtip = x;
                            break;
                        }
                        rgb++;
                    }
                }
                else
                {
                    //apply xpara restriction
                    int xbeg = xmin;
                    if(xbeg<xpara)
                        xbeg = xpara;

                    const WPP::RGB * rgb = &img.at(y,xbeg);
                    for(int x = xbeg; x < xmax; x++)
                    {
                        if(penblue(pbth, *rgb)
                            && penblue(pbth, img.at(y-8,x-8))
                            && penblue(pbth, img.at(y-4,x-4))
                            && penblue(pbth, img.at(y-6,x-6))
                            && penblue(pbth, img.at(y-2,x-2))
                            && penblue(pbth, img.at(y-7,x-7))
                            && penblue(pbth, img.at(y-5,x-5))
                            && penblue(pbth, img.at(y-3,x-3))
                            && penblue(pbth, img.at(y-1,x-1)))
                        {
                            ytip = y;
                            xpara = xtip = x;
                        }
                        rgb++;
                    }
                }
            }
        }


        if(ytip==-1 || ytip > ymax)
        {
            xtip = -1;
            ytip = -1;
            return;
        }

    }


    {
        //STEP 2. COLUMN SUM
        //for(int it = 0; it < 2; it++)
        {
            int sum[15];
            int ssum = sizeof(sum)/sizeof(sum[0]);

            //safety check
            if(
                    xtip - ssum/2 -3 < 0 ||
                    xtip + ssum - ssum/2 + 7 > img.cols ||
                    ytip - 4 - floor((ssum-ssum/2)/2.) < 0 ||
                    ytip + 12 + ceil((ssum/2)/2.) > img.rows)
            {
                ytip = -1;
                xtip = -1;
                return;
            }

            //sum within each tilted column
            for(int i = 0; i < ssum; i++)
            {
                sum[i] = 0;
                double oi = (i-ssum/2)/2.;
                if(flefthanded)
                {
                    for(int j = -2; j < 10; j++)
                    {
                        double x = xtip+i-ssum/2-j/2.;
                        double y = ytip+j+oi;
                        sum[i] +=
                                WPP::SUM(img.at(floor(y),floor(x))) +
                                WPP::SUM(img.at(floor(y),ceil(x))) +
                                WPP::SUM(img.at(ceil(y),floor(x))) +
                                WPP::SUM(img.at(ceil(y),ceil(x)));
                    }
                }
                else
                {
                    for(int j = -2; j < 10; j++)
                    {
                        double x = xtip+i-ssum/2+j/2.;
                        double y = ytip+j-oi;
                        sum[i] +=
                                WPP::SUM(img.at(floor(y),floor(x))) +
                                WPP::SUM(img.at(floor(y),ceil(x))) +
                                WPP::SUM(img.at(ceil(y),floor(x))) +
                                WPP::SUM(img.at(ceil(y),ceil(x)));
                    }
                }
            }

            //select column that minimizes sum (after [1 2 1] convolution)
            double min_s=1e20;
            int min_i=ssum/2;
            for(int i = 1; i < ssum-1; i++)
            {
                double v = sum[i-1]+2*sum[i]+sum[i+1];
                if(v < min_s)
                {
                    min_s = v;
                    min_i = i;
                }
            }

            //maximize sobel within column
            double maxso = 0;
            int candj = 0;
            int candi = min_i-ssum/2;
            if(flefthanded)
            {
                for(int j = -2; j < 10; j++)
                {
                    double x = xtip+candi-j/2.;
                    double y = ytip+candi/2.+j;

                    int so = smooth_sobel_lh(img, y, x);
                    if(so > maxso)
                    {
                         candj = j;
                         maxso = so;
                    }
                }
                ytip = (int)(ytip+candj+candi/2);
                xtip = (int)(xtip-candj/2+candi);
            }
            else
            {
                for(int j = -2; j < 10; j++)
                {
                    double x = xtip+candi+j/2.;
                    double y = ytip-candi/2.+j;

                    int so = smooth_sobel_rh(img, y, x);
                    if(so > maxso)
                    {
                         candj = j;
                         maxso = so;
                    }
                }
                ytip = (int)(ytip+candj-candi/2);
                xtip = (int)(xtip+candj/2+candi);
            }


            //STEP 3. OBJECTIVE FUNCTION

            #define LANCD 12
            double lancvec[2*LANCD-1]; //precomputed sinc² filter
            for(int i = 0; i < 2*LANCD-1; i++)
                lancvec[i] = sqr(sinc(i-LANCD+1,LANCD));
            double objfun[9]; //surrounding pixels + center pixel
            bool hasval[9]; //true if corresponding objfun has already been calculated
            for(int option=0; option<=8; option++)
                hasval[option] = false;
            int xtip0 = xtip; //reference value (just to make objfun values have a reasonable scale)
            int ytip0 = ytip;
            for(int it = 0; it < 10; it++)
            {
                int xcand = xtip; //surrounding candidate (search direction)
                int ycand = ytip;
                double objfuncand = 0;
                int optcand = 0; // \in {0,...,8}

                if(xtip-LANCD < 0 || ytip-LANCD-1 < 0 || xtip+LANCD+1 > img.cols || ytip+LANCD+2 > img.rows)
                {
                    ytip = -1;
                    xtip = -1;
                    return;
                }

                //Fill objfun vector
                for(int option=0; option<=8; option++)
                {
                    int x = xtip+(option==1)-(option==2)-(option==5)+(option==6)-(option==7)+(option==8);
                    int y = ytip+(option==3)-(option==4)-(option==5)-(option==6)+(option==7)+(option==8);
                    if(!hasval[option])
                    {
                        objfun[option] = 0;
                        double dB = 0;
                        double dR = 0;
                        double dG = 0;
                        for(int xl = x-LANCD+1; xl < x+LANCD; xl++)
                            for(int yl = y-LANCD+1; yl < y+LANCD; yl++)
                            {
                                double f = lancvec[xl-(x-LANCD+1)] * lancvec[yl-(y-LANCD+1)];
                                dB += f * ((int)img.at(yl+1,xl)[0] - (int)img.at(yl-1,xl)[0]);
                                dG += f * ((int)img.at(yl+1,xl)[1] - (int)img.at(yl-1,xl)[1]);
                                dR += f * ((int)img.at(yl+1,xl)[2] - (int)img.at(yl-1,xl)[2]);
                            }
                        objfun[option] = exp(((y-ytip0)+(flefthanded?-2:2)*(x-xtip0))/25.) * (3*(dR+dG)-dB);
                    }
                    if(objfun[option]>objfuncand)
                    {
                        optcand = option;
                        objfuncand = objfun[option];
                        xcand = x;
                        ycand = y;
                    }
                }

                //Keep measurements from previous iteration
                switch(optcand)
                {
                case 0:
                    {
                        //STEP 4. MAXIMIZE WITH SUBPIXEL PRECISION

                        //objfun = ax²+bxy+cy²+dx+ey+f

                        //    [ 1 -2  1]
                        //a = [ 1 -2  1]/6
                        //    [ 1 -2  1]

                        //    [ 1  0 -1]
                        //b = [ 0  0  0]/4
                        //    [-1  0  1]

                        //    [ 1  1  1]
                        //c = [-2 -2 -2]/6
                        //    [ 1  1  1]

                        //    [-1  0  1]
                        //d = [-1  0  1]/6
                        //    [-1  0  1]

                        //    [-1 -1 -1]
                        //e = [ 0  0  0]/6
                        //    [ 1  1  1]

                        //    [-1  2 -1]
                        //f = [ 2  5  2]/9
                        //    [-1  2 -1]

                        //objfun indexation:
                        //[5 4 6]
                        //[2 0 1]
                        //[7 3 8]
                        //

                        double a = (-2*(objfun[0]+objfun[3]+objfun[4])+objfun[1]+objfun[6]+objfun[8]+objfun[2]+objfun[5]+objfun[7])/6.;
                        double b = (objfun[5]-objfun[6]-objfun[7]+objfun[8])/4.;
                        double c = (-2*(objfun[0]+objfun[1]+objfun[2])+objfun[3]+objfun[6]+objfun[8]+objfun[4]+objfun[5]+objfun[7])/6.;
                        double d = (objfun[1]+objfun[6]+objfun[8]-objfun[2]-objfun[5]-objfun[7])/6.;
                        double e = (objfun[7]+objfun[3]+objfun[8]-objfun[5]-objfun[4]-objfun[6])/6.;
                        double f = (5*objfun[0]+2*(objfun[1]+objfun[2]+objfun[3]+objfun[4])-(objfun[5]+objfun[6]+objfun[7]+objfun[8]))/9.;
                        //[a  b/2][c  -b/2] = [1 0]*(ac-b²/4)
                        //[b/2  c][-b/2  a]   [0 1]

                        //[x] = -[c  -b/2][d]/(2ac-b²/2)
                        //[y]    [-b/2  a][e]
                        double dx = (c*d-b*e/2)/(2*a*c-b*b/2);
                        double dy = (a*e-b*d/2)/(2*a*c-b*b/2);
                        if(dx > .5) dx = .5;
                        if(dy > .5) dy = .5;
                        if(dx < -.5) dx = -.5;
                        if(dy < -.5) dy = -.5;
                        xtip -= dx;
                        ytip -= dy;

                        //Checking for NaN, just in case
                        if(!(ytip==ytip) || !(xtip==xtip))
                        {
                            ytip = -1;
                            xtip = -1;
                            return;
                        }
                    }
                    break;
                case 1:
                    hasval[5]=true; hasval[4]=true; hasval[6]=false;
                    hasval[2]=true; hasval[0]=true; hasval[1]=false;
                    hasval[7]=true; hasval[3]=true; hasval[8]=false;

                    objfun[5]=objfun[4]; objfun[4]=objfun[6];
                    objfun[2]=objfun[0]; objfun[0]=objfun[1];
                    objfun[7]=objfun[3]; objfun[3]=objfun[8];
                    break;
                case 2:
                    hasval[5]=false; hasval[4]=true; hasval[6]=true;
                    hasval[2]=false; hasval[0]=true; hasval[1]=true;
                    hasval[7]=false; hasval[3]=true; hasval[8]=true;

                    objfun[6]=objfun[4]; objfun[4]=objfun[5];
                    objfun[1]=objfun[0]; objfun[0]=objfun[2];
                    objfun[8]=objfun[3]; objfun[3]=objfun[7];
                    break;
                case 3:
                    hasval[5]=true; hasval[4]=true; hasval[6]=true;
                    hasval[2]=true; hasval[0]=true; hasval[1]=true;
                    hasval[7]=false; hasval[3]=false; hasval[8]=false;

                    objfun[5]=objfun[2]; objfun[4]=objfun[0]; objfun[6]=objfun[1];
                    objfun[2]=objfun[7]; objfun[0]=objfun[3]; objfun[1]=objfun[8];
                    break;
                case 4:
                    hasval[5]=false; hasval[4]=false; hasval[6]=false;
                    hasval[2]=true; hasval[0]=true; hasval[1]=true;
                    hasval[7]=true; hasval[3]=true; hasval[8]=true;

                    objfun[7]=objfun[2]; objfun[3]=objfun[0]; objfun[8]=objfun[1];
                    objfun[2]=objfun[5]; objfun[0]=objfun[4]; objfun[1]=objfun[6];
                    break;
                case 5:
                    hasval[5]=false; hasval[4]=false; hasval[6]=false;
                    hasval[2]=false; hasval[0]=true; hasval[1]=true;
                    hasval[7]=false; hasval[3]=true; hasval[8]=true;

                    objfun[3]=objfun[2]; objfun[8]=objfun[0];
                    objfun[0]=objfun[5]; objfun[1]=objfun[4];
                    break;
                case 6:
                    hasval[5]=false; hasval[4]=false; hasval[6]=false;
                    hasval[2]=true; hasval[0]=true; hasval[1]=false;
                    hasval[7]=true; hasval[3]=true; hasval[8]=false;

                    objfun[7]=objfun[0]; objfun[3]=objfun[1];
                    objfun[2]=objfun[4]; objfun[0]=objfun[6];
                    break;
                case 7:
                    hasval[5]=false; hasval[4]=true; hasval[6]=true;
                    hasval[2]=false; hasval[0]=true; hasval[1]=true;
                    hasval[7]=false; hasval[3]=false; hasval[8]=false;

                    objfun[4]=objfun[2]; objfun[6]=objfun[0];
                    objfun[0]=objfun[7]; objfun[1]=objfun[3];
                    break;
                case 8:
                    hasval[5]=true; hasval[4]=true; hasval[6]=false;
                    hasval[2]=true; hasval[0]=true; hasval[1]=false;
                    hasval[7]=false; hasval[3]=false; hasval[8]=false;

                    objfun[4]=objfun[1]; objfun[5]=objfun[0];
                    objfun[0]=objfun[8]; objfun[2]=objfun[3];
                    break;
                }

                if(optcand==0)
                    break;

                xtip = xcand;
                ytip = ycand;
            }
        }
    }
}
