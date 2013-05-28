/********************************************************************\
 Leela - a CLI frontend for the poppler-glib libary of PDF tools
 Copyright (C) 2012  Jesse McClure

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
\********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <poppler.h>
#include <cairo.h>
#include <cairo-pdf.h>

typedef struct {
	int *pages;
	int npages;
	PopplerDocument *pdf;
	const char *out;
} Options;

typedef struct {
	const char *name;
	void (*func) (Options *);
	const char *description;
} Command;

static const char *atypes[] = {
	[POPPLER_ANNOT_FREE_TEXT]		= "free text",
	[POPPLER_ANNOT_TEXT]			= "text",
	[POPPLER_ANNOT_LINK]			= "link",
	[POPPLER_ANNOT_LINE]			= "line",
	[POPPLER_ANNOT_SQUARE]			= "square",
	[POPPLER_ANNOT_CIRCLE]			= "circle",
	[POPPLER_ANNOT_POLYGON]			= "polygon",
	[POPPLER_ANNOT_POLY_LINE]		= "polyline",
	[POPPLER_ANNOT_HIGHLIGHT]		= "highlight",
	[POPPLER_ANNOT_UNDERLINE]		= "underline",
	[POPPLER_ANNOT_SQUIGGLY]		= "squiggly",
	[POPPLER_ANNOT_STRIKE_OUT]		= "strikeout",
	[POPPLER_ANNOT_STAMP]			= "stamp",
	[POPPLER_ANNOT_CARET]			= "caret",
	[POPPLER_ANNOT_INK]				= "ink",
	[POPPLER_ANNOT_POPUP]			= "popup",
	[POPPLER_ANNOT_FILE_ATTACHMENT]	= "file",
	[POPPLER_ANNOT_SOUND]			= "sound",
	[POPPLER_ANNOT_MOVIE]			= "movie",
	[POPPLER_ANNOT_WIDGET]			= "widget",
	[POPPLER_ANNOT_SCREEN]			= "screen",
	[POPPLER_ANNOT_PRINTER_MARK]	= "mark",
	[POPPLER_ANNOT_TRAP_NET]		= "trap net",
	[POPPLER_ANNOT_WATERMARK]		= "watermark",
	[POPPLER_ANNOT_3D]				= "3D",
	[POPPLER_ANNOT_UNKNOWN]			= "unknown",
};

static void leela_annot(Options *);
static void leela_append(Options *);
static void leela_attachment(Options *);
static void leela_data(Options *);
static void leela_help(Options *);
static void leela_images(Options *);
static void leela_pages(Options *);
static void leela_render(Options *);
static void leela_shrink(Options *);
static void leela_text(Options *);
static PopplerDocument *open_pdf(const char *);
static Command command[] = {
	{ "annots",			leela_annot,
		"extract annotations in an xml format"	},
	{ "append",			leela_append,
		"append pages from input onto output"	},
	{ "attachments",	leela_attachment,
		"extract attacments from input into outputXXX [EXPERIMENTAL]" },
	{ "data",			leela_data,
		"display data about pdf"				},
	{ "help",			leela_help,
		"show this help menu and exit"			},
	{ "images",			leela_images,
		"extract images from input into output" },
	{ "pages",			leela_pages,
		"extract pages from input into output"	},
	{ "render",			leela_render,
		"render annotations onto output [EXPERIMENTAL]"		},
	{ "shrink",			leela_shrink,
		"render for screen, often reduces file size"},
	{ "text",			leela_text,
		"extract text of pdf file"				},
};

void leela_annot(Options *opt) {
	PopplerPage *page;
	PopplerColor *col;
	PopplerRectangle *rect;
	PopplerAnnot *annot;
	GList *annots, *list;
	gchar *text;
	int i,j;
	for (i = 0; i < opt->npages; i++) {
		page = poppler_document_get_page(opt->pdf,opt->pages[i]);
		annots = poppler_page_get_annot_mapping(page);
		for (list = annots, j=0; list; list = list->next, j++) {
			rect = &((PopplerAnnotMapping *)list->data)->area;
			annot = ((PopplerAnnotMapping *)list->data)->annot;
			printf("<annot page=\"%d\" index=\"%d\" type=\"%s\">\n"
					"  <rect x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\"/>\n",
					i,j,atypes[	poppler_annot_get_annot_type(annot)],
					rect->x1,rect->y1,rect->x2,rect->y2);
			if ( (text=poppler_annot_get_name(annot)) ) {
				printf("  <name>%s</name>\n",text);
				g_free(text);
			}
			if ( (col=poppler_annot_get_color(annot)) ) {
				printf("  <color r=\"%d\" g=\"%d\" b=\"%d\"/>\n",
						col->red,col->green,col->blue);
				g_free(col);
			}
			if (POPPLER_IS_ANNOT_TEXT(annot)) {
				PopplerAnnotText *atext = (PopplerAnnotText *) annot;
			}
			if (POPPLER_IS_ANNOT_FREE_TEXT(annot)) {
				PopplerAnnotFreeText *ftext = (PopplerAnnotFreeText *) annot;
			}
			if (POPPLER_IS_ANNOT_FILE_ATTACHMENT(annot)) {
				PopplerAnnotFileAttachment *file = 
						(PopplerAnnotFileAttachment *) annot;
			}
			if (POPPLER_IS_ANNOT_MOVIE(annot)) {
				PopplerAnnotMovie *mov = (PopplerAnnotMovie *) annot;
			}
			if (POPPLER_IS_ANNOT_SCREEN(annot)) {
				PopplerAnnotScreen *scr = (PopplerAnnotScreen *) annot;
			}
			if (POPPLER_IS_ANNOT_MARKUP(annot)) {
				PopplerAnnotMarkup *mark = (PopplerAnnotMarkup *) annot;
				if ( (text=poppler_annot_markup_get_label(mark)) ) {
					printf("  <label>%s</label>\n",text);
					g_free(text);
				}
				if ( (text=poppler_annot_markup_get_subject(mark)) ) {
					printf("  <subject>%s</subject>\n",text);
					g_free(text);
				}
				printf("  </markup>\n");
			}
			if ( (text=poppler_annot_get_contents(annot)) ) {
				printf("  <text>%s</text>\n",text);
				g_free(text);
			}
			printf("</annot>\n");
		}
		poppler_page_free_annot_mapping(annots);
	}
	return;
}

void leela_append(Options *opt) {
	double pdfw,pdfh;
	char tmpPDF[] = "file:///tmp/leela_XXXXXX";
	mktemp(tmpPDF);
	PopplerDocument *base = open_pdf(opt->out);
	poppler_document_save_a_copy(base,tmpPDF,NULL);
	base = open_pdf(tmpPDF+7);
	PopplerPage *page = poppler_document_get_page(base,0);
	poppler_page_get_size(page,&pdfw,&pdfh);
	cairo_surface_t *t = cairo_pdf_surface_create(opt->out,pdfw,pdfh);
	cairo_t *c = cairo_create(t);
	cairo_surface_destroy(t);
	/* loop through pages and render them to the out file */
	int i, nbase = poppler_document_get_n_pages(base);
	for (i = 0; i < nbase; i++) {
		page = poppler_document_get_page(base,i);
		poppler_page_get_size(page,&pdfw,&pdfh);
		//cairo_pdf_surface_set_size(t,pdfw,pdfh);
		poppler_page_render(page,c);
		cairo_show_page(c);
	}
	page = poppler_document_get_page(opt->pdf,0);
	poppler_page_get_size(page,&pdfw,&pdfh);
	/* loop through pages and render them to the out file */
	for (i = 0; i < opt->npages; i++) {
		page = poppler_document_get_page(opt->pdf,opt->pages[i]);
		poppler_page_get_size(page,&pdfw,&pdfh);
		//cairo_pdf_surface_set_size(t,pdfw,pdfh);
		poppler_page_render(page,c);
		cairo_show_page(c);
	}
	cairo_destroy(c);
	remove(tmpPDF+7);
}

void leela_attachment(Options *opt) {
	int i;
	GList *list = poppler_document_get_attachments(opt->pdf);
	PopplerAttachment *att;
	char *fname = (char *) calloc(strlen(opt->out)+6,sizeof(char));
	strcpy(fname,opt->out);
	char *c = fname + strlen(opt->out);
	for (i = 0; list; i++, list = list->next) {
		att =  (PopplerAttachment *)list->data;
		snprintf(c,5,"%03d",i);
		poppler_attachment_save(att,fname,NULL);
	}
	free(fname);
	g_list_free(list);
}

void leela_data(Options *opt) {
	gchar *data=NULL;
	time_t when = 0;
	if (!opt->out)
		data = poppler_document_get_metadata(opt->pdf);
	else if (strncmp(opt->out,"title",5)==0)
		data = poppler_document_get_title(opt->pdf);
	else if (strncmp(opt->out,"author",6)==0)
		data = poppler_document_get_author(opt->pdf);
	else if (strncmp(opt->out,"subject",7)==0)
		data = poppler_document_get_subject(opt->pdf);
	else if (strncmp(opt->out,"keywords",7)==0)
		data = poppler_document_get_keywords(opt->pdf);
	else if (strncmp(opt->out,"creator",7)==0)
		data = poppler_document_get_creator(opt->pdf);
	else if (strncmp(opt->out,"producer",6)==0)
		data = poppler_document_get_producer(opt->pdf);
	else if (strncmp(opt->out,"created",7)==0)
		when = poppler_document_get_creation_date(opt->pdf);
	else if (strncmp(opt->out,"modified",5)==0)
		when = poppler_document_get_modification_date(opt->pdf);
	else
		return;
	if (when) {
		data = (gchar *) malloc(30);
		strftime(data,28,"%a %b %d %H:%M",localtime(&when));
	}
	if (data) {
		printf("%s\n",data);
		g_free(data);
	}
}

void leela_help(Options *opt) {
	int i;
	printf(
"Leela v0.3 Copyright (C) 2012-2013  Jesse McClure\n"
"This program comes with ABSOLUTELY NO WARRANTY\n"
"This is free software, and you are welcome to redistribute\n"
"it under the conditions of the GPLv3 license.\n\n"
"USAGE: leela command [ n1 n2 ... nN ] pdf output\n"
"  command     command from the list below\n"
"  n1...nN     range of pages to act on (defaul=all)\n"
"  pdf         filename of input pdf file\n"
"  output      select output type or filename\n\n"
"COMMANDS:\n");
	for (i = 0; i < sizeof(command)/sizeof(command[0]); i++)
		printf("  %-11s %s\n",command[i].name,command[i].description);
	printf("\nSee `man leela` for more information.\n\n");
	exit(0);
}

void leela_images(Options *opt) {
	PopplerPage *page;
	PopplerRectangle *rect;
	GList *images, *list;
	gint img_id = 0;
	cairo_surface_t *img;
	cairo_surface_t *t = cairo_pdf_surface_create(opt->out,10,10);
	cairo_t *c = cairo_create(t);
	int i,j;
	for (i = 0; i < opt->npages; i++) {
		page = poppler_document_get_page(opt->pdf,opt->pages[i]);
		images = poppler_page_get_image_mapping(page);
		for (list = images, j=0; list; list = list->next, j++) {
			rect = &((PopplerImageMapping *)list->data)->area;
			img_id = ((PopplerImageMapping *)list->data)->image_id;
			img = poppler_page_get_image(page,img_id);
			cairo_pdf_surface_set_size(t,rect->x2 - rect->x1,rect->y2 - rect->y1);
			cairo_set_source_surface(c,img,0,0);
			cairo_paint(c);
			cairo_show_page(c);
		}
		poppler_page_free_image_mapping(images);
	}
	cairo_surface_destroy(t);
	cairo_destroy(c);
	if (!img_id) {
		printf("no images found.\n");
		remove(opt->out);
	}
}

static void leela_page_render(Options *opt,PopplerPrintFlags p) {
	double pdfw,pdfh;
	PopplerPage *page = poppler_document_get_page(opt->pdf,0);
	poppler_page_get_size(page,&pdfw,&pdfh);
	cairo_surface_t *t = cairo_pdf_surface_create(opt->out,pdfw,pdfh);
	cairo_t *c = cairo_create(t);
	cairo_surface_destroy(t);
	/* loop through pages and render them to the out file */
	int i;
	for (i = 0; i < opt->npages; i++) {
		page = poppler_document_get_page(opt->pdf,opt->pages[i]);
		poppler_page_get_size(page,&pdfw,&pdfh);
		//cairo_pdf_surface_set_size(t,pdfw,pdfh);
		poppler_page_render_for_printing_with_options(page,c,p);
		cairo_show_page(c);
	}
	cairo_destroy(c);
}

void leela_pages(Options *opt) {
	if (!opt->npages)
		printf("%d\n",poppler_document_get_n_pages(opt->pdf));
	else
		leela_page_render(opt,POPPLER_PRINT_DOCUMENT);
}

void leela_render(Options *opt) {
	leela_page_render(opt,POPPLER_PRINT_ALL);
}

void leela_shrink(Options *opt) {
	double pdfw,pdfh;
	PopplerPage *page = poppler_document_get_page(opt->pdf,0);
	poppler_page_get_size(page,&pdfw,&pdfh);
	cairo_surface_t *t = cairo_pdf_surface_create(opt->out,pdfw,pdfh);
	cairo_t *c = cairo_create(t);
	cairo_surface_destroy(t);
	/* loop through pages and render them to the out file */
	int i;
	for (i = 0; i < opt->npages; i++) {
		page = poppler_document_get_page(opt->pdf,opt->pages[i]);
		poppler_page_get_size(page,&pdfw,&pdfh);
		//cairo_pdf_surface_set_size(t,pdfw,pdfh);
		poppler_page_render(page,c);
		cairo_show_page(c);
	}
	cairo_destroy(c);
}

void leela_text(Options *opt) {
	int i;
	FILE *out = stdout;
	if (opt->out) out = fopen(opt->out,"w");
	for (i = 0; i < opt->npages; i++) {
		fprintf(out,"%s\n", poppler_page_get_text(poppler_document_get_page(
				opt->pdf,opt->pages[i])) );
	}
	if (opt->out) fclose(out);
	return;
}

PopplerDocument *open_pdf(const char *arg) {
	char *fullpath = realpath(arg,NULL);
	if (!fullpath) {
		fprintf(stderr,"Cannot find file \"%s\"\n",arg);
		exit(1);
	}
	char *uri = (char *) malloc(strlen(fullpath)+8);
	strcpy(uri,"file://");
	strcat(uri,fullpath);
	free(fullpath);
	PopplerDocument *pdf = poppler_document_new_from_file(uri,NULL,NULL);
	free(uri);
	return pdf;
}

int range(Options **opt_p,int argc, const char **argv) {
	Options *opt = *opt_p;
	int i;
	for (i = 1; i < argc && argv[i][0] != ']'; i++) {
		opt->pages = (int *) realloc(opt->pages,i*sizeof(int));
		opt->pages[i-1] = atoi(argv[i]);
	}
	return (opt->npages=i-1)+1;
}

int range_all(Options **opt_p) {
	Options *opt = *opt_p;
	opt->npages = poppler_document_get_n_pages(opt->pdf);
	int i;
	opt->pages = (int *) malloc(opt->npages*sizeof(int));
	for (i = 0; i < opt->npages; i++) opt->pages[i] = i;
}

int main(int argc, const char **argv) {
	Options *opt = (Options *) calloc(1,sizeof(Options));
	if (argc < 3) leela_help(opt);
	int i;
	for (i = 2; i < argc; i++) {
		if (argv[i][0] == '[') i += range(&opt,argc-i,&argv[i]);
		else if (!opt->pdf) opt->pdf = open_pdf(argv[i]);
		else if (!opt->out) opt->out = argv[i];
		else fprintf(stderr,"ignoring unrecognized parameter \"%s\"\n",argv[i]);
	}
	if (!opt->npages && strncmp(argv[1],"page",4)) range_all(&opt);
	int max = poppler_document_get_n_pages(opt->pdf) - 1;
	for (i = 0; i < opt->npages; i++) {
		if (opt->pages[i] < 0) opt->pages[i] = 0;
		if (opt->pages[i] > max) opt->pages[i] = max;
	}
	for (i = 0; i < sizeof(command)/sizeof(command[0]); i++)
		if (strncmp(argv[1],command[i].name,strlen(argv[1]))==0)
			command[i].func(opt);
	if (opt->pages) free(opt->pages);
	free(opt);
	return 0;
}


