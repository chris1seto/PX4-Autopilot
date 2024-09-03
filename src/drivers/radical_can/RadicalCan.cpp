
#include "RadicalCan.hpp"

extern orb_advert_t mavlink_log_pub;

RadicalCan::RadicalCan() :
	ScheduledWorkItem(MODULE_NAME, px4::wq_configurations::uavcan)
{
}

RadicalCan::~RadicalCan()
{
}

void RadicalCan::Run()
{
	if (should_exit())
  {
		exit_and_cleanup();
		return;
	}

	if (!_initialized)
  {
		_initialized = true;
	}

		/*battery_status_s battery_status = {};
		battery_status.timestamp = hrt_absolute_time();
		battery_status.connected = true;
		battery_status.cell_count = 12;

		sprintf(battery_status.serial_number, "%d", tattu_message.manufacturer);
		battery_status.id = static_cast<uint8_t>(tattu_message.sku);

		battery_status.cycle_count = tattu_message.cycle_life;
		battery_status.state_of_health = static_cast<uint16_t>(tattu_message.health_status);

		battery_status.voltage_v = static_cast<float>(tattu_message.voltage) / 1000.0f;
		battery_status.current_a = static_cast<float>(tattu_message.current) / 1000.0f;
		battery_status.remaining = static_cast<float>(tattu_message.remaining_percent) / 100.0f;
		battery_status.temperature = static_cast<float>(tattu_message.temperature);
		battery_status.capacity = tattu_message.standard_capacity;
		battery_status.voltage_cell_v[0] = 0;
		battery_status.voltage_cell_v[1] = 0;
		battery_status.voltage_cell_v[2] = 0;
		battery_status.voltage_cell_v[3] = 0;
		battery_status.voltage_cell_v[4] = 0;
		battery_status.voltage_cell_v[5] = 0;
		battery_status.voltage_cell_v[6] = 0;
		battery_status.voltage_cell_v[7] = 0;
		battery_status.voltage_cell_v[8] = 0;
		battery_status.voltage_cell_v[9] = 0;
		battery_status.voltage_cell_v[10] = 0;
		battery_status.voltage_cell_v[11] = 0;

		_battery_status_pub.publish(battery_status);*/
}


int RadicalCan::start()
{
	return PX4_OK;
}

int RadicalCan::task_spawn(int argc, char *argv[])
{
	RadicalCan *instance = new RadicalCan();

	if (!instance)
  {
		PX4_ERR("driver allocation failed");
		return PX4_ERROR;
	}

	_object.store(instance);
	_task_id = task_id_is_work_queue;

	instance->start();
	return 0;
}

int RadicalCan::print_usage(const char *reason)
{
	if (reason)
  {
		printf("%s\n\n", reason);
	}

	PRINT_MODULE_DESCRIPTION(
		R"DESCR_STR(
### Description

)DESCR_STR");

	PRINT_MODULE_USAGE_NAME("radical_can", "system");
	PRINT_MODULE_USAGE_COMMAND("start");
	PRINT_MODULE_USAGE_DEFAULT_COMMANDS();

	return 0;
}

int RadicalCan::custom_command(int argc, char *argv[])
{
	if (!is_running())
  {
		PX4_INFO("not running");
		return PX4_ERROR;
	}

	return print_usage("Unrecognized command.");
}

extern "C" __EXPORT int radical_can_main(int argc, char *argv[])
{
	return RadicalCan::main(argc, argv);
}
