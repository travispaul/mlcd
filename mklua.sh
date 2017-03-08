#!/bin/bash
cc -DDEBUG -I /opt/pkg/include/lua-5.3 -L /opt/pkg/lib -llua5.3 -dynamiclib lmlcd.c bit_array.c -o mlcd.dylib
