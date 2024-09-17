#include "AeroprobeHyperionParser.hpp"


AeroprobeHyperionParser::AeroprobeHyperionParser()
{
  QueueBuffer_Init(_parse_queuebuffer, parse_buffer_, PARSE_BUFFER_SIZE);
}

AeroprobeHyperionParser::~AeroprobeHyperionParser()
{
}

bool AeroprobeHyperionParser::LoadData(const uint8_t* data, const uint32_t size)
{
	return QueueBuffer_AppendBuffer(&_parse_queuebuffer, data, size);
}

uint32_t AeroprobeHyperionParser::FreeParseBufferSize(void)
{
	return PARSE_BUFFER_SIZE - QueueBuffer_Count(&_parse_queuebuffer);
}

bool AeroprobeHyperionParser::Parse(AeroprobeMessage& message_out)
{
	uint32_t buffer_count;
  uint8_t working_byte;

  buffer_count = QueueBuffer_Count(&_parse_queuebuffer);

  while ((_parse_index < buffer_count)
    && (buffer_count - _parse_index) >= _parse_segment_size)
  {
    switch (_parse_state)
    {
      case Header1:
        if (QueueBuffer_Get(&_parse_queuebuffer, &working_byte))
        {
          if (working_byte == HEADER1_WORD)
          {
            _parse_state = Header2;
            _parse_segment_size = HEADER2_SIZE;
            _parse_index = 0;
            buffer_count = QueueBuffer_Count(&_parse_queuebuffer);
            continue;
          }
        }

        _parse_index = 0;
        _parse_segment_size = HEADER1_SIZE;
        break;

      case Header2:
        break;
    }
  }

  return false;
}
