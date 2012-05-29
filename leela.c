#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <poppler.h>
#include "leela.h"

#define USAGE	{printf("Requires 2 arguments.\n\
Argument 1 must be a command switch \"-Qat\" is the only one currently recognized\n\
Argument 2 must be a full URI such as file:///home/username/file.pdf\n\n"); \
exit(1);}

int main(int argc, char **argv) {
	if ( (argc < 3) || (argv[1][0] != '-') || (strlen(argv[1]) < 2) ) USAGE; 
	PopplerDocument *PDF;
	g_type_init();
	if (argv[1][0] != '-') USAGE;
	int i,flags=0,page_num;
	for (i = 1; i < strlen(argv[1]); i++) {
		if (i < 3) switch (argv[1][i]) { //FLAGS and subFLAGS
			case 'Q': flags += Q_FLAG; break;
			case 'S': flags += S_FLAG; break;
			case 'W': flags += W_FLAG; break;
			case 'a': flags += a_FLAG; break;
			case 'p': flags += p_FLAG; break;
			case 'd': flags += d_FLAG; break;
			default: break;
		}
		else switch (argv[1][i]) { //OPTIONS and LIMITS
			case 't': flags += t_OPT; break;
			case 'i': flags += i_OPT; break;
			case 'v': flags += v_OPT; break;
			case 'a': flags += a_OPT; break;
			case 'l': flags += l_OPT; break;
			case 'p': flags += p_LIM; sscanf(&argv[1][i],"p%d",&page_num); break;
			default: break;
		}
	}
	// TODO: test if flag string requires additional parameters
	PDF = openPDF(argv[2]);
	if ( (Q_FLAG == (flags & Q_FLAG)) && (a_FLAG == (a_FLAG & flags)) )
		queryAnnots(PDF,flags,page_num);
	else if ( (Q_FLAG == (flags & Q_FLAG)) && (p_FLAG == (p_FLAG & flags)) )
		queryPageCount(PDF);
	else if ( (Q_FLAG == (flags & Q_FLAG)) && (d_FLAG == (d_FLAG & flags)) )
		queryData(PDF,flags);
	else USAGE;
	return 0;
}

PopplerDocument *openPDF(const char *filename) {
	if ( strncmp(filename,"file://",6) != 0 ) USAGE;
	// TODO: other error checking and filename parsing
	return poppler_document_new_from_file(filename,NULL,NULL);
}

int queryAnnots(PopplerDocument *PDF,int flags,int page_num) {
	PopplerPage *page;
	GList *annots;
	GList *list;
	gchar *text=NULL;
	PopplerAnnotMapping *mapping;
	int i,j;
	for (i=0; i<poppler_document_get_n_pages(PDF); i++) {
		page = poppler_document_get_page(PDF,i);
		annots = poppler_page_get_annot_mapping(page);	
		j=0;
		for(list = annots; list; list = list->next) {
			mapping = (PopplerAnnotMapping *)list->data;
			j++;
			text = poppler_annot_get_contents((PopplerAnnot *)mapping->annot);
			printf("Annotation #%d on page #%d:\n%s\n\n",j,i+1,text); 
			g_free(text);
		}
		poppler_page_free_annot_mapping(annots);
	}
	return 0;
}

int queryPageCount(PopplerDocument *PDF) {
	printf("%d\n",poppler_document_get_n_pages(PDF));
	return 0;
}

int queryData(PopplerDocument *PDF, int flags) {
/* TODO: this is not currently very useful
Eventually this function will parse the XML metadata, as well as have
options to scan deeper into the file to extract title, author, and other
such strings.  But for now, it is what it is.
*/
	gchar *metadata = poppler_document_get_metadata(PDF);
	printf("%s\n",metadata);
	g_free(metadata);
	return 0;
}
