/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/


/*
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved. The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 */
     
/*
 * Copyright (c) 1983, 1984 1985, 1986, 1987, 1988, Sun Microsystems, Inc.
 * All Rights Reserved.
 */

/*	from "e.h	1.5	05/06/02 SMI"	"ucbeqn:e.h 1.1" */

/*
 * Portions Copyright (c) 2005 Gunnar Ritter, Freiburg i. Br., Germany
 *
 * Sccsid @(#)e.h	1.5 (gritter) 8/13/05
 */

#include <stdio.h>

#define	FATAL	1
#define	ROM	'1'
#ifndef NEQN
#define	ITAL	'2'
#define	BLD	'3'
#else /* NEQN */
#define	ITAL	'1'
#define	BLD	'1'
#endif /* NEQN */

#ifndef NEQN
#define	VERT(n)	((((n)+1)/3)*3)
#define POINT	72
#define EM(m, ps)	(int)((((float)(m)*(ps) * resolution) / POINT))
#else /* NEQN */
#define	VERT(n)	(20 * (n))
#endif /* NEQN */
#define	EFFPS(p)	((p) >= 6 ? (p) : 6)

extern int	dbg;
extern int	ct;
extern int	lp[];
extern int	used[];	/* available registers */
extern int	ps;	/* dflt init pt size */
extern int	resolution;	/* resolution of ditroff */
extern int	deltaps;	/* default change in ps */
extern int	gsize;	/* global size */
extern int	gfont;	/* global font */
extern int	ft;	/* dflt font */
extern FILE	*curfile;	/* current input file */
extern int	ifile;	/* input file number */
extern int	linect;	/* line number in current file */
extern int	eqline;	/* line where eqn started */
extern int	svargc;
extern char	**svargv;
extern int	eht[];
extern int	ebase[];
extern int	lfont[];
extern int	rfont[];
extern int	yyval;
extern int	*yypv;
extern int	yylval;
extern int	eqnreg, eqnht;
extern int	lefteq, righteq;
extern int	lastchar;	/* last character read by lex */
extern int	markline;	/* 1 if this EQ/EN contains mark or lineup */
extern char	*progname;

typedef struct s_tbl {
	char	*name;
	char	*defn;
	struct s_tbl *next;
} tbl;
extern  char    *spaceval;  /* use in place of normal \x (for pic) */

/* diacrit.c */
void diacrit(int, int);
/* e.c */
int yyparse();
/* eqnbox.c */
void eqnbox(int, int, int);
/* font.c */
void setfont(char);
void font(int, int);
void fatbox(int);
void globfont(void);
/* fromto.c */
void fromto(int, int, int);
/* funny.c */
void funny(int);
/* glob.c */
/* integral.c */
void integral(int, int, int);
void setintegral(void);
/* io.c */
int main(int, char **);
void eqnexit(int);
int eqn(int, char **);
#define	getline(s, n)	eqngetline(s, n)
int getline(char **, size_t *);
void do_inline(void);
void putout(int);
int max(int, int);
int oalloc(void);
void ofree(int);
void setps(int);
void nrwid(int, int, int);
void setfile(int, char **);
void yyerror(char *);
void init(void);
void error(int, const char *, ...);
/* lex.c */
int gtc(void);
int openinfile(void);
void pbstr(register char *);
int yylex(void);
void getstr(char *, register int);
int cstr(char *, int, int);
void define(int);
void space(void);
char *strsave(char *);
void include(void);
void delim(void);
/* lookup.c */
tbl *lookup(tbl **, char *, char *);
void init_tbl(void);
/* mark.c */
void mark(int);
void lineup(int);
/* matrix.c */
void column(int, int);
void matrix(int);
/* move.c */
void move(int, int, int);
/* over.c */
void boverb(int, int);
/* paren.c */
void paren(int, int, int);
void brack(int, char *, char *, char *);
/* pile.c */
void lpile(int, int, int);
/* shift.c */
void bshiftb(int, int, int);
void shift(int);
void shift2(int, int, int);
/* size.c */
void setsize(char *);
void size(int, int);
void globsize(void);
/* sqrt.c */
#define	sqrt(n)	eqnsqrt(n)
void sqrt(int);
/* text.c */
void text(int, char *);
int trans(int, char *);
void shim(void);
void roman(int);
void name4(int, int);