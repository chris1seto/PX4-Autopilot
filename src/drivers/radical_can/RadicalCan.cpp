
#include "RadicalCan.hpp"

extern orb_advert_t mavlink_log_pub;

RadicalCan::RadicalCan() :
	ScheduledWorkItem(MODULE_NAME, px4::wq_configurations::lp_default)
{
}

int RadicalCan::start()
{
  ScheduleNow();
	return PX4_OK;
}

RadicalCan::~RadicalCan()
{
}

void RadicalCan::Run()
{
	if (should_exit())
  {
    PX4_WARN("Exit");
    ScheduleClear();
		exit_and_cleanup();
		return;
	}

	if (!_initialized)
  {
    PX4_INFO("Start");
    const char *const can_iface_name = "can1";

    struct sockaddr_can addr;
    struct ifreq ifr;

    // open socket
    if ((_fd = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0)
    {
      PX4_ERR("socket");
      return;
    }

    strncpy(ifr.ifr_name, can_iface_name, IFNAMSIZ - 1);
    ifr.ifr_name[IFNAMSIZ - 1] = '\0';
    ifr.ifr_ifindex = if_nametoindex(ifr.ifr_name);

    if (!ifr.ifr_ifindex)
    {
      PX4_ERR("if_nametoindex");
      return;
    }

    memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
      PX4_ERR("bind");
      return;
    }

    // Setup RX msg
    _recv_iov.iov_base = &_recv_frame;
    _recv_iov.iov_len = sizeof(struct can_frame);

    memset(_recv_control, 0x00, sizeof(_recv_control));

    _recv_msg.msg_iov = &_recv_iov;
    _recv_msg.msg_iovlen = 1;
    _recv_msg.msg_control = &_recv_control;
    _recv_msg.msg_controllen = sizeof(_recv_control);
    _recv_cmsg = CMSG_FIRSTHDR(&_recv_msg);

		_initialized = true;
	}

  while (recvmsg(_fd, &_recv_msg, MSG_DONTWAIT) >= 0)
  {
    if (_bms.ProcessFrame(_recv_frame))
    {
      continue;
    }
  }

  /*if (HrtHelper_IsDue(_last_bms_update_time, BMS_UPDATE_PERIOD_MS))
  {
    _bms.Update()
  }*/

  ScheduleDelayed(100_ms);
}

int RadicalCan::task_spawn(int argc, char *argv[])
{
	RadicalCan *instance = new RadicalCan();

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
