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

#include "feedbackviewer.h"

FeedbackViewer::FeedbackViewer(QWidget *parent) :
    QLabel(parent)
{
    connect(this, SIGNAL(showImage_(QImage)), this, SLOT(setImage_(QImage)));
}

void FeedbackViewer::setImage(const WPP::Matrix<WPP::RGB> & view)
{
    QImage img(view.cols, view.rows, QImage::Format_RGB888);
    for(int i = 0; i < view.rows; i++)
    {
        const WPP::RGB * rgb1 = & view.at(i,view.cols-1);
        WPP::RGB * rgb2 = (WPP::RGB*)img.scanLine(i);
        for(int j = 0; j < view.cols; j++)
        {
            (*rgb2)[0] = (*rgb1)[2];
            (*rgb2)[1] = (*rgb1)[1];
            (*rgb2)[2] = (*rgb1)[0];
            rgb1--;
            rgb2++;
        }
    }
    emit showImage_(img);
}

void FeedbackViewer::setImage_(QImage img)
{
    setPixmap(QPixmap::fromImage(img));
}
