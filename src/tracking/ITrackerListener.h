#pragma once

#include <Eigen/Core>
#include <Eigen/Dense>

class ITrackerListener
{
public:
    virtual void QuatUpdate(Eigen::Quaterniond quat, Eigen::Quaterniond qImu, Eigen::Quaterniond qImuRel){};
    virtual void CalibUpdate(Eigen::Quaterniond qGyro2Cam, double error, double errorMax, double errorMin){};
};

