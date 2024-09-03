
#pragma once

#include <px4_platform_common/defines.h>
#include <px4_platform_common/module.h>
#include <px4_platform_common/module_params.h>
#include <px4_platform_common/posix.h>
#include <px4_platform_common/px4_work_queue/ScheduledWorkItem.hpp>
#include <uORB/SubscriptionInterval.hpp>
#include <uORB/PublicationMulti.hpp>
#include <uORB/Subscription.hpp>
#include <uORB/topics/parameter_update.h>
#include <uORB/topics/battery_status.h>

#include "../../uavcan_driver.hpp"

#include <nuttx/can/can.h>

using namespace time_literals;

extern "C" __EXPORT int radical_canbus_main(int argc, char *argv[]);

/**
 * RadicalCanbus radar altimeter driver
 */
class RadicalCanbus : public ModuleBase<RadicalCanbus>, public px4::ScheduledWorkItem
{
public:
	virtual ~RadicalCanbus();

	bool init();

	/** @see ModuleBase */
	static int task_spawn(int argc, char *argv[]);

	/** @see ModuleBase */
	static int custom_command(int argc, char *argv[]);

	/** @see ModuleBase */
	static int print_usage(const char *reason = nullptr);

protected:
	void Run() override;

private:
	RadicalCanbus();

	static RadicalCanbus *instance_;

	static constexpr unsigned kRxQueueCapacity = 10;
	uavcan::ICanIface *iface_;	///< CAN communication interface.
	UAVCAN_DRIVER::CanInitHelper<kRxQueueCapacity> *canInitHelper_;

	bool _is_initialized{false};

	/**
	 * @brief Read a single CANBus message
	 *
	 *
	 * @param received_frame pointer to the CanFrame where the incoming CANBus frame will be written to
	 *
	 * @return 1 on CanFrame read, 0 on no frame read
	 */
	int16_t receive(uavcan::CanFrame *received_frame);
  
  uORB::PublicationMulti<battery_status_s> _battery_status_pub{ORB_ID(battery_status)};
  //uORB::SubscriptionInterval _parameter_update_sub{ORB_ID(parameter_update), 1_s};
  
	/*DEFINE_PARAMETERS(
		(ParamBool<px4::params::RAD_CAN_ENABLE>) _param_rad_can_enable,
    (ParamBool<px4::params::RAD_CAN_BITRATE>) _param_rad_can_bitrate,
    (ParamBool<px4::params::RAD_CAN_BUS_IDX>) _param_rad_can_bus_idx
	)*/
};