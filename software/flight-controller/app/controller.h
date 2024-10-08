#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "msg.h"

void controller_reset();
void controller_calculate(msg_frame_controller_t *controller);

#endif
