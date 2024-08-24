// NOTE: for any new params, touch them in ROMFS/px4fmu_common/init.d/rc.sensors, radical canbus driver section

/**
 *
 *  0 - RADICAL_CANBUS disabled.
 *  1 - RADICAL_CANBUS enabled.
 *
 * @min 0
 * @max 1
 * @value 0 Disabled
 * @value 1 Enabled
 * @reboot_required true
 * @group RADICAL_CANBUS
 */
PARAM_DEFINE_INT32(RADICAL_CANBUS_ENABLE, 1);

/**
 * RADICAL_CANBUS CAN bus bitrate.
 *
 * @unit bit/s
 * @min 20000
 * @max 1000000
 * @reboot_required true
 * @group RADICAL_CANBUS
 */
PARAM_DEFINE_INT32(RADICAL_CANBUS_BITRATE, 1000000);

/**
 * RADICAL_CANBUS CAN interface selection.
 *
 * @min 0
 * @max 1
 * @reboot_required true
 * @group RADICAL_CANBUS
 */
PARAM_DEFINE_INT32(RADICAL_CANBUS_CAN_IDX, 0);
