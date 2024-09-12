#include "SensataBms.hpp"

static uint32_t Unpack32Le(const uint8_t** data);
static uint16_t Unpack16Le(const uint8_t** data);
static uint8_t Unpack8(const uint8_t** data);

static uint32_t Unpack32Le(const uint8_t** data)
{
  uint32_t word = 0;
  word |= **data;
  (*data)++;
  word |= **data << 8;
  (*data)++;
  word |= **data << 16;
  (*data)++;
  word |= **data << 24;
  (*data)++;
  return word;
}

static uint16_t Unpack16Le(const uint8_t** data)
{
  uint16_t word = 0;
  word |= **data;
  (*data)++;
  word |= **data << 8;
  (*data)++;
  return word;
}

static uint8_t Unpack8(const uint8_t** data)
{
  uint8_t word = 0;
  word |= **data;
  (*data)++;
  return word;
}

SensataBms::SensataBms()
{
}

SensataBms::~SensataBms()
{
}

bool SensataBms::ProcessFrame(struct can_frame& frame)
{
  uint32_t frame_index;
  uint32_t pack_index;
  BmsPackData* pack_instance;
  bool frame_consumed = false;

  uint32_t frame_id = frame.can_id & CAN_EFF_MASK;

  if (frame_id < BMS_FRAME_ID_BASE
    || frame_id > BMS_FRAME_ID_BASE + (BMS_FRAME_ID_SPACING * BMS_FRAME_COUNT))
  {
    return false;
  }

  /*
    pack_index is 0..PARALLEL_PACK_COUNT
    frame_index is 1 indexed, to keep consistency with the BMS user manual
  */
  pack_index = (frame_id - BMS_FRAME_ID_BASE) % BMS_FRAME_ID_SPACING;
  frame_index = ((frame_id - BMS_FRAME_ID_BASE - pack_index) / BMS_FRAME_ID_SPACING) + 1;

  if (pack_index > PARALLEL_PACK_COUNT)
  {
    return false;
  }

  pack_instance = &_packs_data[pack_index];

  switch (frame_index)
  {
    case 1:
      break;

    case 6:
      UnpackFrame6(pack_instance, frame.data, frame.can_dlc);
      frame_consumed = true;
      break;

    case 7:
      UnpackFrame7(pack_instance, frame.data, frame.can_dlc);
      frame_consumed = true;
      break;

    case 8:
      UnpackFrame8(pack_instance, frame.data, frame.can_dlc);
      frame_consumed = true;
      break;

    case 9:
      UnpackFrame9(pack_instance, frame.data, frame.can_dlc);
      frame_consumed = true;
      break;

    case 10:
      UnpackFrame10(pack_instance, frame.data, frame.can_dlc);
      frame_consumed = true;
      break;

    case 11:
      UnpackFrame11(pack_instance, frame.data, frame.can_dlc);
      frame_consumed = true;
      break;

    default:
      break;
  }

  return frame_consumed;
}

void SensataBms::UnpackFrame6(BmsPackData* const pack_instance, const uint8_t* data, const uint32_t dlc)
{
  /*
    Items
      1 CELL_V_MIN_VAL UINT16 0,1 mV Lowest cell voltage
      4 CELL_V_MAX_VAL UINT16 0,1 mV Highest cell voltage
      11 PACK_V_SUM_OF_CELLS UINT16 0,1 V Sum of all cells
      19 SOC_TRIMMED UINT16 0,01 % Trimmed SoC
  */
  const uint8_t* parse_ptr = data;

  if (pack_instance == NULL)
  {
    return;
  }

  if (dlc != 8)
  {
    return;
  }

  pack_instance->heard_frame_bits |= FRAME_6;
  pack_instance->min_cell_voltage_v = static_cast<float>(Unpack16Le(&parse_ptr)) * 0.0001f;
  pack_instance->max_cell_voltage_v = static_cast<float>(Unpack16Le(&parse_ptr)) * 0.0001f;
  pack_instance->voltage_v = static_cast<float>(Unpack16Le(&parse_ptr)) * 0.1f;
  pack_instance->trimmed_soc_pct = static_cast<float>(Unpack16Le(&parse_ptr)) * 0.01f;

  /*PX4_INFO("F6: min cell %f, max cell %f, voltage %f, soc %f",
    (double)pack_instance->min_cell_voltage_v,
    (double)pack_instance->min_cell_voltage_v,
    (double)pack_instance->voltage_v,
    (double)pack_instance->trimmed_soc_pct);*/
}

void SensataBms::UnpackFrame7(BmsPackData* const pack_instance, const uint8_t* data, const uint32_t dlc)
{
  /*
    Items
      1081 SOH UINT16 0,01 % SoH
  */

  const uint8_t* parse_ptr = data;

  if (pack_instance == NULL)
  {
    return;
  }

  if (dlc != 2)
  {
    return;
  }

  pack_instance->heard_frame_bits |= FRAME_7;
  pack_instance->soh_pct = static_cast<float>(Unpack16Le(&parse_ptr)) * 0.01f;

  //PX4_INFO("F7 soh %f", (double)pack_instance->soh_pct);
}

void SensataBms::UnpackFrame8(BmsPackData* const pack_instance, const uint8_t* data, const uint32_t dlc)
{
  /*
    Items
      3002 PACK_RESISTANCE_U UINT32 0,1 μohm Calculated Pack Resistance
      16 PACK_I_MASTER INT32 0,01 mA Selected current source (shunt or hall) will be the system reference current.
  */

  const uint8_t* parse_ptr = data;

  if (pack_instance == NULL)
  {
    return;
  }

  if (dlc != 8)
  {
    return;
  }

  pack_instance->heard_frame_bits |= FRAME_8;
  pack_instance->resistance_r = static_cast<float>(Unpack32Le(&parse_ptr)) * 0.01f;
  pack_instance->current_a = static_cast<float>(Unpack32Le(&parse_ptr)) * 0.0001f;

  //PX4_INFO("F8 res %f, current_a %f", (double)pack_instance->resistance_r, (double)pack_instance->current_a);
}

void SensataBms::UnpackFrame9(BmsPackData* const pack_instance, const uint8_t* data, const uint32_t dlc)
{
  /*
    Items
      138 MCU_T1 INT8 1 °C AUX temperature 0
      139 MCU_T2 INT8 1 °C AUX temperature 1
      140 MCU_T3 INT8 1 °C AUX temperature 2
      141 MCU_T4 INT8 1 °C AUX temperature 3
      72 CMUS_CMU1_BALANCE_T 1 INT8 1 °C CMU balance circuit temperature. 121 = OC, -41 = SC
      73 CMUS_CMU1_BALANCE_T 2 INT8 1 °C CMU balance circuit temperature. 121 = OC, -41 = SC
  */

  uint32_t i;
  const uint8_t* parse_ptr = data;

  if (pack_instance == NULL)
  {
    return;
  }

  if (dlc != 6)
  {
    return;
  }

  pack_instance->heard_frame_bits |= FRAME_9;

  for (i = 0; i < PACK_TEMP_SENSOR_COUNT; i++)
  {
    pack_instance->pack_temp_sensors_c[i] = static_cast<float>(Unpack8(&parse_ptr));
  }

  for (i = 0; i < CMS_TEMP_SENSOR_COUNT; i++)
  {
    pack_instance->cms_temps_c[i] = static_cast<float>(Unpack8(&parse_ptr));
  }

  //PX4_INFO("F9 temp %f, cms 1 %f, cms %f", (double)pack_instance->pack_temp_sensors_c[0], (double)pack_instance->cms_temps_c[0], (double)pack_instance->cms_temps_c[1]);
}

void SensataBms::UnpackFrame10(BmsPackData* const pack_instance, const uint8_t* data, const uint32_t dlc)
{
  /*
    Items
      20001 GPIO_INPUT1 UINT8 1 - Reflects the digital data on the given input.
        1 bit
      973 BALANCING_BALANCE_SE TTING_CMU1_CELL_BITM ASK UINT32 1 - Cells being balanced as bitmask (b0 = cell 0, ... b11 = cell 11, b12-b15 = always zero)
        24 bits
  */
  //const uint8_t* parse_ptr = data;

  if (pack_instance == NULL)
  {
    return;
  }

  if (dlc != 4)
  {
    return;
  }

  pack_instance->heard_frame_bits |= FRAME_10;
}

void SensataBms::UnpackFrame11(BmsPackData* const pack_instance, const uint8_t* data, const uint32_t dlc)
{
  /*
    Items

  */
  //const uint8_t* parse_ptr = data;

  if (pack_instance == NULL)
  {
    return;
  }

  if (dlc != 4)
  {
    return;
  }

  pack_instance->heard_frame_bits |= FRAME_11;
}

void SensataBms::PublishBatteryStatus()
{
  float voltage_accumulator_v = 0;
  float soc_accumulator_pct = 0;
  float soh_accumulator_pct = 0;
  float total_current_a = 0;
  uint32_t cycle_count_accumulator = 0;
  uint32_t i;
  bool missing_pack = false;
  uint32_t online_pack_count = 0;

  for (i = 0; i < PARALLEL_PACK_COUNT; i++)
  {
    if (!_packs_data[i].online)
    {
      missing_pack = true;
      continue;
    }

    voltage_accumulator_v += _packs_data[i].voltage_v;
    total_current_a += _packs_data[i].current_a;
    soc_accumulator_pct += _packs_data[i].trimmed_soc_pct;
    soh_accumulator_pct += _packs_data[i].soh_pct;
    cycle_count_accumulator += _packs_data[i].cycle_count;
    online_pack_count++;;
  }

  battery_status_s battery_status = {};
  battery_status.timestamp = hrt_absolute_time();
  battery_status.connected = !missing_pack;
  battery_status.cell_count = CELL_COUNT;
  battery_status.serial_number = 0;
  battery_status.id = 0;
  battery_status.cycle_count = cycle_count_accumulator / online_pack_count;
  battery_status.state_of_health = soh_accumulator_pct / static_cast<float>(online_pack_count);
  battery_status.voltage_v = voltage_accumulator_v / static_cast<float>(online_pack_count);
  battery_status.current_a = total_current_a;
  battery_status.current_average_a = total_current_a;
  battery_status.remaining = soc_accumulator_pct / static_cast<float>(online_pack_count);
  battery_status.temperature = static_cast<float>(0);
  battery_status.capacity = 0;
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

  _battery_status_pub.publish(battery_status);
}

void SensataBms::PublishPacksStatus()
{
}

void SensataBms::Update()
{
  uint32_t i;

  if (HrtHelper_IsDue(_last_frame_check_time, FRAME_CHECK_PERIOD))
  {
    // For each parallel pack check the frames seen mask
    for (i = 0; i < PARALLEL_PACK_COUNT; i++)
    {
      if (_packs_data[i].heard_frame_bits == 0)
      {
        if (_packs_data[i].online)
        {
          PX4_INFO("Pack %lu now offline!", i);
          _packs_data[i].online = false;
        }
      }
      else if (_packs_data[i].heard_frame_bits != FRAMES_MASK)
      {
        PX4_INFO("Pack %lu: Unexpected frame heard bits", i);
      }
      else
      {
        if (!_packs_data[i].online)
        {
          PX4_INFO("Pack %lu now online!", i);
          _packs_data[i].online = true;
        }
      }

      // Reset heard bits
      _packs_data[i].heard_frame_bits = 0;
    }
  }

  if (HrtHelper_IsDue(_last_battery_status_publish_time, BATTERY_STATUS_PUBLISH_PERIOD))
  {
    PublishBatteryStatus();
  }

  if (HrtHelper_IsDue(_last_pack_status_publish_time, PACK_STATUS_PUBLISH_PERIOD))
  {
    PublishPacksStatus();
  }
}
