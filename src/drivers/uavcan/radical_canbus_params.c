// NOTE: for any new params, touch them in ROMFS/px4fmu_common/init.d/rc.sensors, radical canbus driver section

/**
 *
 * Radical canbus enabled
 *
 * @min 0
 * @max 1
 * @value 0 Disabled
 * @value 1 Enabled
 * @reboot_required true
 * @group RAD_CAN
 */
PARAM_DEFINE_INT32(RAD_CAN_ENABLE, 1);

/**
 * Radical canbus bitrate
 *
 * @unit bit/s
 * @min 20000
 * @max 1000000
 * @reboot_required true
 * @group RAD_CAN
 */
PARAM_DEFINE_INT32(RAD_CAN_BITRATE, 1000000);

/**
 * Radical canbus CAN interface selection.
 *
 * @min 0
 * @max 1
 * @reboot_required true
 * @group RAD_CAN
 */
PARAM_DEFINE_INT32(RAD_CAN_BUS_IDX, 1);
