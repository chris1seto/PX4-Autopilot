#pragma once

#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <sys/time.h>

#include <px4_platform_common/px4_config.h>
#include <px4_platform_common/getopt.h>
#include <px4_platform_common/log.h>
#include <px4_platform_common/module.h>
#include <px4_platform_common/module_params.h>
#include <px4_platform_common/Serial.hpp>

#include <drivers/drv_hrt.h>

#include <uORB/Publication.hpp>
#include <px4_platform_common/module.h>
#include <px4_platform_common/px4_work_queue/ScheduledWorkItem.hpp>

#include "AeroprobeHyperionParser.hpp"
#include "HrtHelper.hpp"

using namespace time_literals;
using namespace device;

class AeroprobeHyperion : public ModuleBase<AeroprobeHyperion>, public px4::ScheduledWorkItem
{
public:
	AeroprobeHyperion();

	virtual ~AeroprobeHyperion();

	static int print_usage(const char *reason = nullptr);
	static int custom_command(int argc, char *argv[]);

	static int task_spawn(int argc, char *argv[]);

  int start();

private:
  Serial *_uart = nullptr;
  
  static constexpr uint32_t AEROPROBE_HYPERION_BAUDRATE{115200};
  
  static constexpr uint32_t SERIAL_READ_BUFFER_SIZE{100};
  uint8_t serial_read_buffer_[SERIAL_READ_BUFFER_SIZE];

	void Run() override;

	bool _initialized{false};
  
  AeroprobeHyperionParser _parser{};
  
  AeroprobeMessage _new_aeroprobe_message{};

};