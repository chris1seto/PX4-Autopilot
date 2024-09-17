#include "AeroprobeHyperion.hpp"

extern orb_advert_t mavlink_log_pub;

AeroprobeHyperion::AeroprobeHyperion()
:
	ScheduledWorkItem(MODULE_NAME, px4::wq_configurations::lp_default),
  parser_()
{
}

int AeroprobeHyperion::start()
{
  ScheduleNow();
	return PX4_OK;
}

AeroprobeHyperion::~AeroprobeHyperion()
{
}

void AeroprobeHyperion::Run()
{
	if (should_exit())
  {
    PX4_WARN("Exit");

		if (_uart)
    {
			(void) _uart->close();
			delete _uart;
			_uart = nullptr;
		}

    ScheduleClear();
		exit_and_cleanup();
		return;
	}

	if (!_initialized)
  {
    PX4_INFO("Start");

    // Create the UART port instance
    _uart = new Serial("/dev/ttyS3");

    if (_uart == nullptr)
    {
      PX4_ERR("Serial");
      return;
    }

    if (!_uart->isOpen())
    {
      // Configure the desired baudrate if one was specified by the user.
      // Otherwise the default baudrate will be used.
      if (! _uart->setBaudrate(AEROPROBE_HYPERION_BAUDRATE))
      {
        PX4_ERR("setBaudrate");
        return;
      }

      // Open the UART. If this is successful then the UART is ready to use.
      if (! _uart->open())
      {
        PX4_ERR("open");
        return;
      }

      _uart->flush();
    }

		_initialized = true;
	}

  uint32_t free_parse_size = _parser.FreeParseBufferSize();
  
  if (free_parse_size > SERIAL_READ_BUFFER_SIZE)
  {
    free_parse_size = SERIAL_READ_BUFFER_SIZE;
  }

	// Read all available data from the serial RC input UART
	int new_bytes = _uart->readAtLeast(&serial_read_buffer_[0], free_parse_size, 1, 100);
  
  if (new_bytes > 0)
  {
    _parser.LoadData(&serial_read_buffer_, new_bytes);
  }
  
  if (_parser.Parse(&_new_aeroprobe_message))
  {
  }

  ScheduleDelayed(100_ms);
}

int AeroprobeHyperion::task_spawn(int argc, char *argv[])
{
	AeroprobeHyperion *instance = new AeroprobeHyperion();

	if (!instance)
  {
		PX4_ERR("!instance");
		return PX4_ERROR;
	}

	_object.store(instance);
	_task_id = task_id_is_work_queue;

	instance->ScheduleNow();

	return PX4_OK;
}

int AeroprobeHyperion::print_usage(const char *reason)
{
	if (reason)
  {
		printf("%s\n\n", reason);
	}

	PRINT_MODULE_DESCRIPTION(
		R"DESCR_STR(
### Description

)DESCR_STR");

	PRINT_MODULE_USAGE_NAME("aueroprobe_hyperion", "system");
  PRINT_MODULE_USAGE_COMMAND("start");
	PRINT_MODULE_USAGE_DEFAULT_COMMANDS();

	return 0;
}

int AeroprobeHyperion::custom_command(int argc, char *argv[])
{
	if (!is_running())
  {
		PX4_INFO("not running");
		return PX4_ERROR;
	}

	return print_usage("Unrecognized command.");
}

extern "C" __EXPORT int aeroprobe_hyperion_main(int argc, char *argv[])
{
	return AeroprobeHyperion::main(argc, argv);
}