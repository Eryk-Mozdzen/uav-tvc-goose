#include <drake/geometry/meshcat.h>

#include "common/math/utils.h"
#include "common/protocol/msg.h"
#include "Visualizer.h"

using namespace drake::math;
using namespace drake::geometry;

Visualizer::Visualizer(QObject *parent) : QObject{parent} {

}

void Visualizer::start() {
    visualizer = new Meshcat();

    visualizer->SetObject("pos", Sphere(0.1));
    visualizer->SetObject("pos/marker", Box(0.25, 0.25, 0.25), Rgba(1, 1, 1));
    visualizer->SetObject("pos/marker/x", Box(1, 0.05, 0.05), Rgba(1, 0, 0));
    visualizer->SetObject("pos/marker/y", Box(0.05, 1, 0.05), Rgba(0, 1, 0));
    visualizer->SetObject("pos/marker/z", Box(0.05, 0.05, 1), Rgba(0, 0, 1));
    visualizer->SetObject("pos/marker/acc", Sphere(0.05), Rgba(0, 1, 0));
    visualizer->SetObject("pos/marker/mag", Sphere(0.05), Rgba(0, 0, 1));
    visualizer->SetObject("pos/vel", Sphere(0.05), Rgba(1, 0, 0));
    visualizer->SetObject("gps", Sphere(0.15), Rgba(1, 0, 1));

    visualizer->SetTransform("pos/marker/x", RigidTransformd(Eigen::Vector3d(0.5, 0, 0)));
    visualizer->SetTransform("pos/marker/y", RigidTransformd(Eigen::Vector3d(0, 0.5, 0)));
    visualizer->SetTransform("pos/marker/z", RigidTransformd(Eigen::Vector3d(0, 0, 0.5)));
}

void Visualizer::receive(const uint8_t id, const QByteArray &payload) {

    if(id==MSG_ID_SENSOR && payload.size()==sizeof(msg_frame_sensor_t)) {
        const msg_frame_sensor_t *sensor = reinterpret_cast<const msg_frame_sensor_t *>(payload.data());

        if(sensor->valid.accelerometer) {
            visualizer->SetTransform("pos/marker/acc", RigidTransformd(Eigen::Vector3d(
                sensor->accelerometer.calib[0],
                sensor->accelerometer.calib[1],
                sensor->accelerometer.calib[2]
            )));
        }

        if(sensor->valid.magnetometer) {
            visualizer->SetTransform("pos/marker/mag", RigidTransformd(Eigen::Vector3d(
                sensor->magnetometer.calib[0],
                sensor->magnetometer.calib[1],
                sensor->magnetometer.calib[2]
            )));
        }

        if(sensor->valid.gps) {
            float cartesian[2];
            utils_gps_to_enu(sensor->gps, cartesian);

            visualizer->SetTransform("gps", RigidTransformd(Eigen::Vector3d(cartesian[0], cartesian[1], 0)));
        }

        return;
    }

    if(id==MSG_ID_ESTIMATION && payload.size()==sizeof(msg_frame_estimation_t)) {
        const msg_frame_estimation_t *estimation = reinterpret_cast<const msg_frame_estimation_t *>(payload.data());

        visualizer->SetTransform("pos", RigidTransformd(Eigen::Vector3d(
            estimation->position[0],
            estimation->position[1],
            estimation->position[2]
        )));

        visualizer->SetTransform("pos/marker", RigidTransformd(Eigen::Quaternion<double>(
            estimation->orientation[0],
            estimation->orientation[1],
            estimation->orientation[2],
            estimation->orientation[3]
        ), Eigen::Vector3d(0, 0, 0)));

        return;
    }
}
