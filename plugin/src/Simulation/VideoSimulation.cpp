#include "QSynthi2/Simulation/VideoSimulation.h"

#include "QSynthi2/Video.h"
#include <iostream>

VideoSimulation::VideoSimulation(const int targetWidth, const int targetHeight, const juce::String& filename) {
    file = filename;
    frames = {RealMatrix::Zero(targetHeight, targetWidth)};
    psi = ComplexMatrix::Zero(targetHeight, targetWidth);
}

VideoSimulation::~VideoSimulation() {
}

void VideoSimulation::reset() {
    cv::namedWindow( "Example 2-3", cv::WINDOW_AUTOSIZE );
    cv::VideoCapture cap;
    cap.open(file.toStdString());
    if(cap.isOpened() == 0) {
        std::cout<<"The video file cannot be opened."<<std::endl;
        return;
    }
    cv::Mat frame;
    cap >> frame;
    std::cout<<frame.empty()<<std::endl;  // I have a problem here: Why the result is 1?
    for(int i = 0; i < 600; i++)
    {
        cap >> frame;
        if( frame.empty() ) break; // Ran out of film
        cv::imshow( "Example 2-3", frame );
        if( (char)cv::waitKey(33) >= 0 ) break;
    }
}

const ComplexMatrix& VideoSimulation::getNextFrame(Decimal timestep, const ModulationData &modulationData) {
    return psi;
}
