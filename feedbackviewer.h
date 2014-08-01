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

#ifndef FEEDBACKVIEWER_H
#define FEEDBACKVIEWER_H

#include <QLabel>

#include "wppmatrix.h"
#include "wpppoint.h"

class FeedbackViewer : public QLabel
{
    Q_OBJECT
public:
    explicit FeedbackViewer(QWidget *parent = 0);

    void setImage(const WPP::Matrix<WPP::RGB> &);

signals:

    void showImage_(QImage);
    
private slots:

    void setImage_(QImage);

};

#endif // FEEDBACKVIEWER_H
