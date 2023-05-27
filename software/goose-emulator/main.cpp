#include <frames.h>
#include <libserial/SerialPort.h>
#include <math.h>
#include <unistd.h>
#include <iostream>
#include <thread>

#include "transfer.h"

constexpr const char *const SERIAL_PORT_2 = "/dev/pts/8";
LibSerial::SerialPort serial;

struct Generator {
    inline static float angle = 0.0;
    float phase;
    float scale;
    float offset;
    Generator(float phase = 0.0, float scale = 1.0, float offset = 0.0) : phase{phase}, scale{scale}, offset{offset} {
    }

    inline static void update() {
        angle += 0.1;
    }

    float get_sin() {
        return sinf(angle + phase) * scale + offset;
    }

    float get_cos() {
        return cosf(angle + phase) * scale + offset;
    }
};

template <typename T>
static void send(const T &payload, const Transfer::ID id) {
    Transfer::FrameTX tx = Transfer::encode(payload, id);
    for (std::size_t i = 0; i < tx.length; ++i) {
        serial.WriteByte(tx.buffer[i]);
        serial.DrainWriteBuffer();
    }
}

void send_log() {
    std::string warn_data = "warn working?";
    Transfer::FrameTX tx1 = Transfer::encode(warn_data.c_str(), warn_data.size(), Transfer::LOG_WARNING);
    for (std::size_t i = 0; i < tx1.length; ++i) {
        serial.WriteByte(tx1.buffer[i]);
        serial.DrainWriteBuffer();
    }

    std::string error_data = "error working?";
    Transfer::FrameTX tx2 = Transfer::encode(error_data.c_str(), error_data.size(), Transfer::LOG_ERROR);
    for (std::size_t i = 0; i < tx2.length; ++i) {
        serial.WriteByte(tx2.buffer[i]);
        serial.DrainWriteBuffer();
    }

    std::string debug_data = "Debug working?";
    Transfer::FrameTX tx3 = Transfer::encode(debug_data.c_str(), debug_data.size(), Transfer::LOG_DEBUG);
    for (std::size_t i = 0; i < tx3.length; ++i) {
        serial.WriteByte(tx3.buffer[i]);
        serial.DrainWriteBuffer();
    }

    std::string info_data = "Info working?";
    Transfer::FrameTX tx4 = Transfer::encode(info_data.c_str(), info_data.size(), Transfer::LOG_INFO);
    for (std::size_t i = 0; i < tx4.length; ++i) {
        serial.WriteByte(tx4.buffer[i]);
        serial.DrainWriteBuffer();
    }
}

int main(int argc, char **argv) {
    (void)argc;
    try {
        serial.Open(argv[1]);

        serial.SetBaudRate(LibSerial::BaudRate::BAUD_115200);
        serial.SetCharacterSize(LibSerial::CharacterSize::CHAR_SIZE_8);
        serial.SetFlowControl(LibSerial::FlowControl::FLOW_CONTROL_NONE);
        serial.SetParity(LibSerial::Parity::PARITY_NONE);
        serial.SetStopBits(LibSerial::StopBits::STOP_BITS_1);
        Transfer transfer;

        struct __attribute__((packed)) Quaternion {
            float x, y, z, w;
        };
        struct __attribute__((packed)) Vector3D {
            float x, y, z;
        };
        Generator voltage_generator(1.0, 1.7, 10.7);
        Generator current_generator(1.0, 1.0, 1.0);
        Generator battery_level_generator(-1.0, 100.0, 50.0);
        Generator distace_generator(0.0, 4.0, 4.0);

        Generator preasure_generator(3.14, 100, 102400);
        Generator servo_generators[] = {{0.2, 0.2616}, {0.8, 0.2616}, {2.0, 0.2616}, {3.14, 0.2616}};
        Generator trottle_generator(0.2, 50.0, 50.0);

        Quaternion estimation_attitude = {0.0, 0.0, sinf(Generator::angle), cosf(Generator::angle)};
        std::size_t i = 0;
        while (1) {
            send_log();
            estimation_attitude.z = sinf(Generator::angle);
            estimation_attitude.w = cosf(Generator::angle);
            send(estimation_attitude, Transfer::ID::TELEMETRY_ESTIMATION_ATTITUDE);

            estimation_attitude.z = sinf(Generator::angle + 0.1);
            estimation_attitude.w = cosf(Generator::angle + 0.1);
            send(estimation_attitude, Transfer::ID::CONTROL_ATTITUDE_SETPOINT);

            Vector3D acceleration = {sinf(Generator::angle), cosf(Generator::angle), sinf(Generator::angle)/cosf(Generator::angle)};
            send(acceleration, Transfer::ID::TELEMETRY_SENSOR_ACCELERATION);

            Vector3D gyration = {sinf(Generator::angle), sinf(Generator::angle)/cosf(Generator::angle), cosf(Generator::angle)};
            send(gyration, Transfer::ID::TELEMETRY_SENSOR_GYRATION);

            Vector3D magnitude = {sinf(Generator::angle)/cosf(Generator::angle), cosf(Generator::angle), sinf(Generator::angle)};
            send(magnitude, Transfer::ID::TELEMETRY_SENSOR_MAGNETIC_FIELD);

            auto voltage = voltage_generator.get_sin();
            send(voltage, Transfer::TELEMETRY_SENSOR_VOLTAGE);

            auto current = current_generator.get_sin();
            send(current, Transfer::TELEMETRY_SENSOR_CURRENT);

            auto battery_level = battery_level_generator.get_sin();
            send(battery_level, Transfer::TELEMETRY_ESTIMATION_BATTERY_LEVEL);

            auto distance = distace_generator.get_sin();
            send(distance, Transfer::TELEMETRY_SENSOR_DISTANCE);

            auto altitude = distace_generator.get_sin() + 0.2;
            send(altitude, Transfer::TELEMETRY_ESTIMATION_ALTITUDE);

            auto preasure = preasure_generator.get_sin();
            send(preasure, Transfer::TELEMETRY_SENSOR_PRESSURE);

            float trottle = trottle_generator.get_cos();
            send(trottle, Transfer::CONTROL_MOTOR_THROTTLE);

            float angle_rad[4] = {servo_generators[0].get_sin(), servo_generators[1].get_sin(), servo_generators[2].get_sin(), servo_generators[3].get_sin()};
            Transfer::FrameTX tx = Transfer::encode(angle_rad, 4*sizeof(float), Transfer::TELEMETRY_INPUTS_SERVOS);
            for (std::size_t i = 0; i < tx.length; ++i) {
                serial.WriteByte(tx.buffer[i]);
                serial.DrainWriteBuffer();
            }
            usleep(100000);
            Generator::update();
            ++i;
        }
    } catch (const LibSerial::OpenFailed &) {
        std::cerr << "The serial port did not open correctly." << std::endl;
        serial.Close();
        return EXIT_FAILURE;
    }
}
