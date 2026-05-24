#include <cmath>
#include <fstream>
#include <iostream>

#include <GLFW/glfw3.h>
#include <mujoco/mujoco.h>

// #define VISUALIZATION

static double sign(const double in) {
    return (in >= 0.) ? 1. : -1.;
}

static void controller(const mjModel *model, mjData *data) {
    const double pz = data->sensordata[model->sensor_adr[mj_name2id(model, mjOBJ_SENSOR, "rangefinder")]];
    const double vz = data->qvel[2];

    const double imu_gyro[3] = {
        data->sensordata[model->sensor_adr[mj_name2id(model, mjOBJ_SENSOR, "gyro")] + 0],
        data->sensordata[model->sensor_adr[mj_name2id(model, mjOBJ_SENSOR, "gyro")] + 1],
        data->sensordata[model->sensor_adr[mj_name2id(model, mjOBJ_SENSOR, "gyro")] + 2],
    };

    const double imu_accel[3] = {
        data->sensordata[model->sensor_adr[mj_name2id(model, mjOBJ_SENSOR, "accel")] + 0],
        data->sensordata[model->sensor_adr[mj_name2id(model, mjOBJ_SENSOR, "accel")] + 1],
        data->sensordata[model->sensor_adr[mj_name2id(model, mjOBJ_SENSOR, "accel")] + 2],
    };

    const double imu_mag[3] = {
        data->sensordata[model->sensor_adr[mj_name2id(model, mjOBJ_SENSOR, "mag")] + 0],
        data->sensordata[model->sensor_adr[mj_name2id(model, mjOBJ_SENSOR, "mag")] + 1],
        data->sensordata[model->sensor_adr[mj_name2id(model, mjOBJ_SENSOR, "mag")] + 2],
    };

    const double w1 = data->sensordata[model->sensor_adr[mj_name2id(model, mjOBJ_SENSOR, "omega1")]];
    const double w2 = data->sensordata[model->sensor_adr[mj_name2id(model, mjOBJ_SENSOR, "omega2")]];
    const double w3 = data->sensordata[model->sensor_adr[mj_name2id(model, mjOBJ_SENSOR, "omega3")]];
    const double w4 = data->sensordata[model->sensor_adr[mj_name2id(model, mjOBJ_SENSOR, "omega4")]];

    const double target_acceleration = (2 - pz) - 2 * vz;
    const double Fr = (1.04 * (target_acceleration + 9.81)) / 4;
    const double wr = sign(Fr) * std::sqrt(std::abs(Fr) / 0.000003133);
    const double torque1 = 0.1 * (+wr - w1) + 0.00000003133 * sign(w1) * w1 * w1;
    const double torque2 = 0.1 * (-wr - w2) + 0.00000003133 * sign(w2) * w2 * w2;
    const double torque3 = 0.1 * (+wr - w3) + 0.00000003133 * sign(w3) * w3 * w3;
    const double torque4 = 0.1 * (-wr - w4) + 0.00000003133 * sign(w4) * w4 * w4;

    data->ctrl[mj_name2id(model, mjOBJ_ACTUATOR, "motor1")] = torque1;
    data->ctrl[mj_name2id(model, mjOBJ_ACTUATOR, "motor2")] = torque2;
    data->ctrl[mj_name2id(model, mjOBJ_ACTUATOR, "motor3")] = torque3;
    data->ctrl[mj_name2id(model, mjOBJ_ACTUATOR, "motor4")] = torque4;

    data->ctrl[mj_name2id(model, mjOBJ_ACTUATOR, "propeller1")] = sign(w1) * w1 * w1;
    data->ctrl[mj_name2id(model, mjOBJ_ACTUATOR, "propeller2")] = sign(w2) * w2 * w2;
    data->ctrl[mj_name2id(model, mjOBJ_ACTUATOR, "propeller3")] = sign(w3) * w3 * w3;
    data->ctrl[mj_name2id(model, mjOBJ_ACTUATOR, "propeller4")] = sign(w4) * w4 * w4;
}

static mjModel *model = NULL;
static mjData *data = NULL;

#ifdef VISUALIZATION
static mjvCamera cam;
static mjvPerturb pert;
static mjvOption opt;
static mjvScene scn;
static mjrContext con;

static bool button_left = false;
static bool button_middle = false;
static bool button_right = false;
static double lastx = 0;
static double lasty = 0;

static void keyboard(GLFWwindow *window, int key, int scancode, int act, int mods) {
    (void)window;
    (void)scancode;
    (void)mods;
    if((act == GLFW_PRESS) && (key == GLFW_KEY_SPACE)) {
        mj_resetData(model, data);
        mj_forward(model, data);
    }
}

static void mouse_button(GLFWwindow *window, int button, int act, int mods) {
    (void)button;
    (void)act;
    (void)mods;

    button_left = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
    button_middle = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS);
    button_right = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS);

    glfwGetCursorPos(window, &lastx, &lasty);
}

static void mouse_move(GLFWwindow *window, double xpos, double ypos) {
    if(!button_left && !button_middle && !button_right) {
        return;
    }

    const double dx = xpos - lastx;
    const double dy = ypos - lasty;
    lastx = xpos;
    lasty = ypos;

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    const bool mod_shift = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
                            glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS);

    mjtMouse action;
    if(button_right) {
        action = mod_shift ? mjMOUSE_MOVE_H : mjMOUSE_MOVE_V;
    } else if(button_left) {
        action = mod_shift ? mjMOUSE_ROTATE_H : mjMOUSE_ROTATE_V;
    } else {
        action = mjMOUSE_ZOOM;
    }

    mjv_moveCamera(model, action, dx / height, dy / height, &scn, &cam);
}

static void scroll(GLFWwindow *window, double xoffset, double yoffset) {
    (void)window;
    (void)xoffset;
    mjv_moveCamera(model, mjMOUSE_ZOOM, 0, -0.05 * yoffset, &scn, &cam);
}
#endif

int main() {
    char error[256];
    model = mj_loadXML("../model.xml", nullptr, error, sizeof(error));
    if(model == nullptr) {
        std::cerr << error << std::endl;
        return -1;
    }

    data = mj_makeData(model);

    mjcb_control = controller;

#ifdef VISUALIZATION
    glfwInit();
    GLFWwindow *window = glfwCreateWindow(1200, 900, "Demo", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    mjv_defaultCamera(&cam);
    mjv_defaultPerturb(&pert);
    mjv_defaultOption(&opt);
    mjr_defaultContext(&con);

    mjv_makeScene(model, &scn, 1000);
    mjr_makeContext(model, &con, mjFONTSCALE_100);

    glfwSetKeyCallback(window, keyboard);
    glfwSetCursorPosCallback(window, mouse_move);
    glfwSetMouseButtonCallback(window, mouse_button);
    glfwSetScrollCallback(window, scroll);

    while(!glfwWindowShouldClose(window)) {
        const mjtNum start = data->time;
        while((data->time - start) < (1. / 60.)) {
            mj_step(model, data);
        }

        mjrRect viewport = {0, 0, 0, 0};
        glfwGetFramebufferSize(window, &viewport.width, &viewport.height);

        mjv_updateScene(model, data, &opt, NULL, &cam, mjCAT_ALL, &scn);
        mjr_render(viewport, &scn, &con);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    mjv_freeScene(&scn);
    mjr_freeContext(&con);
#else
    std::ofstream file("output.csv");
    file << "time,pz,vz,accel_z" << std::endl;

    while(data->time < 10) {
        mj_step(model, data);
		const double imu_accel[3] = {
			data->sensordata[model->sensor_adr[mj_name2id(model, mjOBJ_SENSOR, "accel")] + 0],
			data->sensordata[model->sensor_adr[mj_name2id(model, mjOBJ_SENSOR, "accel")] + 1],
			data->sensordata[model->sensor_adr[mj_name2id(model, mjOBJ_SENSOR, "accel")] + 2],
		};
		file << data->time << "," << data->qpos[2] << "," << data->qvel[2] << "," << imu_accel[2] << std::endl;
    }
#endif

    mj_deleteModel(model);
    mj_deleteData(data);
}
