#pragma once

#include <stddef.h>
#include <stdint.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/socket.h>

#include <net/if.h>

#include <nuttx/can.h>
#include <nuttx/can/can.h>
#include <netpacket/can.h>

#include <drivers/drv_hrt.h>

#include <uORB/Publication.hpp>
#include <px4_platform_common/module.h>
#include <px4_platform_common/px4_work_queue/ScheduledWorkItem.hpp>

#include "SensataBms.hpp"

#include "HrtHelper.hpp"

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
  int _fd{-1};
  struct iovec       _recv_iov {};
	struct can_frame _recv_frame {};
	struct msghdr      _recv_msg {};
  uint8_t            _recv_control[sizeof(struct cmsghdr) + sizeof(struct timeval)] {};
  struct cmsghdr     *_recv_cmsg {};

	void Run() override;

	bool _initialized{false};

  SensataBms _bms{};
  
  static constexpr hrt_abstime BMS_UPDATE_PERIOD{5000_ms};
  hrt_abstime _last_bms_update_time{0};
};
