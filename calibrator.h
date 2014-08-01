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

#ifndef CALIBRATOR_H
#define CALIBRATOR_H

#include "rectification.h"

class Calibrator
{
private:

    //True if calibration is completed
    bool defined_;

    //Must wait a few frames to get stable
    int stablishing_count_;

    //Resets stablishing_count_. Call not more than once per frame.
    void calibrationFailed()
    {
        if(stablishing_count_ < 0)
            stablishing_count_++;
        else
            stablishing_count_ = 0;
    }

    //whether pixels were classified as paper or not
    WPP::Matrix<bool> nonpaper_;

    //Enumerates cross-like connected components
    WPP::Matrix<char> cc_enumeration_;

    //Latest accepted calibration
    Rectification crosses_;

    //Recently computed calibration
    Rectification crossescand_;

    //True if calibration was computed but not yet accepted
    bool waiting_acceptance_;

    //draws connected components reddish (non-cross) or blueish (cross) depending on classification, using cc_enumeration_ and nonpaper_
    void drawCC(WPP::Matrix<WPP::RGB> & view);

    //draws a rectangle linking the crosses
    void drawRectification(WPP::Matrix<WPP::RGB> & view);

public:

    const Rectification & rectification()
    {
        return crosses_;
    }

    //Called when recalibrate button is pressed
    void reset()
    {
        stablishing_count_ = 0;
        defined_ = false;
    }

    //loads calibration from file
    void load();

    //save calibration from file
    void save();

    Calibrator()
    {
        waiting_acceptance_ = false;
        reset();
        load();
    }

    //True if calibration was computed but not yet accepted
    bool waitingAcceptance()
    { return waiting_acceptance_; }

    //True if calibration is completed
    bool isDefined()
    { return defined_; }

    //Tell to Calibrator whether the calibration was accepted or not
    void decide(bool accepted)
    {
        waiting_acceptance_ = false;

        if(!accepted)
            stablishing_count_ = -5;
        else
        {
            defined_ = true;
            crosses_ = crossescand_;
            crosses_.computeMatrix();
            save();
        }
    }

    //Render classification from the frame when calibration was found
    void drawFoundCalibration(
        WPP::Matrix<WPP::RGB> & view)
    {
        drawCC(view);

        drawRectification(view);
    }

    //kernel of the calibration algorithm
    //returns true when crosses are found and stable (found in 3 consecutive frames)
    bool findCrosses(
        double img_norm, //result from normalize()
        const WPP::Matrix<WPP::RGB> & img,
        WPP::Matrix<WPP::RGB> & view);
};

#endif // CALIBRATOR_H
