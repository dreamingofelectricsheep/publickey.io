static int base64_char(char c)
{
	if (c >= 'A' && c <= 'Z')
		return c - 'A';
	if (c >= 'a' && c <= 'z')
		return c - 'a' + 26;
	if (c >= '0' && c <= '9')
		return c - '0' + 52;
	if (c == '+')
		return 62;
	if (c == '/')
		return 63;
	if (c == '=')
		return 0;

	return -1;
}

bytes base64_decode(bytes base64, bytes mem)
{
	uint8_t n[4];
	int k = 0;

	if (mem.len < base64.len)
		return (bytes) {
		0, 0};

	for (int i = 0; i < base64.len; i += 4, k += 3) {
		for (int j = 0; j < 4; j++) {
			if (j + i < base64.len) {
				n[j] = base64_char(base64.as_char[i++]);
			} else {
				n[j] = 0;
			}
		}

		char *q = mem.as_char;

		q[0] = (n[0] << 2) + (n[1] >> 4);
		q[1] = ((n[1] & 15) << 4) + (n[2] >> 2);
		q[2] = ((n[2] & 3) << 6) + n[3];
	}

	return (bytes) {
	mem.as_void, k};
}
