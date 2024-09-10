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
  uint32_t frame_type;
  uint32_t pack_index;
  BmsPackData* pack_instance;

  if (frame.can_id < BMS_FRAME_ID_BASE
    || frame.can_id > BMS_FRAME_ID_BASE + (BMS_FRAME_ID_SPACING * BMS_FRAME_COUNT))
  {
    return false;
  }

  pack_index = (frame.can_id - BMS_FRAME_ID_BASE) % BMS_FRAME_ID_SPACING;
  frame_type = (frame.can_id - BMS_FRAME_ID_BASE - pack_index) / BMS_FRAME_ID_SPACING;

  if (pack_index > PARALLEL_PACK_COUNT)
  {
    return false;
  }

  pack_instance = &this->packs_data[pack_index];

  switch (frame_type)
  {
    case 0:
      break;

    case 6:
      this->UnpackFrame6(pack_instance, frame.data, frame.can_dlc);
      break;
  }


  return true;
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
  pack_instance->min_cell_voltage_v = static_cast<float>(Unpack16Le(&parse_ptr)) * 0.001f;
  pack_instance->max_cell_voltage_v = static_cast<float>(Unpack16Le(&parse_ptr)) * 0.001f;
  pack_instance->voltage_v = static_cast<float>(Unpack16Le(&parse_ptr)) * 0.1f;
  pack_instance->trimmed_soc_pct = static_cast<float>(Unpack16Le(&parse_ptr)) * 0.01f;
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

void SensataBms::PublishBatteryMonitor()
{
  float voltage_accumulator_v = 0;
  float soc_accumulator_pct = 0;
  float soh_accumulator_pct = 0;
  float total_current_a = 0;
  //float avg_soc_pct;
  uint32_t i;

  for (i = 0; i < PARALLEL_PACK_COUNT; i++)
  {
    voltage_accumulator_v += packs_data[i].voltage_v;
    total_current_a += packs_data[i].current_a;
    soc_accumulator_pct += packs_data[i].trimmed_soc_pct;
    soh_accumulator_pct += packs_data[i].soh_pct;
  }

  battery_status_s battery_status = {};
  battery_status.timestamp = hrt_absolute_time();
  battery_status.connected = true;
  battery_status.cell_count = CELL_COUNT;
  battery_status.serial_number = 0;
  battery_status.id = 0;
  battery_status.cycle_count = 0;
  battery_status.state_of_health = soh_accumulator_pct / static_cast<float>(PARALLEL_PACK_COUNT);
  battery_status.voltage_v = voltage_accumulator_v / static_cast<float>(PARALLEL_PACK_COUNT);
  battery_status.current_a = total_current_a;
  battery_status.current_average_a = total_current_a;
  battery_status.remaining = soc_accumulator_pct / static_cast<float>(PARALLEL_PACK_COUNT);
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

void SensataBms::Update()
{
  uint32_t i;

  if (HrtHelper_IsDue(last_frame_check_time, FRAME_CHECK_PERIOD))
  {
    // For each parallel pack check the frames seen mask
    for (i = 0; i < PARALLEL_PACK_COUNT; i++)
    {
      if (packs_data[i].heard_frame_bits != FRAMES_MASK)
      {
        PX4_INFO("Unexpected frame heard bits");
      }
    }
  }

  if (HrtHelper_IsDue(last_battery_monitor_publish_time, BATTERY_MONITOR_PUBLISH_PERIOD))
  {
    PublishBatteryMonitor();
  }
}
