PROG=	mlcd
SRCS=	mlcd.c 
MAN=	mlcd.1

UNAME?=	$$(uname -s)

LUA_MODULES=		mlcd
LUA_SRCS.mlcd=		lmlcd.c

.if ${UNAME} == "NetBSD"
.include <bsd.lua.mk>
.else
all:
	cc -DDEBUG -I /opt/pkg/include/lua-5.3 -L /opt/pkg/lib -llua5.3 -dynamiclib lmlcd.c -o mlcd.dylib
.endif

.include <bsd.prog.mk>