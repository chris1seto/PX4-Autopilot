//#include "SensataBms.hpp"

static uint16_t Unpack16Le(const uint8_t** data);
static uint64_t TimeNow(void);

static uint64_t TimeNow(void)
{
	struct timespec ts {};

	clock_gettime(CLOCK_MONOTONIC, &ts);

	return ts.tv_sec * 1000000ULL + ts.tv_nsec / 1000ULL;
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

bool SensataBms::ProcessFrame(struct canfd_frame* frame)
{
  uint32_t frame_type;
  uint32_t pack_index;
  BmsPackData* pack_instance;

  if (frame->can_id < BMS_FRAME_ID_BASE
    || frame->can_id > BMS_FRAME_ID_BASE + (BMS_FRAME_ID_SPACING * BMS_FRAME_COUNT))
  {
    return false;
  }

  pack_index = (frame->can_id - BMS_FRAME_ID_BASE) % BMS_FRAME_ID_SPACING;
  frame_type = (frame->can_id - BMS_FRAME_ID_BASE - pack_index) / BMS_FRAME_ID_SPACING;

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
      this->ParseFrame6(pack_instance, frame->data, frame->can_dlc);
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
  uint8_t* parse_ptr = data;

  if (pack_instance == NULL)
  {
    return;
  }

  if (dlc != 8)
  {
    return;
  }

  pack_instance->min_cell_voltage_v = Unpack16Le(&parse_ptr) * 0.001f;
  pack_instance->max_cell_voltage_v = Unpack16Le(&parse_ptr) * 0.001f;
  pack_instance->voltage_v = Unpack16Le(&parse_ptr) * 0.1f;
  pack_instance->trimmed_soc_pct = Unpack16Le(&parse_ptr) * 0.01f;
}

void SensataBms::UnpackFrame7(BmsPackData* const pack_instance, const uint8_t* data, const uint32_t dlc)
{
  /*
    Items
      1081 SOH UINT16 0,01 % SoH
  */

  uint8_t* parse_ptr = data;

  if (pack_instance == NULL)
  {
    return;
  }

  if (dlc != 2)
  {
    return;
  }
}

void SensataBms::UnpackFrame8(BmsPackData* const pack_instance, const uint8_t* data, const uint32_t dlc)
{
  /*
    Items
      3002 PACK_RESISTANCE_U UINT32 0,1 μohm Calculated Pack Resistance
      16 PACK_I_MASTER INT32 0,01 mA Selected current source (shunt or hall) will be the system reference current.
  */

  uint8_t* parse_ptr = data;

  if (pack_instance == NULL)
  {
    return;
  }

  if (dlc != 8)
  {
    return;
  }
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

  uint8_t* parse_ptr = data;

  if (pack_instance == NULL)
  {
    return;
  }

  if (dlc != 6)
  {
    return;
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
  uint8_t* parse_ptr = data;

  if (pack_instance == NULL)
  {
    return;
  }

  if (dlc != 4)
  {
    return;
  }
}

void SensataBms::UnpackFrame11(BmsPackData* const pack_instance, const uint8_t* data, const uint32_t dlc)
{
  /*
    Items

  */
  uint8_t* parse_ptr = data;

  if (pack_instance == NULL)
  {
    return;
  }

  if (dlc != 4)
  {
    return;
  }
}

void SensataBms::Update()
{
}
