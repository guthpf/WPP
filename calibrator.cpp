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

#include "calibrator.h"
#include <fstream>
#include <vector>
using namespace std;

namespace WPP
{
    //Applies size x size square dilation operator. Complexity: O(width*height*size).
    Matrix<bool> dilate(const Matrix<bool> & m, int size)
    {
        WPPASSERT(size>0 && size%2==1);
        int hs = (size-1)/2;
        Matrix<bool> tmp = Matrix<bool>::zeros(m.rows, m.cols);
        for(int i = 0; i < m.rows; i++)
        {
            int iimin = i-hs;
            if(iimin<0)iimin=0;
            int iimax = i+hs;
            if(iimax>m.rows-1)iimax=m.rows-1;
            for(int j = 0; j < m.cols; j++)
                if(m.at(i,j))
                    for(int ii = iimin; ii <= iimax; ii++)
                        tmp.at(ii,j) = true;
        }
        Matrix<bool> ret = Matrix<bool>::zeros(m.rows, m.cols);
        for(int i = 0; i < m.rows; i++)
            for(int j = 0; j < m.cols; j++)
                if(tmp.at(i,j))
                {
                    int jjmin = j-hs;
                    if(jjmin<0)jjmin=0;
                    int jjmax = j+hs;
                    if(jjmax>m.cols-1)jjmax=m.cols-1;
                    for(int jj = jjmin; jj <= jjmax; jj++)
                        ret.at(i,jj) = true;
                }
        return ret;
    }

    //Applies border-normalized, size x size gaussian operator. Complexity: O(width*height*size).
    Matrix<short> gaussian_blur(const Matrix<short> & m, int size, double sigma)
    {
        WPPASSERT(size>0 && size%2==1 && sigma > 0)
        int hs = size/2;

        double * gauss = new double[size];
        for(int i = 0; i < size; i++)
            gauss[i] = exp(-(i-hs)*(i-hs)/2./sigma/sigma);

        Matrix<double> mid = Matrix<double>::zeros(m.rows, m.cols);
        for(int i = 0; i < m.rows; i++)
            for(int j = 0; j < m.cols; j++)
            {
                double mass = 0;
                int jjmin = j-hs;
                if(jjmin<0)jjmin=0;
                int jjmax = j+hs;
                if(jjmax>m.cols-1)jjmax=m.cols-1;
                for(int jj = jjmin; jj <= jjmax; jj++)
                {
                    mid.at(i,j) += gauss[jj-j+hs] * m.at(i,jj);
                    mass += gauss[jj-j+hs];
                }
                mid.at(i,j) /= mass;
            }

        Matrix<short> ret(m.rows, m.cols);

        for(int i = 0; i < m.rows; i++)
            for(int j = 0; j < m.cols; j++)
            {
                double sum = 0;
                double mass = 0;
                int iimin = i-hs;
                if(iimin<0)iimin=0;
                int iimax = i+hs;
                if(iimax>m.rows-1)iimax=m.rows-1;
                for(int ii = iimin; ii <= iimax; ii++)
                {
                    sum += gauss[ii-i+hs] * mid.at(ii,j);
                    mass += gauss[ii-i+hs];
                }
                ret.at(i,j) = sum/mass;
            }

        delete[] gauss;

        return ret;
    }

    //applies 3x3 d/dx sobel operator
    Matrix<short> sobel_3_x(const Matrix<short> & m)
    {
        Matrix<short> ret = Matrix<short>::zeros(m.rows, m.cols);
        for(int i = 1; i < m.rows-1; i++)
            for(int j = 1; j < m.cols-1; j++)
                ret.at(i,j) =
                        m.at(i+1,j+1) - m.at(i+1,j-1) +
                        2*(m.at(i,j+1) - m.at(i,j-1)) +
                        m.at(i-1,j+1) - m.at(i-1,j-1);
        return ret;
    }

    //applies 3x3 d/dy sobel operator
    Matrix<short> sobel_3_y(const Matrix<short> & m)
    {
        Matrix<short> ret = Matrix<short>::zeros(m.rows, m.cols);
        for(int i = 1; i < m.rows-1; i++)
            for(int j = 1; j < m.cols-1; j++)
                ret.at(i,j) =
                        m.at(i+1,j+1) - m.at(i-1,j+1) +
                        2*(m.at(i+1,j) - m.at(i-1,j)) +
                        m.at(i+1,j-1) - m.at(i-1,j-1);
        return ret;
    }
}

//A row of pixels
//Used by scanline algorithms
struct PixSeq
{
    vector<PixSeq>* cc;

    //    xmin        xmax
    //     |           |
    //     v           v
    // . . # # # # # # . . .

    int y;
    int xmin;
    int xmax;

    PixSeq(int xmin_, int xmax_, int y_, vector<PixSeq>* cc_)
    {
        xmin = xmin_;
        xmax = xmax_;
        y = y_;
        cc = cc_;
    }
};

//from a matrix m, finds connected components (ccs) where col > 0
//caution: ccs may contain some empty components
void scanline_raw(const WPP::Matrix<bool> & m, vector<vector<PixSeq>* > & ccs)
{
    vector<PixSeq> upl;
    for(int y = 0; y < m.rows; y++)
    {
        vector<PixSeq> cur;
        bool l = false;
        int xmin, xmax;
        vector<PixSeq>::iterator it = upl.begin();
        vector<PixSeq>::iterator pit = it;
        for(int x = 0; x <= m.cols; x++)
        {
            bool c = (x==m.cols)?0:(m.at(y,x));
            if(!l && c)
                xmin = x;
            else if(l && !c)
            {
                xmax = x;
                cur.push_back(PixSeq(xmin, xmax, y, NULL));
                while(it != upl.end() && it->xmin < xmax)
                {
                    if(it->xmax > xmin)
                    {
                        if(cur.back().cc == NULL)
                            cur.back().cc = it->cc;
                        else if(cur.back().cc != it->cc)
                        {

                            if(it->cc->size() < cur.back().cc->size())
                            {
                                //merge RIGHT
                                while(!it->cc->empty())
                                {
                                    cur.back().cc->push_back(it->cc->back());
                                    it->cc->pop_back();
                                }
                                for(vector<PixSeq>::iterator u = it+1; u != upl.end(); u++)
                                    if(u->cc == it->cc)
                                        u->cc = cur.back().cc;
                                it->cc = cur.back().cc;
                            }
                            else
                            {
                                //merge LEFT
                                while(!cur.back().cc->empty())
                                {
                                    it->cc->push_back(cur.back().cc->back());
                                    cur.back().cc->pop_back();
                                }
                                for(vector<PixSeq>::iterator u = cur.begin(); u != cur.end(); u++)
                                    if(u->cc == cur.back().cc)
                                        u->cc = it->cc;
                            }
                        }
                    }
                    pit = it;
                    it++;
                }
                it = pit;
                if(cur.back().cc == NULL)
                {
                    ccs.push_back(new vector<PixSeq>());
                    cur.back().cc = ccs.back();
                }
            }
            l = c;
        }
        upl = cur;
        for(vector<PixSeq>::iterator u = upl.begin(); u != upl.end(); u++)
            u->cc->push_back(*u);
    }
}

//Finds white connected components in binary image m (type <char>, white <=> non-zero) using scanline method and returns "cross" components (must match some criteria)
//sob2: squared gradient norm obtained from (unnormalized) 3x3 sobel filters
WPP::Matrix<char> scanlineCC(double img_norm, const WPP::Matrix<bool> & m, WPP::Matrix<unsigned short> sob2, int & ccon)
{
    const int pixnummin = 50; //minimum number of pixels component must have in order to be accepted
    const int pixnummax = 2000; //maximum number of pixels component must have in order to be accepted
    const int soblim = .25e6 * img_norm * img_norm; //minimum sum of sob2 component must have in order to be accepted

    WPP::Matrix<char> ret;

    //SCANLINE
    vector<vector<PixSeq>* > ccs;
    scanline_raw(m, ccs);

    ret = WPP::Matrix<char>::zeros(m.rows, m.cols);
    int count = 0;
    for(vector<vector<PixSeq>* >::iterator u = ccs.begin(); u != ccs.end(); u++)
    {
        if(!(*u)->empty())
        {
            //CHECK CONDITIONS
            count++;
            int pixcount = 0;
            double sobelcount = 0;
            bool success = true;
            for(vector<PixSeq>::iterator v = (*u)->begin(); v != (*u)->end(); v++)
            {
                if(v->xmin == 0 || v->xmax == m.cols || v->y == 0 || v->y == m.rows-1)
                {
                    success = false;
                    break;
                }
                pixcount += v->xmax - v->xmin;
                if(pixcount >= pixnummax)
                {
                    success = false;
                    break;
                }
                for(int x = v->xmin; x < v->xmax; x++)
                    sobelcount += sob2.at(v->y,x);
            }
            if(sobelcount <= soblim)
                success = false;
            if(pixcount <= pixnummin)
                success = false;
            if(!success)
            {
                count--;
                continue;
            }

            //copy to ret
            for(vector<PixSeq>::iterator v = (*u)->begin(); v != (*u)->end(); v++)
                for(int x = v->xmin; x < v->xmax; x++)
                    ret.at(v->y, x) = count;
        }
        delete *u;
    }
    ccon = count;
    return ret;
}

//draws a line linking points v1 and v2 on img
//use the to_color function to choose the line color (wpppoint.h)
void drawline(
        const WPP::Point & v1,
        const WPP::Point & v2,
        WPP::Matrix<WPP::RGB> & img,
        void (*to_color)(WPP::RGB &)
        )
{
    if(v1.x>=0 && v1.x<img.cols && v2.x>=0 && v2.x<img.cols && v1.y>=0 && v1.y<img.rows && v2.y>=0 && v2.y<img.rows) //-> TODO: change back to ASSERT()?
    {
        int len = sqrt(pow(v1.x-v2.x,2)+pow(v1.y-v2.y,2));
        double dx = (v2.x-v1.x)/len;
        double dy = (v2.y-v1.y)/len;
        for(int j = 0; j < len; j++)
            to_color(img.at(v1.y + dy*j, v1.x + dx*j));
    }
}

void Calibrator::drawCC(WPP::Matrix<WPP::RGB> & view)
{
    for(int i = 0; i < view.rows; i++)
        for(int j = 0; j < view.cols; j++)
            if(cc_enumeration_.at(i,j) > 0)
                darken_light_blue(view.at(i,j));
            else if(nonpaper_.at(i,j))
                darken_red(view.at(i,j));
}

void Calibrator::drawRectification(WPP::Matrix<WPP::RGB> & view)
{
    WPPASSERT(NCROSSESCALIB==4)
    for(int i =  0; i < 4; i++)
        drawline( //TODO asserion error found
            crossescand_[i],
            crossescand_[(i+1)%4],
            view, to_orange);
}

bool Calibrator::findCrosses(
    double img_norm,
    const WPP::Matrix<WPP::RGB> & img,
    WPP::Matrix<WPP::RGB> & view)
{
    if(defined_)
        return waiting_acceptance_;

    //Fit Ax + By + C + Dx² + Ey² + Fxy
    double paperwhiteD=0;
    double paperwhiteE=0;
    double paperwhiteF=0;
    double paperwhiteA=0;
    double paperwhiteB=0;
    double paperwhiteC;

    //compute sum of the 3 color channels (sumi)
    WPP::Matrix<short> sumi(img.rows, img.cols);
    for(int y = 0; y < img.rows; y++)
        for(int x = 0; x < img.cols; x++)
            sumi.at(y,x) = WPP::SUM(img.at(y,x));


    //STEP 1. INITIAL GUESS FOR PAPERWHITEC: FROM HIERARCHICAL SEARCH
    {
        const double SIGMA2 = 200000; //stop criterion parameter

        int left = 0;
        int right = img.cols;
        int top = 0;
        int bottom = img.rows;
        while(left+1 < right && top+1 < bottom)
        {
            int midh = (left + right)/2;
            int midv = (top + bottom)/2;

            //Get the mean of the 4 subrectangles:
            int alims[] = {
                left, midh, top, midv,
                midh, right, top, midv,
                left, midh, midv, bottom,
                midh, right, midv, bottom};
            int* lims[] = {alims, alims+4, alims+8, alims+12};
            int sum[] = {0, 0, 0, 0};
            double mean[] = {0, 0, 0, 0};
            for(int i = 0; i < 4; i++)
            {
                for(int y = lims[i][2]; y < lims[i][3]; y++)
                    for(int x = lims[i][0]; x < lims[i][1]; x++)
                        sum[i] += sumi.at(y,x);
                mean[i] = sum[i] / (double)((lims[i][1]-lims[i][0])*(lims[i][3]-lims[i][2]));
            }

            //If they are approximately the same (under an expected variance margin)
            if(mean[0]*mean[0] + mean[1]*mean[1] + mean[2]*mean[2] + mean[3]*mean[3] - pow(mean[0]+mean[1]+mean[2]+mean[3], 2)/4 < SIGMA2/((right-left)*(bottom-top)))
            {
                //end recursion

                paperwhiteC = (mean[0]+mean[1]+mean[2]+mean[3])/4;
                //cout << "paperwhiteC" << paperwhiteC << endl;
                break;
            }
            else
            {
                //else look at the whitest subrectange

                int im = 0;
                int mm = mean[0];
                for(int i = 1; i < 4; i++)
                    if(mean[i] > mm)
                    {
                        im = i;
                        mm = mean[i];
                    }
                left = lims[im][0];
                right = lims[im][1];
                top = lims[im][2];
                bottom = lims[im][3];
            }
        }
    }

    //STEP 2: FIT QUADRATIC FUNCTION (PAPERWHITEA~F) AVOIDING OUTLIERS
    {

        int inlierlim = img_norm * 20; //inlier threshold

        for(int it = 4; it > 0; it--)
        {
            double
                    a=0,
                    ax=0,
                    ay=0,
                    axx=0,
                    axy=0,
                    ayy=0,
                    axxx=0,
                    axxy=0,
                    axyy=0,
                    ayyy=0,
                    axxxx=0,
                    axxxy=0,
                    axxyy=0,
                    axyyy=0,
                    ayyyy=0,
                    s=0,
                    sx=0,
                    sy=0,
                    sxx=0,
                    sxy=0,
                    syy=0;

            //In the first iterations, fewer sampling points are used
            int delta = pow(2,it);
            for(int y = 0; y < img.rows; y+=delta)
                for(int x = 0; x < img.cols; x+=delta)
                    if(abs(paperwhiteA*x + paperwhiteB*y + paperwhiteC + paperwhiteD*x*x + paperwhiteE*y*y + paperwhiteF*x*y - sumi.at(y,x)) < inlierlim)
                    {
                        long long xx = x*x;
                        long long yy = y*y;
                        long long xy = x*y;
                        a++;
                        ax+=x;
                        ay+=y;
                        axx+=xx;
                        axy+=xy;
                        ayy+=yy;
                        axxx+=xx*x;
                        axxy+=xx*y;
                        axyy+=x*yy;
                        ayyy+=y*yy;
                        axxxx+=xx*xx;
                        axxxy+=xx*xy;
                        axxyy+=xx*yy;
                        axyyy+=xy*yy;
                        ayyyy+=yy*yy;
                        s+=sumi.at(y,x);
                        sx+=x*sumi.at(y,x);
                        sy+=y*sumi.at(y,x);
                        sxx+=xx*sumi.at(y,x);
                        sxy+=xy*sumi.at(y,x);
                        syy+=yy*sumi.at(y,x);
                    }
            WPP::Matrix<double> AtA = WPP::Matrix<double>::zeros(6, 6);
            WPP::Matrix<double> AtY = WPP::Matrix<double>::zeros(6, 1);

            AtA.at(0,0) = axx;
            AtA.at(0,1) = axy;
            AtA.at(0,2) = ax;
            AtA.at(0,3) = axxx;
            AtA.at(0,4) = axyy;
            AtA.at(0,5) = axxy;

            AtA.at(1,0) = axy;
            AtA.at(1,1) = ayy;
            AtA.at(1,2) = ay;
            AtA.at(1,3) = axxy;
            AtA.at(1,4) = ayyy;
            AtA.at(1,5) = axyy;

            AtA.at(2,0) = ax;
            AtA.at(2,1) = ay;
            AtA.at(2,2) = a;
            AtA.at(2,3) = axx;
            AtA.at(2,4) = ayy;
            AtA.at(2,5) = axy;

            AtA.at(3,0) = axxx;
            AtA.at(3,1) = axxy;
            AtA.at(3,2) = axx;
            AtA.at(3,3) = axxxx;
            AtA.at(3,4) = axxyy;
            AtA.at(3,5) = axxxy;

            AtA.at(4,0) = axyy;
            AtA.at(4,1) = ayyy;
            AtA.at(4,2) = ayy;
            AtA.at(4,3) = axxyy;
            AtA.at(4,4) = ayyyy;
            AtA.at(4,5) = axyyy;

            AtA.at(5,0) = axxy;
            AtA.at(5,1) = axyy;
            AtA.at(5,2) = axy;
            AtA.at(5,3) = axxxy;
            AtA.at(5,4) = axyyy;
            AtA.at(5,5) = axxyy;

            AtY.at(0,0) = sx;
            AtY.at(0,1) = sy;
            AtY.at(0,2) = s;
            AtY.at(0,3) = sxx;
            AtY.at(0,4) = syy;
            AtY.at(0,5) = sxy;

            cout << "ATA" << endl;
            for(int i = 0; i < AtA.rows; i++)
            {
                for(int j = 0; j < AtA.cols; j++)
                    cout << AtA.at(i,j) << "\t";
                cout << endl;
            }
            cout << endl;


            WPP::Matrix<double> sol = AtA.inv() * AtY;
            paperwhiteA = sol.at(0,0);
            paperwhiteB = sol.at(1,0);
            paperwhiteC = sol.at(2,0);
            paperwhiteD = sol.at(3,0);
            paperwhiteE = sol.at(4,0);
            paperwhiteF = sol.at(5,0);

            //cout << "paperwhite: (" << paperwhiteA << ", " << paperwhiteB << ", " << paperwhiteC << ")" << endl;
        }

    }

    //wref: the fitted quadratic function
    WPP::Matrix<short> wref(img.rows, img.cols);
    for(int y = 0, i = 0; y < img.rows; y++)
        for(int x = 0; x < img.cols; x++, i++)
            wref.at(y,x) = paperwhiteA*x + paperwhiteB*y + paperwhiteC + paperwhiteD*x*x + paperwhiteE*y*y + paperwhiteF*x*y;


    //STEP 3. THRESHOLD + DILATION
    nonpaper_ = WPP::dilate(sumi < .92*wref, 11);


    //STEP 4. COMPUTE AND CLASSIFY CONNECTED COMPONENTS

    //sobel to discriminate
    WPP::Matrix<short> gx = WPP::sobel_3_x(sumi);
    WPP::Matrix<short> gy = WPP::sobel_3_y(sumi);

    WPP::Matrix<unsigned short> dst = WPP::Matrix<unsigned short>::zeros(img.rows, img.cols);
    for(int y = 0; y < img.rows; y++)
        for(int x = 0; x < img.cols; x++)
            dst.at(y, x) = sqr((int)gx.at(y, x)) + sqr((int)gy.at(y, x));

    int ncrossesfound;

    cc_enumeration_ = scanlineCC(img_norm, nonpaper_, dst, ncrossesfound);

    cout << ncrossesfound << " components" << endl;

    drawCC(view);

    if(ncrossesfound==NCROSSESCALIB)
    {

        //STEP 5. INITIAL GUESS FOR CROSS CENTER

        //Get the center (min gauss(I)) of each components
        WPP::Matrix<short> gsumi = WPP::gaussian_blur(sumi, 15, 5);
        vector<int> imin(ncrossesfound, 0); //index (x+w*y) of the argmin
        vector<int> vmin(ncrossesfound, 0x300); //minimum found
        for(int i = 0, x = 0, y = 0; i < img.rows * img.cols; i++, x++)
        {
            if(x == img.cols)
            {
                x = 0;
                y++;
            }
            if(cc_enumeration_.at(y, x)>0)
            {
                int sum = gsumi.at(y, x);
                int ix = cc_enumeration_.at(y, x)-1;
                if(vmin[ix] > sum)
                {
                    vmin[ix] = sum;
                    imin[ix] = i;
                }
            }
        }


        //STEP 6. UPDATE CROSS CENTER ITERATIVELY

        //Fit a quadratic equation around center and recompute center
        {
            #define RNG 3
            WPP::Matrix<double> A = WPP::Matrix<double>::zeros((2*RNG+1)*(2*RNG+1),6);
            for(int dy = -RNG, px = 0; dy <= RNG; dy++)
                for(int dx = -RNG; dx <= RNG; dx++, px++)
                {
                    double att = exp(-(dx*dx+dy*dy)/10.);
                    A.at(px,0) = att;
                    A.at(px,1) = att*dx;
                    A.at(px,2) = att*dy;
                    A.at(px,3) = att*dx*dx;
                    A.at(px,4) = att*dx*dy;
                    A.at(px,5) = att*dy*dy;
                }
            WPP::Matrix<double> psiA = (A.t()*A).inv()*A.t();
            //cout << psiA << endl;

            for(int v = 0; v < ncrossesfound; v++)
            {
                double xc = imin[v]%img.cols, yc = imin[v]/img.cols;
                for(int it = 0; it < 5; it++)
                    if(xc >= RNG && xc < img.cols - RNG && yc >= RNG && yc < img.rows - RNG)
                    {
                        WPP::Matrix<double> Y((2*RNG+1)*(2*RNG+1),1);
                        for(int dy = -RNG, px = 0; dy <= RNG; dy++)
                            for(int dx = -RNG; dx <= RNG; dx++, px++)
                            {
                                double att = exp(-(dx*dx+dy*dy)/10.);
                                Y.at(px,0) = att*gsumi.at(yc + dy, xc + dx);
                                //TODO: incorporate gaussian filter to psiA and remove computation of gsumi
                            }
                        WPP::Matrix<double> coefs = psiA * Y;
                        double a = coefs.at(0,0),
                               b = coefs.at(1,0),
                               c = coefs.at(2,0),
                               d = coefs.at(3,0),
                               e = coefs.at(4,0),
                               f = coefs.at(5,0);
                        //min_{x,y}(a+bx+cy+dx²+exy+fy²) ->
                        //[2d  e][x] = [b]
                        //[e  2f][y]   [c]
                        //
                        //[2d  e][-2f e] = [e²-4df      0]
                        //[e  2f][e -2d]   [0      e²-4df]
                        //
                        //[x] = [-2f  e][b] / (e²-4df)
                        //[y]   [e  -2d][c]
                        //
                        //x = (ec-2bf)/(e²-4df)
                        //y = (eb-2dc)/(e²-4df)
                        double det = 4*d*f-e*e;
                        if(d > 0 && f > 0 && det > 0)
                        {
                            double dxc = (e*c-2*b*f)/(-det);
                            double dyc = (e*b-2*d*c)/(-det);
                            xc -= dxc;
                            yc -= dyc;
                        }
                    }
                if(xc >= 0 && xc < img.cols - 1 && yc >= 0 && yc < img.rows - 1)
                    crossescand_[v] = WPP::Point(yc,xc);
                else
                {
                    calibrationFailed();

                    return waiting_acceptance_;
                }
            }
        }

        stablishing_count_++;
        if(stablishing_count_ >= 3) //crosses are "stable" if found NCROSSESCALIB crosses in 3 consecutive frames
        {

            //STEP 7. CLASSIFY CROSSES

            crossescand_.putInOrder();

            drawRectification(view);

            waiting_acceptance_ = true;

        }
    }
    else
        calibrationFailed();

    return waiting_acceptance_;

}

void Calibrator::load()
{
    ifstream f("webcampaperpen.calibration");
    if(!f)
        return;
    for(int i = 0; i < NCROSSESCALIB; i++)
    {
        f >> crosses_[i].x;
        f >> crosses_[i].y;
    }
    if(f.bad())
        return;
    f.close();
    crosses_.computeMatrix();
    defined_ = true;
}

void Calibrator::save()
{
    ofstream f("webcampaperpen.calibration");
    if(!f)
        return;
    for(int i = 0; i < NCROSSESCALIB; i++)
        f << crosses_[i].x << " " << crosses_[i].y << endl;
    f.close();
}
