
#define CRC24_INIT 0xB704CEL
#define CRC24_POLY 0x1864CFBL

uint32_t crc(bytes data)
{
	uint32_t crc = CRC24_INIT;
	int i;

	while (data.len--) {
		crc ^= (*data.as_char++) << 16;
		for (i = 0; i < 8; i++) {
			crc <<= 1;
			if (crc & 0x1000000) {
				crc ^= CRC24_POLY;
			}
		}
	}

	return crc & 0xFFFFFFL;
}


