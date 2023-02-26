#include "QueueCPP.h"

#include "queue_element.h"

Queue<queue_element_t, 16> sensor_queue("sensor");
Queue<queue_element_t, 16> tx_queue("TX");
