#pragma once

#include <uORB/topics/battery_status.h>
#include <uORB/Publication.hpp>

#include <nuttx/can.h>
#include <nuttx/can/can.h>
#include <netpacket/can.h>

#include <drivers/drv_hrt.h>
#include <px4_platform_common/module.h>

#include "HrtHelper.hpp"

using namespace time_literals;

class SensataBms
{
public:
  SensataBms();
  ~SensataBms();

  bool ProcessFrame(struct can_frame& frame);
  void Update();

private:

  enum FrameBits : uint32_t {
    FRAME_1  = 1 << 0,
    FRAME_2  = 1 << 1,
    FRAME_3  = 1 << 2,
    FRAME_4  = 1 << 3,
    FRAME_5  = 1 << 4,
    FRAME_6  = 1 << 5,
    FRAME_7  = 1 << 6,
    FRAME_8  = 1 << 7,
    FRAME_9  = 1 << 8,
    FRAME_10 = 1 << 9,
    FRAME_11 = 1 << 10,
    FRAME_12 = 1 << 11,
    FRAME_13 = 1 << 12,
    FRAME_14 = 1 << 13,
    FRAME_15 = 1 << 14,
    FRAME_16 = 1 << 15,
    FRAME_17 = 1 << 16,
    FRAME_18 = 1 << 17,
    FRAME_19 = 1 << 18,
    FRAME_20 = 1 << 19
  };

  static constexpr uint32_t PACK_TEMP_SENSOR_COUNT{6};
  static constexpr uint32_t CMS_TEMP_SENSOR_COUNT{2};

  struct BmsPackData
  {
    // Meta
    uint32_t heard_frame_bits = 0;
    bool online = false;
    bool missing_frames = false;


    // Frame 6
    float min_cell_voltage_v;
    float max_cell_voltage_v;
    float voltage_v;
    float trimmed_soc_pct;

    // Frame 7
    float soh_pct;

    // Frame 8
    float resistance_r;
    float current_a;

    // Frame 9
    float pack_temp_sensors_c[PACK_TEMP_SENSOR_COUNT];
    float cms_temps_c[CMS_TEMP_SENSOR_COUNT];

    // Frame 10
    bool heater_on;
    uint32_t balancing_bits;

    // Frame 11
    uint32_t cycle_count;
    uint32_t config_crc;
  };

  void UnpackFrame6(BmsPackData* const pack_instance, const uint8_t* data, const uint32_t dlc);
  void UnpackFrame7(BmsPackData* const pack_instance, const uint8_t* data, const uint32_t dlc);
  void UnpackFrame8(BmsPackData* const pack_instance, const uint8_t* data, const uint32_t dlc);
  void UnpackFrame9(BmsPackData* const pack_instance, const uint8_t* data, const uint32_t dlc);
  void UnpackFrame10(BmsPackData* const pack_instance, const uint8_t* data, const uint32_t dlc);
  void UnpackFrame11(BmsPackData* const pack_instance, const uint8_t* data, const uint32_t dlc);

  uORB::Publication<battery_status_s> _battery_status_pub{ORB_ID::battery_status};

  static constexpr uint32_t BMS_FRAME_ID_BASE{1010};
  static constexpr uint32_t BMS_FRAME_ID_SPACING{10};
  static constexpr uint32_t BMS_FRAME_COUNT{20};
  static constexpr uint32_t PARALLEL_PACK_COUNT{6};
  static constexpr uint32_t CELL_COUNT{4};

  hrt_abstime last_frame_check_time{0};

  static constexpr hrt_abstime FRAME_CHECK_PERIOD{2000_ms};

  static constexpr hrt_abstime BATTERY_MONITOR_PUBLISH_PERIOD{1000_ms};
  hrt_abstime last_battery_monitor_publish_time{0};

  BmsPackData packs_data[PARALLEL_PACK_COUNT]{};

  void PublishBatteryMonitor();

  static constexpr uint32_t FRAMES_MASK{FRAME_6 | FRAME_7 | FRAME_8 | FRAME_9 | FRAME_10 | FRAME_11};
};
