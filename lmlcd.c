/* mlcd interface for Lua */

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "common.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

int luaopen_mlcd(lua_State*);

struct constant {
	const char *name;
	int value;
};

static const struct constant mlcd_constant[] = {
	{ "HEIGHT",		MLCD_HEIGHT },
	{ "WIDTH",		MLCD_WIDTH },
	{ NULL,			0 }
};

typedef struct bit_array {
	uint8_t * bytes;
	uint16_t num_of_bits;
	uint8_t num_of_bytes;
} bit_array;

bit_array *frame = NULL;

static void
bit_array_clear_all(bit_array *b) {
	memset(b->bytes, 0, b->num_of_bytes);
}

bit_array *
bit_array_create(uint16_t nbits) {
	bit_array* b = malloc(sizeof(bit_array));
	if (b != NULL) {
		b->num_of_bits = nbits;
		b->num_of_bytes = nbits / 8;
		b->bytes = malloc(b->num_of_bytes);
		if (b->bytes == NULL) {
			free(b);
			return NULL;
		}
		bit_array_clear_all(b);
		return b;
	}	
	return NULL;
}

static void
bit_array_set_all(bit_array *b) {
	memset(b->bytes, 0xFF, b->num_of_bytes);
}

static void 
bit_array_print(bit_array *b, FILE* fout) {
	int i;
	for (i = 0; i < b->num_of_bytes; i++) {
		fprintf(fout, "%c%c%c%c%c%c%c%c", BYTE_TO_ASCII(b->bytes[i]));
	}
}

static void
bit_array_set(bit_array *b, uint16_t bit) {
	b->bytes[bit/8] |= 1UL << (7 - bit % 8);
}

static void
_point(int x, int y)
{
	bit_array_set(frame, (y * MLCD_WIDTH) + x);
}

static int
mlcd_point(lua_State *L)
{
	_point(luaL_checkinteger(L, 1), luaL_checkinteger(L, 2));
	return 0;
}

static void
_line(int x0, int y0, int x1, int y1)
{
	int dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
	int dy = abs(y1-y0), sy = y0<y1 ? 1 : -1; 
	int err = (dx>dy ? dx : -dy)/2, e2;
	for(;;){
		bit_array_set(frame, (y0 * MLCD_WIDTH) + x0);
		if (x0==x1 && y0==y1) break;
		e2 = err;
		if (e2 >-dx) { err -= dy; x0 += sx; }
		if (e2 < dy) { err += dx; y0 += sy; }
	}
}

static int
mlcd_line(lua_State *L)
{
	_line(luaL_checkinteger(L, 1), luaL_checkinteger(L, 2),
		luaL_checkinteger(L, 3), luaL_checkinteger(L, 4));
	return 0;
}

static int
mlcd_triangle(lua_State *L)
{
	int x0 = luaL_checkinteger(L, 1);
	int y0 = luaL_checkinteger(L, 2);
	int x1 = luaL_checkinteger(L, 3);
	int y1 = luaL_checkinteger(L, 4);
	int x2 = luaL_checkinteger(L, 5);
	int y2 = luaL_checkinteger(L, 6);
	_line(x0, y0, x1, y1);
	_line(x1, y1, x2, y2);
	_line(x2, y2, x0, y0);
	return 0;
}

static int
mlcd_quad(lua_State *L)
{
	int x0 = luaL_checkinteger(L, 1);
	int y0 = luaL_checkinteger(L, 2);
	int x1 = luaL_checkinteger(L, 3);
	int y1 = luaL_checkinteger(L, 4);
	int x2 = luaL_checkinteger(L, 5);
	int y2 = luaL_checkinteger(L, 6);
	int x3 = luaL_checkinteger(L, 7);
	int y3 = luaL_checkinteger(L, 8);
	_line(x0, y0, x1, y1);
	_line(x1, y1, x2, y2);
	_line(x2, y2, x3, y3);
	_line(x3, y3, x0, y0);
	return 0;
}

static int
mlcd_circle(lua_State *L)
{
	int x0 = luaL_checkinteger(L, 1);
	int y0 = luaL_checkinteger(L, 2);
	int r = luaL_checkinteger(L, 3);

	int x = r;
	int y = 0;
	int err = 0;

	while (x >= y) {

		_point(x0 + x, y0 + y);
		_point(x0 + y, y0 + x);
		_point(x0 - y, y0 + x);
		_point(x0 - x, y0 + y);
		_point(x0 - x, y0 - y);
		_point(x0 - y, y0 - x);
		_point(x0 + y, y0 - x);
		_point(x0 + x, y0 - y);

		if (err <= 0) {
			y += 1;
			err += 2*y + 1;
		}

		if (err > 0) {
			x -= 1;
			err -= 2*x + 1;
		}
	}
	return 0;
}

static int
mlcd_clear(lua_State *L)
{
	bit_array_clear_all(frame);
	return 0;
}

static int
mlcd_background(lua_State *L)
{
	if (luaL_checkinteger(L, 1)) {
		bit_array_set_all(frame);
	} else {
		bit_array_clear_all(frame);
	}
	return 0;
}

static int
mlcd_dump(lua_State *L)
{
	bit_array_print(frame, stdout);
	fputc('\n', stdout);
	return 0;
}

static int
mlcd_save(lua_State *L)
{
	unsigned char data[((MLCD_WIDTH * MLCD_HEIGHT) * 3) + 1];
	const char *path;
	size_t length;
	int byte;
	int bit;
	int offset;
	int current;
	path = lua_tolstring (L, 1, &length);
	
	for (byte = 0; byte < frame->num_of_bytes; byte++) {
		for (bit = 0; bit < 8; bit++) {
			offset = ((byte * 8) + bit) * 3 - 1;
			current = frame->bytes[byte] >> (7 - bit) & 1;
			data[offset + 1] = (current) ? 0 : 255;
			data[offset + 2] = (current) ? 0 : 255;
			data[offset + 3] = (current) ? 0 : 255;
		}
	}

	stbi_write_bmp(path, MLCD_WIDTH, MLCD_HEIGHT, 3, data);

	return 0;
}

static int
mlcd_draw(lua_State *L)
{
	const char *path;
	size_t length;
	int draw;
	int frames;
	FILE *f;
	int mssleep;

	path = lua_tolstring (L, 1, &length);

	if (lua_isfunction(L, 2)) {
		lua_pushvalue(L, 2);
		draw = luaL_ref(L, LUA_REGISTRYINDEX);
	} else {
		printf("no draw fn");
		return 0;
	}

	frames = -1;
	if (lua_isnumber(L, 3)) {
		frames = luaL_checkinteger(L, 3);
	}

	mssleep = -1;
	if (lua_isnumber(L, 4)) {
		mssleep = luaL_checkinteger(L, 4);
	}

	for (;;) {
		lua_rawgeti(L, LUA_REGISTRYINDEX, draw);
		lua_pcall(L, 0, 1, 0);
		if (path) {
			f = fopen(path, "a"); // XXX
			if (f == NULL) { 
				printf("unable to open file");
				return 0;
			}
			if(fwrite(frame->bytes, 1, frame->num_of_bytes, f) != MLCD_BYTES) {
				printf("did not write all bytes\n");
			}
			fclose(f); // XXX
		}
		if (frames != -1) {
			frames -= 1;
			if (!frames) {
				break;
			}
		}
		usleep((mssleep == -1) ? 10000 : mssleep);
	}
	return 0;
}

static int
mlcd_text(lua_State *L)
{
	const char *str;
	size_t length;
	str = lua_tolstring (L, 1, &length);
	printf("String: %.*s", (int)length, str);
	return 0;
}

int
luaopen_mlcd(lua_State* L)
{
	static const struct luaL_Reg mlcd_methods[] = {
		{ "point",			mlcd_point },
		{ "line",			mlcd_line },
		{ "triangle",			mlcd_triangle },
		{ "quad",			mlcd_quad },
		{ "circle",			mlcd_circle },
		{ "clear",			mlcd_clear },
		{ "background",			mlcd_background },
		{ "dump",			mlcd_dump },
		{ "save",			mlcd_save },
		{ "draw",			mlcd_draw },
		{ "text",			mlcd_text },
		{ NULL,				NULL }
	};

	frame = bit_array_create(MLCD_WIDTH * MLCD_HEIGHT);

	luaL_newlib(L, mlcd_methods);

	for (int n = 0; mlcd_constant[n].name != NULL; n++) {
		lua_pushinteger(L, mlcd_constant[n].value);
		lua_setfield(L, -2, mlcd_constant[n].name);
	};

	return 1;
}
