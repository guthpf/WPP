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

#ifndef WPPMATRIX_H
#define WPPMATRIX_H

#include "common.h"

namespace WPP
{
    //Handles memory allocation efficiently to avoid reduplication and memory leaking
    template <class T>
    class Reference
    {
    private:
        int * count_;
        T * data_;

        void checkDelete()
        {
            if(*count_==0)
            {
                delete data_;
                delete count_;
            }
        }

    public:

        T * data()
        {
            return data_;
        }

        const T * data() const
        {
            return data_;
        }

        Reference()
        {
            data_ = new T;
            count_ = new int;
            *count_ = 1;
        }

        Reference(const Reference & other)
        {
            data_ = other.data_;
            count_ = other.count_;
            (*other.count_)++;
        }

        Reference & operator=(const Reference & other)
        {
            (*other.count_)++;
            (*count_)--;
            checkDelete();
            data_ = other.data_;
            count_ = other.count_;
            return *this;
        }

        virtual ~Reference()
        {
            (*count_)--;
            checkDelete();
        }
    };

    //Very simple vector class
    template <typename T>
    class Vector_
    {
    public:
        T * data;

        void clean()
        {
            if(data)
            {
                delete[] data;
                data = NULL;
            }
        }

        Vector_()
        {
            data = NULL;
        }

        void alloc(int n)
        {
            clean();
            data = new T[n];
        }

        virtual ~Vector_()
        {
            clean();
        }
    };
}

#include <Eigen/Eigen>

namespace WPP
{
    //Template-based matrix class for linear algebra operation and handling images
    //Similarly to cv::Mat and QImage, handles memory allocation efficiently to avoid reduplication and memory leaking
    template <typename T>
    class Matrix: private Reference<Vector_<T> >
    {
    public:
        int rows;
        int cols;

        Matrix<T>(): Reference<Vector_<T> >()
        {
            rows = 0;
            cols = 0;
        }

        Matrix<T>(int rows_, int cols_): Reference<Vector_<T> >()
        {
            rows = rows_;
            cols = cols_;
            Reference<Vector_<T> >::data()->alloc(rows_*cols_);
        }

        Matrix<T>(const Matrix<T> & other): Reference<Vector_<T> >(other)
        {
            rows = other.rows;
            cols = other.cols;
        }

        Matrix<T> clone() const
        {
            Matrix<T> other(rows, cols);
            int n = rows*cols*sizeof(T);
            for(int i = 0; i < n; i++)
                ((char*)(other.data()->data))[i] = ((const char*)(this->data()->data))[i];
            return other;
        }

        inline T & at(int i, int j)
        {
            return this->data()->data[i*cols+j];
        }

        inline const T & at(int i, int j) const
        {
            return this->data()->data[i*cols+j];
        }

        static Matrix<T> zeros(int rows_, int cols_)
        {
            Matrix<T> other(rows_, cols_);
            int n = rows_*cols_*sizeof(T);
            for(int i = 0; i < n; i++)
                ((char*)(other.data()->data))[i] = 0;
            return other;
        }

        Matrix<T> operator*(const Matrix<T> & other) const
        {
            WPPASSERT(cols==other.rows)
            Matrix<T> ret = Matrix<T>::zeros(rows, other.cols);
            for(int i = 0; i < ret.rows; i++)
                for(int j = 0; j < ret.cols; j++)
                    for(int k = 0; k < cols; k++)
                        ret.at(i,j) += at(i,k) * other.at(k,j);
            return ret;
        }

        Matrix<bool> operator>(T c) const
        {
            Matrix<bool> ret(rows, cols);
            for(int i = 0; i < rows; i++)
                for(int j = 0; j < cols; j++)
                    ret.at(i,j) = (at(i,j) > c);
            return ret;
        }

        Matrix<bool> operator<(const Matrix<T> & other) const
        {
            WPPASSERT(cols == other.cols)
            WPPASSERT(rows == other.rows)
            Matrix<bool> ret(rows, cols);
            for(int i = 0; i < rows; i++)
                for(int j = 0; j < cols; j++)
                    ret.at(i,j) = (at(i,j) < other.at(i,j));
            return ret;
        }

        //returns transpose
        Matrix<T> t() const
        {
            Matrix<T> ret(cols, rows);
            for(int i = 0; i < rows; i++)
                for(int j = 0; j < cols; j++)
                    ret.at(j,i) = at(i,j);
            return ret;
        }

        Matrix<T> inv() const
        {
            WPPASSERT(rows == cols)
            Eigen::MatrixXd m(rows, cols);
            for(int i = 0; i < rows; i++)
                for(int j = 0; j < cols; j++)
                    m(i,j) = at(i,j);
            Eigen::MatrixXd minv = m.inverse();
            Matrix<T> ret(rows, cols);
            for(int i = 0; i < rows; i++)
                for(int j = 0; j < cols; j++)
                    ret.at(i,j) = minv(i,j);
            return ret;
        }
    };

    template <typename T>
    Matrix<T> operator*(double c, const Matrix<T> & m)
    {
        Matrix<T> ret(m.rows, m.cols);
        for(int i = 0; i < m.rows; i++)
            for(int j = 0; j < m.cols; j++)
                ret.at(i,j) = m.at(i,j) * c;
        return ret;
    }
}

#endif // WPPMATRIX_H
