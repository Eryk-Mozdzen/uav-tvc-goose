#include "transport.h"

Transport::Transport() : sensor_queue("sensor readings"), tx_queue("transmitter queue") {

}

Transport & Transport::getInstance() {
	static Transport transport;

	return transport;
}
