#include <frame_rx.h>
#include <frame_tx.h>
#include <libserial/SerialPort.h>
#include <transfer.h>

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <unistd.h>

constexpr const char* const SERIAL_PORT_2 = "/dev/pts/5";

int main(int argc, char** argv) {
    LibSerial::SerialPort serial_port;

    try {
        // Open the Serial Port at the desired hardware port.
        serial_port.Open(SERIAL_PORT_2, std::ios_base::out);
    } catch (const LibSerial::OpenFailed&) {
        std::cerr << "The serial port did not open correctly." << std::endl;
        return EXIT_FAILURE;
    }

    serial_port.SetBaudRate(LibSerial::BaudRate::BAUD_115200);
    serial_port.SetCharacterSize(LibSerial::CharacterSize::CHAR_SIZE_8);
    serial_port.SetFlowControl(LibSerial::FlowControl::FLOW_CONTROL_NONE);
    serial_port.SetParity(LibSerial::Parity::PARITY_NONE);
    serial_port.SetStopBits(LibSerial::StopBits::STOP_BITS_1);

    std::cout << "Writing input file contents to the serial port." << std::endl;

    Transfer transfer;
    LibSerial::DataBuffer buffer = {1, 2, 3, 4};
    while (1) {
        serial_port.Write(buffer);
        serial_port.DrainWriteBuffer();
        sleep(1);
    }

    serial_port.Close();
    std::cout << "The example program successfully completed!" << std::endl;
    return EXIT_SUCCESS;
}