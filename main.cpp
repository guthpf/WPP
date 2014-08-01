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

#if QT_VERSION >= 0x050000
#include <QtWidgets/QApplication>
#else
#include <QApplication>
#endif
#include "mainwindow.h"

#include <string>
#include <iostream>
#include <cstdlib>
using namespace std;

int main(int argc, char *argv[])
{
#ifdef WIN32
#else
    if(argc==1)
        return system(("gksudo " + string(argv[0]) + " 0").c_str());
#endif

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    
    return a.exec();
}
