OUT		=	leela
FLAGS	=	`pkg-config --cflags --libs poppler-glib`
PREFIX	?=	/usr
MANDIR	?=	/usr/share/man
CC_OPTS =	-Wall -Werror -Wextra -Wno-unused-parameter

${OUT}: leela.c
	@gcc ${CC_OPTS} -o leela leela.c ${FLAGS}

install: ${OUT}
	@install -Dm755 leela ${DESTDIR}${PREFIX}/bin/leela
	@install -Dm644 leela.1 ${DESTDIR}${MANDIR}/man1/leela.1

clean:
	@rm leela leela.1.gz
