#include <stdio.h>
#include <unistd.h>

#include "common.h"

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

	data = (unsigned char *)stbi_load(argv[0], &x, &y, &n, 3);

	if (!data) {
		(void)fprintf(stderr, "stbi_load: %s\n", stbi_failure_reason());
		exit(1);
	}

	if (x != MLCD_WIDTH || y != MLCD_HEIGHT) {
		(void)fprintf(stderr, "image must be %dx%d\n",
			MLCD_WIDTH, MLCD_HEIGHT);
		exit(1);
	}

	if (format == 'c')
		printf("static const char initimg%dx%d[%d] = {\n",
			MLCD_WIDTH, MLCD_HEIGHT, MLCD_BYTES);

	for (ny = 0; ny < y; ny++) {

		if (format == 'a') printf(".byte ");

		for (nx = 0; nx < x; nx++) {

			offset = (nx + ny * x) * 3 - 1;

			total = (total<<1) |
				!(data[offset + 1] + data[offset + 2] + data[offset + 3]);

			count += 1;

			if (count == 8) {
				switch (format) {
					/* asm */
					case 'a':
						printf("%%%c%c%c%c%c%c%c%c",
							BYTE_TO_ASCII(total));
						if (nx != 47) {
							printf(",");
						}
						break;
					/* Each byte to ASCII */
					case 'b':
						printf("%c%c%c%c%c%c%c%c",
							BYTE_TO_ASCII(total));
						break;
					/* sys/arch/dreamcast/dev/maple/mlcd.c */
					case 'c':
						printf("0x%02x", total);
						if (ny == 31 && nx == 47) {
							printf("\n};\n");
						} else {
							printf(", ");
						}
						break;
					/* Raw data */
					case 'd': 
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
