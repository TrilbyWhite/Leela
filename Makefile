OUT		=	leela
FLAGS	=	`pkg-config --cflags --libs poppler-glib`
PREFIX	?=	/usr
MANDIR	?=	/usr/share/man
CC_OPTS =	-Wall -Werror -Wextra -Wno-unused-parameter

${OUT}: leela.c
	@gcc ${CC_OPTS} -o leela leela.c ${FLAGS}
	@gzip -c leela.1 > leela.1.gz

strip: ${OUT}
	@strip --strip-all ${OUT}

install: ${OUT}
	@install -Dm755 leela ${DESTDIR}${PREFIX}/bin/leela
	@install -Dm666 leela.1.gz ${DESTDIR}${MANDIR}/man1/leela.1.gz

clean:
	@rm leela leela.1.gz
