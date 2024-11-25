#include <QTcpSocket>
#include <QProcess>
#include <QTimer>

#include "common/math/utils.h"
#include "common/protocol/msg.h"
#include "Visualizer.h"

Visualizer::Visualizer(QObject *parent) : QObject{parent} {
    setlocale(LC_NUMERIC, "en_US.UTF-8");
}

void Visualizer::start() {
    socket = new QTcpSocket(this);
}

void Visualizer::spawnLight() {
    QProcess *process = new QProcess(this);

    connect(process, &QProcess::finished, process, [this, process]() {
        finished();
        process->deleteLater();
    });

    process->start("../../third-party/visualization-3d/server/build/server");

    QTimer::singleShot(1000, [this]() {
        socket->connectToHost("localhost", 8080);
        socket->waitForConnected();

        write("mode light\n");
        write("clear\n");
        write("create pos            empty\n");
        write("create pos.marker     cuboid material color 255 255 255 geometry 0.25 0.25 0.25\n");
        write("create pos.marker.x   cuboid material color 255   0   0 geometry 1.00 0.05 0.05 transform translation 0.5 0 0\n");
        write("create pos.marker.y   cuboid material color   0 255   0 geometry 0.05 1.00 0.05 transform translation 0 0.5 0\n");
        write("create pos.marker.z   cuboid material color   0   0 255 geometry 0.05 0.05 1.00 transform translation 0 0 0.5\n");
        write("create pos.marker.acc sphere material color   0 255   0 geometry 0.05\n");
        write("create pos.marker.mag sphere material color   0   0 255 geometry 0.05\n");
        write("create pos.vel        sphere material color 255   0   0 geometry 0.05\n");
        write("create gps            sphere material color 255   0 255 geometry 0.15\n");
    });
}

void Visualizer::spawnDark() {
    QProcess *process = new QProcess(this);

    connect(process, &QProcess::finished, process, [this, process]() {
        finished();
        process->deleteLater();
    });

    process->start("../../third-party/visualization-3d/server/build/server");

    QTimer::singleShot(1000, [this]() {
        socket->connectToHost("localhost", 8080);
        socket->waitForConnected();

        write("mode dark\n");
        write("clear\n");
        write("create pos            empty\n");
        write("create pos.marker     cuboid material color 255 255 255 geometry 0.25 0.25 0.25\n");
        write("create pos.marker.x   cuboid material color 255   0   0 geometry 1.00 0.05 0.05 transform translation 0.5 0 0\n");
        write("create pos.marker.y   cuboid material color   0 255   0 geometry 0.05 1.00 0.05 transform translation 0 0.5 0\n");
        write("create pos.marker.z   cuboid material color   0   0 255 geometry 0.05 0.05 1.00 transform translation 0 0 0.5\n");
        write("create pos.marker.acc sphere material color   0 255   0 geometry 0.05\n");
        write("create pos.marker.mag sphere material color   0   0 255 geometry 0.05\n");
        write("create pos.vel        sphere material color 255   0   0 geometry 0.05\n");
        write("create gps            sphere material color 255   0 255 geometry 0.15\n");
    });
}

void Visualizer::receive(const uint8_t id, const double time, const QByteArray &payload) {
    (void)time;

    if(id==MSG_ID_SENSOR && payload.size()==sizeof(msg_frame_sensor_t)) {
        const msg_frame_sensor_t *sensor = reinterpret_cast<const msg_frame_sensor_t *>(payload.data());

        if(sensor->valid.accelerometer) {
            float accelerometer[3];
            utils_normalize(sensor->accelerometer.calib, accelerometer, 3);
            write("update pos.marker.acc transform translation %f %f %f\n",
                accelerometer[0],
                accelerometer[1],
                accelerometer[2]
            );
        }

        if(sensor->valid.magnetometer) {
            float magnetometer[3];
            utils_normalize(sensor->magnetometer.calib, magnetometer, 3);
            write("update pos.marker.mag transform translation %f %f %f\n",
                magnetometer[0],
                magnetometer[1],
                magnetometer[2]
            );
        }

        if(sensor->valid.gps) {
            float cartesian[2];
            utils_gps_to_enu(sensor->gps, latlon_ref, cartesian);
            write("update gps transform translation %f %f 0\n",
                cartesian[0],
                cartesian[1]
            );
        }

        return;
    }

    if(id==MSG_ID_ESTIMATION && payload.size()==sizeof(msg_frame_estimation_t)) {
        const msg_frame_estimation_t *estimation = reinterpret_cast<const msg_frame_estimation_t *>(payload.data());

        if(estimation->position_reference.valid) {
            memcpy(latlon_ref, estimation->position_reference.latlon, 2*sizeof(float));
        }

        constexpr double alpha = 0.99;
        cameraPosition[0] = alpha*cameraPosition[0] + (1 - alpha)*estimation->position[0];
        cameraPosition[1] = alpha*cameraPosition[1] + (1 - alpha)*estimation->position[1];
        cameraPosition[2] = alpha*cameraPosition[2] + (1 - alpha)*estimation->position[2];
        write("camera %f %f %f\n", cameraPosition[0], cameraPosition[1], cameraPosition[2]);

        float quaternion[4];
        utils_normalize(estimation->orientation, quaternion, 4);
        write("update pos.marker transform quaternion %f %f %f %f\n",
            quaternion[0],
            quaternion[1],
            quaternion[2],
            quaternion[3]
        );

        write("update pos transform translation %f %f %f\n",
            estimation->position[0],
            estimation->position[1],
            estimation->position[2]
        );

        write("update pos.vel transform translation %f %f %f\n",
            estimation->velocity[0],
            estimation->velocity[1],
            estimation->velocity[2]
        );

        return;
    }
}

void Visualizer::write(const char *format, ...) {
    if(socket->state()!=QAbstractSocket::SocketState::ConnectedState) {
        return;
    }

    va_list args;
    va_start(args, format);

	char str[256];
    const size_t len = vsprintf(str, format, args);

    socket->write(str, len);
    socket->flush();
}
