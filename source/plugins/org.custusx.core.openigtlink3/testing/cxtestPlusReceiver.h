#ifndef PLUSRECEIVER_H
#define PLUSRECEIVER_H
#include "cxtest_org_custusx_core_openigtlink3_export.h"

#include "catch.hpp"

#include "cxtestReceiver.h"

namespace cxtest
{

class CXTEST_ORG_CUSTUSX_CORE_OPENIGTLINK3_EXPORT PlusReceiver : public Receiver
{
public:
	PlusReceiver(igtlio::LogicPointer logic);

	void send_RequestChannelIds();
	void send_RequestDeviceIds();
	void send_RequestDeviceChannelIds();
	void send_RequestInputDeviceIds();

	void send_RequestDepthAndGain();

};

} //namespace cxtest

#endif // PLUSRECEIVER_H
