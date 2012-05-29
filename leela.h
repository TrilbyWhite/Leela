#ifndef __LEELA_H__
#define __LEELA_H__

// FLAGS: Q Query, S Search, W Write
#define Q_FLAG		0x1000
#define S_FLAG		0x2000
#define W_FLAG		0x4000

// SubFLAGS for Query: a annotations, p pages, d metadata
#define a_FLAG		0x0100
#define p_FLAG		0x0200
#define d_FLAG		0x0400

// OPTIONS						-Qa		-Qp		-Qd
#define t_OPT		0x0010	//	text	NA		title
#define i_OPT		0x0020	//	image	NA		institution/affiliation
#define v_OPT		0x0040	//	video	NA		volume/issue
#define a_OPT		0x0080	//	audio	NA		NA
#define l_OPT		0x00F0	//	link	NA		NA

// LIMITS
#define p_LIM		0x0001	// limit to page number

PopplerDocument *openPDF(const char *);
int querryAnnots(PopplerDocument *,int,int);
int querryPageCount(PopplerDocument *);
int querryData(PopplerDocument *,int);

#endif // __LEELA_H__
