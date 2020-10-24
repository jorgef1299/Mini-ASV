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

// DUNE headers.
#include <DUNE/DUNE.hpp>

#include "Calib.hpp"

namespace Vision {
//! Insert short task description here.
//!
//! Insert explanation on task behaviour here.
//! @author Alexandre Rocha
using DUNE_NAMESPACES;

namespace RPiCam {
struct Arguments {
  //! Manouver-is-over threshold distance
  float finish_dist;
};

struct Task : public DUNE::Tasks::Task {
  //! LiDAR frontal distance measurement
  double frontal_dist;
  //! Capture RPiCam video
  cv::VideoCapture cap;
  //! Capture width
  uint16_t width;
  //! Capture height
  uint16_t height;
  //! Video frame
  cv::Mat cap_frame;
  //! Undistortion Map 1
  cv::Mat map_1;
  //! Undistortion Map 2
  cv::Mat map_2;
  
  //! Heading reference to aim
  double heading_ref;
  
  //! Calibration parameters
  FrameCalibrationParemeters calib_data;
  //! Task Arguments
  Arguments m_args;

  //! Constructor.
  //! @param[in] name task name.
  //! @param[in] ctx context.
  Task(const std::string &name, Tasks::Context &ctx)
      : DUNE::Tasks::Task(name, ctx), 
      frontal_dist(0.0),
      width(640),
      height(480) {
    paramActive(Tasks::Parameter::SCOPE_MANEUVER,
                Tasks::Parameter::VISIBILITY_USER);

    param("Manouver-is-over threshold distance", m_args.finish_dist)
        .defaultValue("1.0")
        .description(
            "Distance used as reference to confirm docking manouver success");

    bind<IMC::Distance>(this);
  }

  void consume(const IMC::Distance *msg) {
    if (!msg->validity)
      return;

    frontal_dist = msg->value;
  }

  //! Update internal state with new parameter values.
  void onUpdateParameters(void) {}

  //! Reserve entity identifiers.
  void onEntityReservation(void) {}

  //! Resolve entity names.
  void onEntityResolution(void) {}

  //! Acquire resources.
  void onResourceAcquisition(void) {
    setEntityState(IMC::EntityState::ESTA_NORMAL, Status::CODE_ACTIVE);
    cap.open(0);
  }

  //! Initialize resources.
  void onResourceInitialization(void) {
    cap.set(cv::CAP_PROP_FRAME_WIDTH, width);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, height);

    undistortionMaps(map_1, map_2, cap, calib_data);
  }

  //! Release resources.
  void onResourceRelease(void) {
    cap.release();
  }

  //! Red Circle Detection
  void redCircleDetection(void) {
    
    calib_data.cam_matrix_data = {
    681.9474487304688,    0.000000000000000000, 279.387553359592862,
    0.000000000000000000, 679.4100341796875,    240.90015807337113,
    0.000000000000000000, 0.000000000000000000, 1.000000000000000000};

    calib_data.dist_matrix_data = {-0.4624503562479969, -0.43432558990654135,
                             0.001974482671297278, -0.008023538703377298,
                             2.9986277588121113};

    calib_data.roi = {7, 13, 623, 453};

    remap(cap_frame, cap_frame, map_1, map_2, cv::INTER_LINEAR);
    cropROI(cap_frame, calib_data);

    cap.read(cap_frame);
    imshow("debug window", cap_frame);
  }

  //! Main loop.
  void onMain(void) {
    if (!cap.isOpened()) // check if we succeeded
      return;

    while (!stopping()) {
      
      redCircleDetection();
            
      waitForMessages(1.0);
    }
  }
};
} // namespace RPiCam
} // namespace Vision

DUNE_TASK
