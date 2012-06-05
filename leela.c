#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <poppler.h>

#ifndef PS2PDF
#define PS2PDF "gs -P- -dSAFER -q -P- -dNOPAUSE -dBATCH -sDEVICE=pdfwrite -sstdout=%%stderr -sOutputFile=- -P- -dSAFER -c .setpdfwrite -f %s"
#define PS2PDFLEN	135
#endif //PS2PDF

#define VERSION		"v0.1"
#define COMMANDS	7
#define DATE_FORMAT	"%a %b %d %H:%M"

typedef int (*callable)(int,const char **);
PopplerDocument *openPDF(const char *);

// leela_get_page extracts the specified page numbers from
// the input PDF and pipes them out as a new PDF.
// leela_get_page allows for commands to be strung together
// in order to restrict on which pages a command is run, eg:
// $ leela page 1 myPDF.pdf | leela annots -
int leela_get_page(int argc, const char **argv) {
	PopplerDocument *PDF = openPDF(argv[argc-1]);
	// check page count:
	if (argv[2][0] == '?') {
		printf("%d\n",poppler_document_get_n_pages(PDF));
		return 0;
	}
	// create temp ps file:
	char *tmpPS=NULL;
	tmpPS = (char *) malloc(L_tmpnam);
	tmpnam(tmpPS);
	PopplerPSFile *ps = poppler_ps_file_new(PDF,tmpPS,0,argc-3);
	// loop through pages and render them to the ps file:
	int i,p;
	for (i = 2; i < argc - 1; i++) {
		sscanf(argv[i],"%d",&p);
		poppler_page_render_to_ps(poppler_document_get_page(PDF,p-1),ps);
	}
	poppler_ps_file_free(ps);
	char *command;
	command = (char *) malloc(PS2PDFLEN + 20);
	sprintf(command,PS2PDF,tmpPS);
	free(tmpPS);
	system(command);
	return 0;
}

// leela_annots extracts annotations from a PDF
int leela_annots(int argc, const char **argv) {
	// currently text only
	PopplerDocument *PDF = openPDF(argv[argc-1]);
	PopplerPage *page;
	GList *annots, *list;
	gchar *text=NULL;
	PopplerAnnotMapping *mapping;
	int i,j;
	for (i = 0; i < poppler_document_get_n_pages(PDF); i++) {
		page = poppler_document_get_page(PDF,i);
		annots = poppler_page_get_annot_mapping(page);
		j=0;
		PopplerAnnotType atype;
		for (list = annots; list; list = list->next) {
			mapping = (PopplerAnnotMapping *)list->data;
			j++;
			atype = poppler_annot_get_annot_type((PopplerAnnot *)mapping->annot);
			switch (atype) {
				case POPPLER_ANNOT_UNKNOWN:
					printf("<%d,%d:unknown>",j,i+1); break;
				case POPPLER_ANNOT_FREE_TEXT:
					printf("<%d,%d:free text>",j,i+1); break;
				case POPPLER_ANNOT_TEXT:
					printf("<%d,%d:text>",j,i+1); break;
				case POPPLER_ANNOT_LINK:
					printf("<%d,%d:link>",j,i+1); break;
				case POPPLER_ANNOT_LINE:
					printf("<%d,%d:line>",j,i+1); break;
				case POPPLER_ANNOT_SQUARE:
					printf("<%d,%d:square>",j,i+1); break;
				case POPPLER_ANNOT_CIRCLE:
					printf("<%d,%d:circle>",j,i+1); break;
				case POPPLER_ANNOT_POLYGON:
					printf("<%d,%d:polygon>",j,i+1); break;
				case POPPLER_ANNOT_POLY_LINE:
					printf("<%d,%d:polyline>",j,i+1); break;
				case POPPLER_ANNOT_HIGHLIGHT:
					printf("<%d,%d:highlight>",j,i+1); break;
				case POPPLER_ANNOT_UNDERLINE:
					printf("<%d,%d:underline>",j,i+1); break;
				case POPPLER_ANNOT_SQUIGGLY:
					printf("<%d,%d:squiggly>",j,i+1); break;
				case POPPLER_ANNOT_STRIKE_OUT:
					printf("<%d,%d:strikeout>",j,i+1); break;
				case POPPLER_ANNOT_STAMP:
					printf("<%d,%d:stamp>",j,i+1); break;
				case POPPLER_ANNOT_CARET:
					printf("<%d,%d:caret>",j,i+1); break;
				case POPPLER_ANNOT_INK:
					printf("<%d,%d:ink>",j,i+1); break;
				case POPPLER_ANNOT_POPUP:
					printf("<%d,%d:popup>",j,i+1); break;
				case POPPLER_ANNOT_FILE_ATTACHMENT:
					printf("<%d,%d:file>",j,i+1); break;
				case POPPLER_ANNOT_SOUND:
					printf("<%d,%d:sound>",j,i+1); break;
				case POPPLER_ANNOT_MOVIE:
					printf("<%d,%d:movie>",j,i+1); break;
				case POPPLER_ANNOT_WIDGET:
					printf("<%d,%d:widget>",j,i+1); break;
				case POPPLER_ANNOT_SCREEN:
					printf("<%d,%d:screen>",j,i+1); break;
				case POPPLER_ANNOT_PRINTER_MARK:
					printf("<%d,%d:mark>",j,i+1); break;
				case POPPLER_ANNOT_TRAP_NET:
					printf("<%d,%d:trap net>",j,i+1); break;
				case POPPLER_ANNOT_WATERMARK:
					printf("<%d,%d:watermark>",j,i+1); break;
				case POPPLER_ANNOT_3D:
					printf("<%d,%d:3D>",j,i+1); break;
			}
			text = poppler_annot_get_contents((PopplerAnnot *)mapping->annot);
			printf("%s\n",text);
			g_free(text);
		}
		poppler_page_free_annot_mapping(annots);
	}
	return 0;
}

int leela_text(int argc, const char **argv) {
	PopplerDocument *PDF = openPDF(argv[argc-1]);
	char *text;
	int i;
	for (i = 0; i < poppler_document_get_n_pages(PDF); i++) {
		//page = poppler_document_get_page(PDF,i);
		text = poppler_page_get_text(poppler_document_get_page(PDF,i));
		printf("%s\n",text);
	}
	return 0;
}

// leela_append adds one PDF to the end of the other
int leela_append(int argc, const char **argv) {
	PopplerDocument *PDF = openPDF(argv[argc-1]);
	PopplerDocument *append = openPDF(argv[2]);
	int pgs1 = poppler_document_get_n_pages(PDF);
	int pgs2 = poppler_document_get_n_pages(append);
	// create temp ps files:
	char *tmp1=NULL;
	char *tmp2=NULL;
	tmp1 = (char *) malloc(L_tmpnam);
	tmp2 = (char *) malloc(L_tmpnam);
	tmpnam(tmp1);
	tmpnam(tmp2);
	// loop through pages and render them to the ps file:
	int i;
	PopplerPSFile *ps = poppler_ps_file_new(PDF,tmp1,0,pgs1);
	for (i = 0; i < pgs1; i++)
		poppler_page_render_to_ps(poppler_document_get_page(PDF,i),ps);
	poppler_ps_file_free(ps);
	ps = poppler_ps_file_new(append,tmp2,0,pgs2);
	for (i = 0; i < pgs2; i++)
		poppler_page_render_to_ps(poppler_document_get_page(append,i),ps);
	poppler_ps_file_free(ps);
	char *command;
	command = (char *) malloc(PS2PDFLEN + 40);
	sprintf(command,PS2PDF " %s",tmp1,tmp2);
	free(tmp1);free(tmp2);
	system(command);
	return 0;
}

int leela_get_metadata(int argc, const char **argv) {
	PopplerDocument *PDF = openPDF(argv[argc-1]);
	gchar *data=NULL;
	time_t when;
	if (argc == 3)
		data = poppler_document_get_metadata(PDF);
	else if (strncmp(argv[2],"title",5)==0)
		data = poppler_document_get_title(PDF);
	else if (strncmp(argv[2],"author",6)==0)
		data = poppler_document_get_author(PDF);
	else if (strncmp(argv[2],"subject",7)==0)
		data = poppler_document_get_subject(PDF);
	else if (strncmp(argv[2],"keywords",7)==0)
		data = poppler_document_get_keywords(PDF);
	else if (strncmp(argv[2],"creator",7)==0)
		data = poppler_document_get_creator(PDF);
	else if (strncmp(argv[2],"producer",6)==0)
		data = poppler_document_get_producer(PDF);
	else if (strncmp(argv[2],"created",7)==0)
		when = poppler_document_get_creation_date(PDF);
	else if (strncmp(argv[2],"modified",5)==0)
		when = poppler_document_get_modification_date(PDF);
	else
		return 1;
	if (data == NULL) {
		data = (gchar *) malloc(30);
		strftime(data,28,DATE_FORMAT,localtime(&when));
	}
	printf("%s\n",data);
	g_free(data);
	return 0;
}

int leela_get_images(int argc, const char **argv) {
	// get all images and send list to stdout?
	return 0;
}

int leela_get_attachment(int argc, const char **argv) {
	PopplerDocument *PDF = openPDF(argv[argc-1]);
	gboolean has;
	if (argv[2][0] == '?') {
		has = poppler_document_has_attachments(PDF);
		if (has == TRUE) printf("Yes\n");
		else printf("No\n");
		return 0;
	}
	// TODO: read attachments.
	return 0;
}

// leela_help is defined below the command table so
// it can refer to the table
int leela_help(int,const char **);

//----------  COMMAND -> FUNCTION TABLE  -----------//
/*////////////////////////////////////////////////////
COMMAND LINE			MIN PARAMS	FUNCTION TO RUN */	struct{
const char *command;	int argc;	callable function;	}list[COMMANDS]={{
//////////////////////////////////////////////////////
"page",					1,			leela_get_page		},{
"annots",				0,			leela_annots		},{
"append",				1,			leela_append		},{
"data",					0,			leela_get_metadata	},{
"attachment",			1,			leela_get_attachment},{
"text",					0,			leela_text			},{
"help",					-1,			leela_help			}};
//////////////////////////////////////////////////////

int leela_help(int argc, const char **argv){
	printf("%s %s\nLICENSE INFO\n\nAvailable commands:\n",argv[0],VERSION);
	int i;
	printf("%s",list[0].command);
	for (i = 1; i < COMMANDS; i++) printf(", %s",list[i].command);
	printf("\nEach command has it's own man page under leela-<command>\n\n");
	return 0;
}

// usage prints help and exits depending on the opt code
int usage(int opt) {
	if (opt < 0) exit(1);
	return 0;
}

PopplerDocument *openPDF(const char *filename) {
	char *name=NULL, *path;
	int c;
	FILE *tmpfile;
	if ( (filename[0]=='-') && (filename[1]=='\0') ) {
		name = (char *) malloc(L_tmpnam);
		tmpnam(name);
		tmpfile = fopen(name,"w");
		while ( (c=getc(stdin)) != EOF )
			fputc(c,tmpfile);
		fclose(tmpfile);
		return openPDF(name);
	}
	// parse filename and expand to URL if needed	
	if ((	strncmp(filename,"http:",5)==0) ||	// full uri
			(strncmp(filename,"file:",5)==0) ||
			(strncmp(filename,"https:",6)==0)) {
		name = (char *) malloc(strlen(filename) + 1);
		strcpy(name,filename);
	}
	else if (filename[0] == '/') {	// full path but not uri
			name = (char *) malloc(strlen(filename) + 8);
			strcpy(name,"file://");
			strcat(name,filename);
	} else {							// no path, no uri
		path = getcwd(NULL,0);
		name = (char *) malloc(strlen(path) + strlen(filename) + 8);
		strcpy(name,"file://");
		strcat(name,path);
		strcat(name,"/");
		strcat(name,filename);
		free(path);
	}
	PopplerDocument *PDF =  poppler_document_new_from_file(name,NULL,NULL);
	free(name);
	return PDF;
}

// main does nothing put read the command in argv[1] and
// passes control on to the appropriate function.
int main(int argc, const char **argv) {
	if (argc < 2) exit(1);
	g_type_init();
	int match;
	int i;
	for (i = 0; i < COMMANDS; i++)
		if (strncmp(argv[1],list[i].command,4)==0) {
			if (argc < list[i].argc + 3) usage(-1-i);
			list[i].function(argc,argv);
			match = 1;
		}
	if (match == 1) return 0;
	if (argv[1][0] == '-') switch (argv[1][1]) {
		case '?':
		case 'h': leela_help(argc,argv); return 0;
		case 'v': printf("%s\n",VERSION); return 0;
		default:
			fprintf(stderr,"Unrecognized command line switch \"%s\"\n",argv[1]);
			return 1;
	}
	fprintf(stderr,"Unrecognized command \"%s\"\ntry '%s help' for help.\n",
		argv[1],argv[0]);
	return 1;
}


