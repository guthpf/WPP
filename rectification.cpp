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

#include "rectification.h"
#include <Eigen/SVD>

namespace WPP
{
    void full_svd(const Matrix<double> & m, Matrix<double> & w, Matrix<double> & u, Matrix<double> & vt)
    {
        //convert WPP to Eigen
        Eigen::MatrixXd mx(m.rows, m.cols);
        for(int i = 0; i < m.rows; i++)
            for(int j = 0; j < m.cols; j++)
                mx(i,j) = m.at(i,j);

        //Call Eigen SVD
        Eigen::JacobiSVD<Eigen::MatrixXd> svd(mx, Eigen::ComputeFullU | Eigen::ComputeFullV);

        //convert back Eigen to WPP
        u = Matrix<double>(m.rows, m.rows);
        vt = Matrix<double>(m.cols, m.cols);
        w = Matrix<double>(svd.singularValues().rows(), 1);
        for(int i = 0; i < m.rows; i++)
            for(int j = 0; j < m.rows; j++)
                u.at(i,j) = svd.matrixU()(i,j);
        for(int i = 0; i < m.cols; i++)
            for(int j = 0; j < m.cols; j++)
                vt.at(j,i) = svd.matrixV()(i,j);
        for(int i = 0; i < w.rows; i++)
            w.at(i,0) = svd.singularValues()(i);
    }
}

void Rectification::computeMatrix()
{
    std::vector<WPP::Point> & cvec = *this;

    WPPASSERT(cvec.size()==4)

    //Now solve:
    //                    [ 1  0  0  1]
    //H[u0 u1 u2 u3 u4] = [ 0  0  1  1] D
    //                    [ 1  1  1  1]
    WPP::Matrix<double> tgt(2,4);
    tgt.at(0,0) = 1;
    tgt.at(1,0) = 0;
    tgt.at(0,1) = 0;
    tgt.at(1,1) = 0;
    tgt.at(0,2) = 0;
    tgt.at(1,2) = 1;
    tgt.at(0,3) = 1;
    tgt.at(1,3) = 1;

    //tgt_i x Hu_i = 0
    WPP::Matrix<double> system = WPP::Matrix<double>::zeros(tgt.cols*2,9);
    for(int i = 0; i < tgt.cols; i++)
    {
        //tgt_i1 * (H20 u0 + H21u1 + H22u2) - tgt_i2 * (H10 u0 + H11u1 + H12u2) = 0
        //tgt_i2 * (H00 u0 + H01u1 + H02u2) - tgt_i0 * (H20 u0 + H21u1 + H22u2) = 0
        system.at(2*i, 0) = 0;
        system.at(2*i, 1) = 0;
        system.at(2*i, 2) = 0;
        system.at(2*i, 3) = -cvec[i].x;
        system.at(2*i, 4) = -cvec[i].y;
        system.at(2*i, 5) = -128;
        system.at(2*i, 6) = tgt.at(1,i) * cvec[i].x;
        system.at(2*i, 7) = tgt.at(1,i) * cvec[i].y;
        system.at(2*i, 8) = tgt.at(1,i) * 128;
        system.at(2*i+1, 0) = cvec[i].x;
        system.at(2*i+1, 1) = cvec[i].y;
        system.at(2*i+1, 2) = 128;
        system.at(2*i+1, 3) = 0;
        system.at(2*i+1, 4) = 0;
        system.at(2*i+1, 5) = 0;
        system.at(2*i+1, 6) = -tgt.at(0,i) * cvec[i].x;
        system.at(2*i+1, 7) = -tgt.at(0,i) * cvec[i].y;
        system.at(2*i+1, 8) = -tgt.at(0,i) * 128;
    }
    WPP::Matrix<double> w, u, vt;
    WPP::full_svd(system, w, u, vt);
    WPP::Matrix<double> H(3,3);
    H.at(0,0) = vt.at(8,0);
    H.at(0,1) = vt.at(8,1);
    H.at(0,2) = vt.at(8,2)*128;
    H.at(1,0) = vt.at(8,3);
    H.at(1,1) = vt.at(8,4);
    H.at(1,2) = vt.at(8,5)*128;
    H.at(2,0) = vt.at(8,6);
    H.at(2,1) = vt.at(8,7);
    H.at(2,2) = vt.at(8,8)*128;
    matH_ = H;
}

void Rectification::putInOrder()
{
    Rectification & crossescand_ = *this;
#define SWAP(i,j) {WPP::Point temp = crossescand_[i]; crossescand_[i] = crossescand_[j]; crossescand_[j] = temp;}
    WPPASSERT(NCROSSESCALIB == 4)
    WPPASSERT(crossescand_.size() == 4)

    //sort by X
    for(int i = 0; i < NCROSSESCALIB; i++)
        for(int j = i+1; j < NCROSSESCALIB; j++)
            if(crossescand_[i].x > crossescand_[j].x)
                SWAP(i,j)
    if(crossescand_[0].y < crossescand_[1].y)
        SWAP(0,1)
    if(crossescand_[2].y > crossescand_[3].y)
        SWAP(2,3)
    SWAP(1,3)
#undef SWAP
}
