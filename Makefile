OUT		=	leela
FLAGS	=	`pkg-config --cflags --libs poppler-glib`
PREFIX	?=	/usr

${OUT}: leela.c leela.h
	@gcc -o leela leela.c ${FLAGS}

strip: ${OUT}
	@strip --strip-all ${OUT}

install: ${OUT}
	@install -Dm755 AnnotExtract ${DESTDIR}${PREFIX}/bin/Annot
	
