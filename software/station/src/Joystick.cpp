#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <sstream>
#include <cstring>
#include <cassert>

#include <fcntl.h>
#include <unistd.h>
#include <linux/joystick.h>
#include <libudev.h>

#include "Joystick.h"

namespace utils {

Joystick::Joystick(const Info &info) : info{info} {
    assert(info.numAxes>0);
    assert(info.numButtons>0);
    assert(info.node.size()>0);

    axes.resize(info.numAxes, 0);
    buttons.resize(info.numButtons, false);

    fd = open(info.node.c_str(), O_RDONLY | O_NONBLOCK);
    if(fd<0) {
        return;
    }

    thread_active = true;
    thread = std::thread(&Joystick::eventLoop, this);
}

Joystick::~Joystick() {
    thread_active = false;
    if(thread.joinable()) {
        thread.join();
    }
}

void Joystick::eventLoop() {
    while(thread_active) {
        js_event event;

        if(read(fd, &event, sizeof(event))==sizeof(event)) {
            switch(event.type) {
                case JS_EVENT_AXIS: {
                    axes.at(event.number) = event.value/32768.;
                } break;
                case JS_EVENT_BUTTON: {
                    buttons.at(event.number) = event.value;
                } break;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

double Joystick::getAxis(const int id) const {
    return axes.at(id);
}

bool Joystick::getButton(const int id) const {
    return buttons.at(id);
}

bool Joystick::isAvailable() const {
    struct udev *udev = udev_new();
    if(!udev) {
        return false;
    }

    struct udev_enumerate *enumerate = udev_enumerate_new(udev);
    udev_enumerate_add_match_subsystem(enumerate, "input");
    udev_enumerate_scan_devices(enumerate);

    struct udev_list_entry *devices = udev_enumerate_get_list_entry(enumerate);
    struct udev_list_entry *entry;

    udev_list_entry_foreach(entry, devices) {
        const char *path = udev_list_entry_get_name(entry);
        struct udev_device *dev = udev_device_new_from_syspath(udev, path);

        const char *node = udev_device_get_devnode(dev);

        if(node) {
            if(strcmp(node, info.node.c_str())==0) {
                return true;
            }
        }

        udev_device_unref(dev);
    }

    udev_enumerate_unref(enumerate);
    udev_unref(udev);

    return false;
}

const Joystick::Info & Joystick::getInfo() const {
    return info;
}

std::vector<Joystick::Info> Joystick::getAvailable() {
    std::vector<Info> available;

    struct udev *udev = udev_new();
    if(!udev) {
        return available;
    }

    struct udev_enumerate *enumerate = udev_enumerate_new(udev);
    udev_enumerate_add_match_subsystem(enumerate, "input");
    udev_enumerate_scan_devices(enumerate);

    struct udev_list_entry *devices = udev_enumerate_get_list_entry(enumerate);
    struct udev_list_entry *entry;

    udev_list_entry_foreach(entry, devices) {
        const char *path = udev_list_entry_get_name(entry);
        struct udev_device *dev = udev_device_new_from_syspath(udev, path);

        const char *node = udev_device_get_devnode(dev);

        if(node) {
            if(strstr(node, "js")) {
                struct udev_device *parent = udev_device_get_parent_with_subsystem_devtype(dev, "input", NULL);

                if(parent) {
                    const char *name = udev_device_get_sysattr_value(parent, "name");
                    const char *phys = udev_device_get_sysattr_value(parent, "phys");

                    Info info;
                    info.name = name ? name : "unknown";
                    info.node = node;
                    info.physical = phys ? phys : "unknown";
                    info.driverVersion = "unknown";
                    info.numAxes = -1;
                    info.numButtons = -1;
                    available.push_back(info);
                }
            }
        }

        udev_device_unref(dev);
    }

    udev_enumerate_unref(enumerate);
    udev_unref(udev);

    for(Info &info : available) {
        int fd = open(info.node.c_str(), O_RDONLY);
        if(fd<0) {
            continue;
        }

        int version;
        if(ioctl(fd, JSIOCGVERSION, &version)<0) {
            close(fd);
            continue;
        }

        __u8 axes;
        if(ioctl(fd, JSIOCGAXES, &axes)<0) {
            close(fd);
            continue;
        }

        __u8 buttons;
        if(ioctl(fd, JSIOCGBUTTONS, &buttons)<0) {
            close(fd);
            continue;
        }

        char name[128];
        if(ioctl(fd, JSIOCGNAME(sizeof(name)), name)<0) {
            strncpy(name, "unknown", sizeof(name));
        }

        info.name = name;
        info.numAxes = axes;
        info.numButtons = buttons;

        std::stringstream ss;
        ss << (version >> 16) << ".";
        ss << ((version >> 8) & 0xFF) << ".";
        ss << (version & 0xFF);
        info.driverVersion = ss.str();

        close(fd);
    }

    return available;
}

}
