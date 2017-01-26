#include <stdio.h>
#include <unistd.h>

#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0') 

#define STBI_NO_HDR
#define STBI_TYPE_SPECIFIC_FUNCTIONS
#define STBI_SMALL_STACK
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.c"

static void usage(void) __dead;

int
main(int argc, char **argv)
{
	extern char *optarg;
	extern int optind;
	int ch, format;
	int x, y, n;
	int nx, ny;
	unsigned char *data;
	int offset, count, total;

	format = 'b';
	count = total = 0;

	while ((ch = getopt(argc, argv, "abcd")) != -1) {
		switch (ch) {
			case 'a':
				format = 'a';
				break;
			case 'b':
				format = 'b';
				break;
			case 'c':
				format = 'c';
				break;
			case 'd':
				format = 'd';
				break;
			case '?':
			default:
				usage();
		}
	}
	argc -= optind;
	argv += optind;

	if (!argv[0]) {
		(void)fprintf(stderr, "no input image provided\n");
		usage();
	}

	data = stbi_load(argv[0], &x, &y, &n, 3);

	if (!data) {
		(void)fprintf(stderr, "stbi_load: %s\n", stbi_failure_reason());
		exit(1);
	}

	if (x != 48 || y != 32) {
		(void)fprintf(stderr, "image must be 48x32\n");
		exit(1);
	}

	if (format == 'c') printf("static const char initimg48x32[192] = {\n");

	for (ny = 0; ny < y; ny++) {

		if (format == 'a') printf(".byte ");

		for (nx = 0; nx < x; nx++) {

			offset = (nx + ny * x) * 3 - 1;

			total = (total<<1) |
				!(data[offset + 1] + data[offset + 2] + data[offset + 3]);

			count += 1;

			if (count == 8) {
				switch (format) {
					case 'a': // asm
						printf("%%%c%c%c%c%c%c%c%c", BYTE_TO_BINARY(total));
						if (nx != 47) {
							printf(",");
						}
						break;
					case 'b': // acii binary
						printf("%c%c%c%c%c%c%c%c", BYTE_TO_BINARY(total));
						break;
					case 'c': // C code
						printf("0x%02x", total);
						if (ny == 31 && nx == 47) {
							printf("\n};\n");
						} else {
							printf(", ");
						}
						break;
					case 'd': // binary data
						putc(total, stdout);
						break;
				}
				count = total = 0;
			}
		}
		if (format != 'd') printf("\n");
	}

}

static void
usage()
{
	(void)fprintf(stderr,
		"Usage: mlcd [-abcd] image\n");
	exit(1);
}
