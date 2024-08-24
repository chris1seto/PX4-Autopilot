#include <radical_canbus.hpp>

#include <math.h>
#include <px4_platform_common/getopt.h>
#include <px4_platform_common/log.h>
#include <px4_platform_common/posix.h>
#include <uORB/topics/parameter_update.h>
#include <lib/drivers/device/Device.hpp>

RadicalCanbus::RadicalCanbus() :
	ScheduledWorkItem(MODULE_NAME, px4::wq_configurations::uavcan),
	iface_(nullptr),
	canInitHelper_(nullptr)
{
}

RadicalCanbus::~RadicalCanbus()
{
}

bool RadicalCanbus::init()
{
	ScheduleOnInterval(10_ms);
	return true;
}

void RadicalCanbus::Run()
{
	if (should_exit()) {
		ScheduleClear();
		exit_and_cleanup();
		return;
	}

	if (!_is_initialized) {
		_is_initialized = true;

		int32_t iface_index;
		param_get(param_find("RADICAL_CANBUS_CAN_IDX"), &iface_index);

		device::Device::DeviceId device_id;
		device_id.devid_s.bus_type = device::Device::DeviceBusType_UNKNOWN;

		// Select only the desired interface
		canInitHelper_ = new UAVCAN_DRIVER::CanInitHelper<kRxQueueCapacity>(1 << iface_index);

		int32_t bitrate;
		param_get(param_find("RADICAL_CANBUS_BITRATE"), &bitrate);
		int ret = canInitHelper_->init((uint32_t) bitrate, false);

		if (ret < 0) {
			PX4_ERR("CAN driver init failed with code %i", ret);
			return;
		}

		iface_ = canInitHelper_->driver.getIface((uint8_t)iface_index);

		PX4_INFO("Startup done");
	}

	uavcan::CanFrame received_frame{};

	while (receive(&received_frame) > 0)
  {
    // received_frame.id
	}
}

int16_t RadicalCanbus::receive(uavcan::CanFrame *received_frame)
{
	uavcan::MonotonicTime out_ts_monotonic;
	uavcan::UtcTime out_ts_utc;
	uavcan::CanIOFlags out_flags = 0;
	int16_t rx_res = iface_->receive(*received_frame, out_ts_monotonic, out_ts_utc, out_flags);

	return rx_res;
}

int RadicalCanbus::task_spawn(int argc, char *argv[])
{
	RadicalCanbus *instance = new RadicalCanbus();

	if (instance)
  {
		_object.store(instance);
		_task_id = task_id_is_work_queue;

		if (instance->init())
    {
			return PX4_OK;
		}

	}
  else
  {
		PX4_ERR("alloc failed");
	}

	delete instance;
	_object.store(nullptr);
	_task_id = -1;

	return PX4_ERROR;
}

int RadicalCanbus::custom_command(int argc, char *argv[])
{
	return 0;
}

int RadicalCanbus::print_usage(const char *reason)
{
	PX4_INFO("Radical Canbus [start | stop | status]");
	PX4_INFO("Note: Module may be started in rc.sensors automatically according to RADICAL_CANBUS_ENABLE");

	return 0;
}

extern "C" __EXPORT int radical_canbus_main(int argc, char *argv[])
{
	return RadicalCanbus::main(argc, argv);
}