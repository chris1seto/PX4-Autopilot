#pragma once

#include "QueueBuffer.hpp"

class AeroprobeHyperionParser
{
public:
  enum AeroprobeMessageType
  {
    AirData
  };

  struct AeroprobeAirData
  {
  };

  struct AeroprobeMessage
  {
    AeroprobeMessageType type;
    union
    {
      AeroprobeAirData air_data;
    };
  };

  AeroprobeHyperionParser();
  ~AeroprobeHyperionParser();

  bool LoadData(const uint8_t* data, const uint32_t size);
  uint32_t FreeParseBufferSize(void);
  bool Parse(AeroprobeMessage& message_out);

private:
  QueueBuffer_t _parse_queuebuffer;
  static constexpr uint32_t PARSE_BUFFER_SIZE{200};

  uint8_t _parse_buffer[PARSE_BUFFER_SIZE]{};

  enum ParseState
  {
    Header1,
    Header2,
    PacketId,
    PacketLength,
    Payload,
    Checksum
  };

  ParseState _parse_state{Header1};
  uint32_t _parse_index{0};
  uint32_t _parse_segment_size{1};

  static constexpr uint32_t HEADER1_SIZE{1};
  static constexpr uint32_t HEADER2_SIZE{1};
  static constexpr uint32_t HEADER1_WORD{0xff};
  static constexpr uint32_t HEADER2_WORD{0xfe};

};