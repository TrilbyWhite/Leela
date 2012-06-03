OUT		=	leela
FLAGS	=	`pkg-config --cflags --libs poppler-glib`
PREFIX	?=	/usr

${OUT}: leela.c
	@gcc -o leela leela.c ${FLAGS}

strip: ${OUT}
	@strip --strip-all ${OUT}

install: ${OUT}
	@install -Dm755 leela ${DESTDIR}${PREFIX}/bin/leela
	
