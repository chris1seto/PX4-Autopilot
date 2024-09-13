uint64 timestamp			# time since system start (microseconds)

uint8 index

bool is_online
bool is_missing_frames

# Frame 6
float32 min_cell_voltage_v
float32 max_cell_voltage_v
float32 voltage_v
float32 trimmed_soc_pct

# Frame 7
float32 soh_pct

# Frame 8
float32 resistance_r
float32 current_a

# Frame 9
float32[1] pack_temps_c
float32[2] cms_temps_c

# Frame 10
bool heater_on
uint32 balancing_bits

# Frame 11
uint32 cycle_count
uint32 config_crc
