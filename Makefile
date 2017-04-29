PROG=	mlcd
SRCS=	mlcd.c 
MAN=	mlcd.1

LUA_MODULES=		mlcd
LUA_SRCS.mlcd=		lmlcd.c

.include <bsd.lua.mk>
.include <bsd.prog.mk>
