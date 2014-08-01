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

#ifndef RECTIFICATION_H
#define RECTIFICATION_H

#include "wppmatrix.h"
#include "wpppoint.h"
#include <vector>

//Currently using 4 crosses for calibration, but a 5-crosses scheme could also be used (currently not implemented)
#define NCROSSESCALIB 4

//A vector of NCROSSESCALIB components indicating the position of each cross in calibration
class Rectification: public std::vector<WPP::Point>
{
private:
    //Rectification matrix: H * cross = corner in projective geometry, for some corner in {(0,0,1), (1,0,1), (0,1,1), (1,1,1)}.
    WPP::Matrix<double> matH_;

public:

    //Solves rectification for already arranged crosses
    //Preconditioned with Z=128.
    void computeMatrix();

    //Puts vector in order:
    //    3       2
    //
    //  0           1
    void putInOrder();

    Rectification(): std::vector<WPP::Point>(NCROSSESCALIB){}

    //Returns a Point in [0,1]x[0,1] indicating the position relative to the four crosses
    WPP::Point rectifyPoint(double ptx, double pty) const
    {
        WPP::Matrix<double> u(3, 1);
        u.at(0,0) = ptx;
        u.at(1,0) = pty;
        u.at(2,0) = 1;
        WPP::Matrix<double> v = matH_ * u;
        return WPP::Point(
            v.at(1,0)/v.at(2,0),
            v.at(0,0)/v.at(2,0));
    }

};

#endif // RECTIFICATION_H
