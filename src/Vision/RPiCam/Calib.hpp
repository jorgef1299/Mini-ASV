//***************************************************************************
// Copyright 2007-2020 Universidade do Porto - Faculdade de Engenharia      *
// Laboratório de Sistemas e Tecnologia Subaquática (LSTS)                  *
//***************************************************************************
// This file is part of DUNE: Unified Navigation Environment.               *
//                                                                          *
// Commercial Licence Usage                                                 *
// Licencees holding valid commercial DUNE licences may use this file in    *
// accordance with the commercial licence agreement provided with the       *
// Software or, alternatively, in accordance with the terms contained in a  *
// written agreement between you and Faculdade de Engenharia da             *
// Universidade do Porto. For licensing terms, conditions, and further      *
// information contact lsts@fe.up.pt.                                       *
//                                                                          *
// Modified European Union Public Licence - EUPL v.1.1 Usage                *
// Alternatively, this file may be used under the terms of the Modified     *
// EUPL, Version 1.1 only (the "Licence"), appearing in the file LICENCE.md *
// included in the packaging of this file. You may not use this work        *
// except in compliance with the Licence. Unless required by applicable     *
// law or agreed to in writing, software distributed under the Licence is   *
// distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF     *
// ANY KIND, either express or implied. See the Licence for the specific    *
// language governing permissions and limitations at                        *
// https://github.com/LSTS/dune/blob/master/LICENCE.md and                  *
// http://ec.europa.eu/idabc/eupl.html.                                     *
//***************************************************************************
// Author: Alexandre Rocha                                                  *
//***************************************************************************

#include <DUNE/DUNE.hpp>

#include <opencv2/calib3d.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

#define MAX_PICAM_ANGLE 31.1

namespace Vision {
using DUNE_NAMESPACES;
namespace RPiCam {

//! Intrinsic Parameters
float intrinsic_parameters[9] = {
    681.9474487304688,    0.000000000000000000, 279.387553359592862,
    0.000000000000000000, 679.4100341796875,    240.90015807337113,
    0.000000000000000000, 0.000000000000000000, 1.000000000000000000};
//! Distortion Coeficients
float distortion_coeficients[5] = {-0.4624503562479969, -0.43432558990654135,
                                   0.001974482671297278, -0.008023538703377298,
                                   2.9986277588121113};
//! Distortion Coeficients
float roi_limits[4] = {7, 13, 623, 453};

void undistortionMaps(cv::Mat &map_1, cv::Mat &map_2, cv::VideoCapture &cap) {

  cv::Mat temp;
  cv::Mat camera_matrix, dist_coefs;

  cap.read(temp);
  map_1 = cv::Mat(temp.size(), CV_16SC2);
  map_2 = cv::Mat(temp.size(), CV_16UC1);

  camera_matrix = cv::Mat(3, 3, CV_32F, intrinsic_parameters);
  dist_coefs = cv::Mat(1, 5, CV_32F, distortion_coeficients);

  cv::initUndistortRectifyMap(camera_matrix, dist_coefs, cv::Mat(),
                              camera_matrix, temp.size(), CV_32F, map_1, map_2);
}

void cropROI(cv::Mat &frame) {

  cv::Rect new_roi =
      cv::Rect(roi_limits[0], roi_limits[1], roi_limits[2], roi_limits[3]);
  frame = frame(new_roi);
}

} // namespace RPiCam
} // namespace Vision