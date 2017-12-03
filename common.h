#ifndef MLCD_COMMON_H
#define MLCD_COMMON_H

#define BYTE_TO_ASCII(byte)  \
	(byte & 0x80 ? '#' : ' '), \
	(byte & 0x40 ? '#' : ' '), \
	(byte & 0x20 ? '#' : ' '), \
	(byte & 0x10 ? '#' : ' '), \
	(byte & 0x08 ? '#' : ' '), \
	(byte & 0x04 ? '#' : ' '), \
	(byte & 0x02 ? '#' : ' '), \
	(byte & 0x01 ? '#' : ' ')

#define MLCD_WIDTH 48
#define MLCD_HEIGHT 32
#define MLCD_BYTES ((MLCD_WIDTH * MLCD_HEIGHT) / 8)

#endif /* MLCD_COMMON_H */
