OUT		=	leela
FLAGS	=	`pkg-config --cflags --libs poppler-glib`
PREFIX	?=	usr/
CC_OPTS =	-Wall -Werror -Wextra -Wno-unused-parameter

${OUT}: leela.c
	@gcc ${CC_OPTS} -o leela leela.c ${FLAGS}

strip: ${OUT}
	@strip --strip-all ${OUT}

install: ${OUT}
	@install -Dm755 leela ${DESTDIR}${PREFIX}/bin/leela
	
