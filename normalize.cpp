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

#include "normalize.h"
#include <vector>

using namespace std;

struct RGBSUM
{
    int R, G, B;

    bool operator<(const RGBSUM & other) const
    {
        return (R+G+B > other.R+other.G+other.B);
    }
};

double normalize(const WPP::Matrix<WPP::RGB> & cfr_raw)
{
    const int linemod = 10;
    vector<RGBSUM> linesum(cfr_raw.rows/linemod);

    //compute the sum of one of every 10 lines
    for(int i = 0; i < linesum.size(); i++)
    {
        const unsigned char * c = &cfr_raw.at(i*linemod,0)[0];
        RGBSUM & v = linesum[i];
        for(int L = cfr_raw.cols; L>0; L--)
        {
            v.B += *(c++);
            v.G += *(c++);
            v.R += *(c++);
        }
    }

    //take the mean of the 10 brightest ones
    sort(linesum.begin(), linesum.end());
    int sR=0, sG=0, sB=0;
    for(int i = 0; i < 10; i++)
    {
        sR += linesum[i].R;
        sG += linesum[i].G;
        sB += linesum[i].B;
    }
    double mean2 = sR+sG+sB;

    return mean2/4200/cfr_raw.cols;
}
