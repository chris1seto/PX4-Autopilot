#ifndef SENSATA_PACK_STATUS_HPP
#define SENSATA_PACK_STATUS_HPP

#include <uORB/topics/sensata_pack_status.h>

class MavlinkStreamSensataPackStatus : public MavlinkStream
{
public:
	static MavlinkStream *new_instance(Mavlink *mavlink) { return new MavlinkStreamSensataPackStatus(mavlink); }

	static constexpr const char *get_name_static() { return "SENSATA_PACK_STATUS"; }
	static constexpr uint16_t get_id_static() { return MAVLINK_MSG_ID_SENSATA_PACK_STATUS; }

	const char *get_name() const override { return get_name_static(); }
	uint16_t get_id() override { return get_id_static(); }

	unsigned get_size() override
	{
		static constexpr unsigned size_per_pack = MAVLINK_MSG_ID_SENSATA_PACK_STATUS_LEN + MAVLINK_NUM_NON_PAYLOAD_BYTES;
		return size_per_pack * _sensata_pack_status_subs.advertised_count();
	}

private:
	explicit MavlinkStreamSensataPackStatus(Mavlink *mavlink) : MavlinkStream(mavlink) {}

	uORB::SubscriptionMultiArray<sensata_pack_status_s, sensata_pack_status_s::MAX_INSTANCES> _sensata_pack_status_subs{ORB_ID::sensata_pack_status};

	bool send() override
	{
		bool updated = false;

		for (auto &sensata_pack_status_sub : _sensata_pack_status_subs)
    {
			sensata_pack_status_s sensata_pack_status;

			if (sensata_pack_status_sub.update(&sensata_pack_status))
      {
        PX4_INFO("Update %i", sensata_pack_status.index);
				mavlink_sensata_pack_status_t sensata_pack_status_msg{};

        sensata_pack_status_msg.index = sensata_pack_status.index;
        sensata_pack_status_msg.flag_bits = sensata_pack_status.flag_bits;
        sensata_pack_status_msg.min_cell_voltage = sensata_pack_status.min_cell_voltage_v;
        sensata_pack_status_msg.max_cell_voltage = sensata_pack_status.max_cell_voltage_v;
        sensata_pack_status_msg.voltage = sensata_pack_status.voltage_v;
        sensata_pack_status_msg.trimmed_soc = static_cast<uint8_t>(sensata_pack_status.trimmed_soc_pct);
        sensata_pack_status_msg.soh = static_cast<uint8_t>(sensata_pack_status.soh_pct);
        sensata_pack_status_msg.resistance = sensata_pack_status.resistance_r;
        sensata_pack_status_msg.current = sensata_pack_status.current_a;
        sensata_pack_status_msg.pack_temps[0] = static_cast<uint8_t>(sensata_pack_status.pack_temps_c[0]);
        sensata_pack_status_msg.cms_temps[0] = static_cast<uint8_t>(sensata_pack_status.cms_temps_c[0]);
        sensata_pack_status_msg.cms_temps[1] = static_cast<uint8_t>(sensata_pack_status.cms_temps_c[1]);
        sensata_pack_status_msg.balancing_bits = sensata_pack_status.balancing_bits;

				mavlink_msg_sensata_pack_status_send_struct(_mavlink->get_channel(), &sensata_pack_status_msg);
				updated = true;
			}
		}

		return updated;
	}
};

#endif // SENSATA_PACK_STATUS_HPP
