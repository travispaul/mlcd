#ifndef MLCD_COMMON_H
#define MLCD_COMMON_H

#define BYTE_TO_ASCII(byte)  \
	(byte & 0x80 ? '1' : '0'), \
	(byte & 0x40 ? '1' : '0'), \
	(byte & 0x20 ? '1' : '0'), \
	(byte & 0x10 ? '1' : '0'), \
	(byte & 0x08 ? '1' : '0'), \
	(byte & 0x04 ? '1' : '0'), \
	(byte & 0x02 ? '1' : '0'), \
	(byte & 0x01 ? '1' : '0')

#define MLCD_WIDTH 48
#define MLCD_HEIGHT 32
#define MLCD_BYTES ((MLCD_WIDTH * MLCD_HEIGHT) / 8)

#endif /* MLCD_COMMON_H */
