#include "transport.h"

Transport::Transport() :
	sensor_queue("sensor readings"),
	wire_tx_queue("wire tx queue"),
	wireless_tx_queue("wireless tx queue"),
	wireless_rx_queue("wireless rx queue") {

}

Transport & Transport::getInstance() {
	static Transport transport;

	return transport;
}
