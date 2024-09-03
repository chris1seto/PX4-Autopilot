#pragma once

#include <nuttx/can/can.h>

#include <px4_platform_common/module.h>
#include <px4_platform_common/px4_work_queue/ScheduledWorkItem.hpp>

#include <uORB/Publication.hpp>
#include <uORB/topics/battery_status.h>

using namespace time_literals;

class RadicalCan : public ModuleBase<RadicalCan>, public px4::ScheduledWorkItem
{
public:
	RadicalCan();

	virtual ~RadicalCan();

	static int print_usage(const char *reason = nullptr);
	static int custom_command(int argc, char *argv[]);

	static int task_spawn(int argc, char *argv[]);

	int start();

private:

	void Run() override;

	bool _initialized{false};

	uORB::Publication<battery_status_s> _battery_status_pub{ORB_ID::battery_status};
};
