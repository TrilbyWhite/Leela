PROG	=	leela
CFLAGS	+=	-Os `pkg-config --cflags poppler-glib cairo`
LDFLAGS	+=	`pkg-config --libs poppler-glib cairo`
PREFIX	?=	/usr
MANDIR	?=	/usr/share/man

${PROG}: ${PROG}.c
	@gcc -o ${PROG} ${PROG}.c ${CFLAGS} ${LDFLAGS}

install: ${PROG}
	@install -Dm755 leela ${DESTDIR}${PREFIX}/bin/leela
	@install -Dm644 leela.1 ${DESTDIR}${MANDIR}/man1/leela.1

clean:
	@rm -f leela
