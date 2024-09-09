#pragma once

#include <uORB/topics/battery_status.h>
#include <uORB/Publication.hpp>

class SensataBms
{
public:
  SensataBms();
  ~SensataBms();

  void Update();

private:
  struct BmsPackData
  {
    // Meta
    uint64_t last_heard_time;
    bool timed_out;

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
    float temp_sensors_c[6];
    float cms_temps_c[2];

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

  static constexpr uint32_t BMS_FRAME_ID_BASE = 1010;
  static constexpr uint32_t BMS_FRAME_ID_SPACING = 10;
  static constexpr uint32_t BMS_FRAME_COUNT = 20;
  static constexpr uint32_t PARALLEL_PACK_COUNT = 6;

  BmsPackData pack_data[PARALLEL_PACK_COUNT];
};