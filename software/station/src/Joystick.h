#pragma once

#include <string>
#include <vector>
#include <thread>

namespace utils {

class Joystick {
public:
    struct Info {
    public:
        std::string name;
        std::string node;
        std::string physical;
        std::string driverVersion;
        int numAxes;
        int numButtons;
    };

    Joystick(const Info &info);
    ~Joystick();
    double getAxis(const int id) const;
    bool getButton(const int id) const;
    bool isAvailable() const;
    const Info & getInfo() const;

    static std::vector<Info> getAvailable();

private:
    int fd;
    std::thread thread;
    bool thread_active;

    Info info;
    std::vector<double> axes;
    std::vector<bool> buttons;

    void eventLoop();
};

}
