#include "communication_base.h"

CommunicationBase::CommunicationBase(ConcurrentQueue<Log> &logs, ConcurrentQueue<Telemetry> &telemetry, ConcurrentQueue<Control> &controls) :
		logs{logs}, telemetry{telemetry}, controls{controls} {

}
