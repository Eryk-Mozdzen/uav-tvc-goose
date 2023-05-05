#include "transport.h"

Transport::Transport() :
	sensor_queue("sensor readings"),
	wire_rx_queue("wire rx queue"),
	wire_tx_queue("wire tx queue"),
	wireless_tx_queue("wireless tx queue"),
	frame_rx_queue("main rx queue") {

}

Transport & Transport::getInstance() {
	static Transport transport;

	return transport;
}
