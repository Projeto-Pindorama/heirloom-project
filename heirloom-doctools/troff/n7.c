/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License, Version 1.0 only
 * (the "License").  You may not use this file except in compliance
 * with the License.
 *
 * You can obtain a copy of the license at usr/src/OPENSOLARIS.LICENSE
 * or http://www.opensolaris.org/os/licensing.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 *
 * When distributing Covered Code, include this CDDL HEADER in each
 * file and include the License file at usr/src/OPENSOLARIS.LICENSE.
 * If applicable, add the following below this CDDL HEADER, with the
 * fields enclosed by brackets "[]" replaced with your own identifying
 * information: Portions Copyright [yyyy] [name of copyright owner]
 *
 * CDDL HEADER END
 */
/*
 * Copyright 2003 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/


/*	from OpenSolaris "n7.c	1.10	05/06/08 SMI"	*/

/*
 * Portions Copyright (c) 2005 Gunnar Ritter, Freiburg i. Br., Germany
 *
 * Sccsid @(#)n7.c	1.48 (gritter) 5/2/06
 */

/*
 * University Copyright- Copyright (c) 1982, 1986, 1988
 * The Regents of the University of California
 * All Rights Reserved
 *
 * University Acknowledgment- Portions of this document are derived from
 * software developed by the University of California, Berkeley, and its
 * contributors.
 */

#include <stdlib.h>
#ifdef EUC
#ifdef NROFF
#ifdef	__sun
#include <widec.h>
#else
#include <wchar.h>
#endif
#include <limits.h>
#endif /* NROFF */
#endif /* EUC */
#include "tdef.h"
#ifdef NROFF
#include "tw.h"
#endif
#include "proto.h"
#ifdef NROFF
#define GETCH gettch
tchar	gettch();
#endif
#ifndef NROFF
#define GETCH getch
#endif

/*
 * troff7.c
 * 
 * text
 */

#include <ctype.h>
#ifdef EUC
#ifdef NROFF
#include <wctype.h>
#endif /* NROFF */
#endif /* EUC */
#include "ext.h"
#ifdef EUC
#ifdef NROFF
char	mbbuf2[MB_LEN_MAX + 1];
char	*mbbuf2p = mbbuf2;
tchar	mtbuf[MB_LEN_MAX + 1];
tchar	*mtbufp;
int	pendmb = 0;
wchar_t	cwc, owc, wceoll;
#endif /* NROFF */
#endif /* EUC */
int	brflg;

#undef	iswascii
#define	iswascii(c)	(((c) & ~(wchar_t)0177) == 0)

void
tbreak(void)
{
	register int pad, k;
	register tchar	*i, j, c;
	register int resol = 0;

	trap = 0;
	if (nb)
		return;
	if (dip == d && numtab[NL].val == -1) {
		newline(1);
		return;
	}
	if (!nc) {
		setnel();
		if (!wch)
			return;
		if (pendw)
			getword(1);
		movword();
	} else if (pendw && !brflg) {
		getword(1);
		movword();
	}
	*linep = dip->nls = 0;
#ifdef NROFF
	if (dip == d)
		horiz(po);
#endif
	if (lnmod)
		donum();
	lastl = ne;
	if (brflg != 1) {
		totout = 0;
	} else if (ad) {
		if ((lastl = ll - un) < ne)
			lastl = ne;
	}
	if (admod && ad && (brflg != 2)) {
		lastl = ne;
		adsp = adrem = 0;
		if (admod == 1)
			un +=  quant(nel / 2, HOR);
		else if (admod == 2)
			un += nel;
	}
	totout++;
	brflg = 0;
	if (lastl + un > dip->maxl)
		dip->maxl = lastl + un;
	horiz(un);
#ifdef NROFF
	if (adrem % t.Adj)
		resol = t.Hor; 
	else 
		resol = t.Adj;
#else
	resol = HOR;
#endif
	adrem = (adrem / resol) * resol;
	for (i = line; nc > 0; ) {
#ifndef EUC
		if ((c = cbits(j = *i++)) == ' ' || c == STRETCH) {
#else
#ifndef NROFF
		if ((c = cbits(j = *i++)) == ' ' || c == STRETCH) {
#else
		if ((c = cbits(j = *i++) & ~MBMASK) == ' ' || c == STRETCH) {
#endif /* NROFF */
#endif /* EUC */
			if (xflag && !fi && dilev || iszbit(j))
				goto std;
			pad = 0;
			if (i > line)
				pad += kernadjust(i[-2], ' '| i[-2]&SFMASK);
			do {
				pad += width(j);
				nc--;
#ifndef EUC
			} while ((c = cbits(j = *i++)) == ' ' ||
					c == STRETCH);
#else
#ifndef NROFF
			} while ((c = cbits(j = *i++)) == ' ' ||
					c == STRETCH);
#else
			} while ((c = cbits(j = *i++) & ~MBMASK) == ' ' ||
					c == STRETCH);
#endif /* NROFF */
#endif /* EUC */
			pad += kernadjust(i[-2], i[-1]);
			i--;
			pad += adsp;
			--nwd;
			if (adrem) {
				if (adrem < 0) {
					pad -= resol;
					adrem += resol;
				} else if ((totout & 01) || adrem / resol >= nwd) {
					pad += resol;
					adrem -= resol;
				}
			}
			pchar((tchar) WORDSP);
			horiz(pad);
		} else {
		std:	pchar(j);
			nc--;
		}
	}
	if (ic) {
		if ((k = ll - un - lastl + ics) > 0)
			horiz(k);
		pchar(ic);
	}
	if (icf)
		icf++;
	else 
		ic = 0;
	ne = nwd = 0;
	un = in;
	setnel();
	newline(0);
	if (dip != d) {
		if (dip->dnl > dip->hnl)
			dip->hnl = dip->dnl;
	} else {
		if (numtab[NL].val > dip->hnl)
			dip->hnl = numtab[NL].val;
	}
	for (k = ls - 1; k > 0 && !trap; k--)
		newline(0);
	spread = 0;
}

void
donum(void)
{
	register int i, nw;

	nrbits = nmbits;
	nw = width('1' | nrbits);
	if (nn) {
		nn--;
		goto d1;
	}
	if (numtab[LN].val % ndf) {
		numtab[LN].val++;
d1:
		un += nw * (3 + nms + ni);
		return;
	}
	i = 0;
	if (numtab[LN].val < 100)
		i++;
	if (numtab[LN].val < 10)
		i++;
	horiz(nw * (ni + i));
	nform = 0;
	fnumb(numtab[LN].val, pchar);
	un += nw * nms;
	numtab[LN].val++;
}

void
text(void)
{
	register tchar i, c, lasti = 0;
	int	k = 0;
	static int	spcnt;

	nflush++;
	numtab[HP].val = 0;
	if ((dip == d) && (numtab[NL].val == -1)) {
		newline(1); 
		return;
	}
	setnel();
	if (ce || !fi) {
		nofill();
		return;
	}
	if (pendw)
		goto t4;
	if (pendt)
		if (spcnt)
			goto t2; 
		else 
			goto t3;
	pendt++;
	if (spcnt)
		goto t2;
	while ((c = cbits(i = GETCH())) == ' ' || c == STRETCH) {
		if (iszbit(i))
			break;
		spcnt++;
		numtab[HP].val += sps;
		widthp = sps;
		lasti = i;
	}
	if (lasti) {
		k = kernadjust(lasti, i);
		numtab[HP].val += k;
		widthp += k;
	}
	if (nlflg) {
t1:
		nflush = pendt = ch = spcnt = 0;
		callsp();
		return;
	}
	ch = i;
	if (spcnt) {
t2:
		tbreak();
		if (nc || wch)
			goto rtn;
		un += spcnt * sps + k;
		spcnt = 0;
		setnel();
		if (trap)
			goto rtn;
		if (nlflg)
			goto t1;
	}
t3:
	if (spread)
		goto t5;
	if (pendw || !wch)
t4:
		if (getword(0))
			goto t6;
	if (!movword())
		goto t3;
t5:
	if (nlflg)
		pendt = 0;
	adsp = adrem = 0;
	if (ad) {
#ifndef	NROFF
		if (nc > 0) {
			c = line[nc-1];
			width(c);
			nel += lasttrack;
			nel += kernadjust(c, ' ' | c&SFMASK);
			if (admod != 1 && rhangtab != NULL && !ismot(c) &&
					rhangtab[xfont] != NULL &&
					(k = rhangtab[xfont][cbits(c)]) != 0) {
				k = (k * u2pts(xpts) + (Unitwidth / 2))
					/ Unitwidth;
				nel += k;
			}
		}
#endif	/* !NROFF */
		if (nwd == 1)
			adsp = nel; 
		else 
			adsp = nel / (nwd - 1);
		adsp = (adsp / HOR) * HOR;
		adrem = nel - adsp*(nwd-1);
		if (admod == 0 && nwd == 1 && warn & WARN_BREAK)
			errprint("can't break line");
		else if (admod == 0 && spreadwarn && adsp >= spreadlimit)
			errprint("spreadlimit exceeded, %gm", (double)adsp/EM);
	}
	brflg = 1;
	tbreak();
	spread = 0;
	if (!trap)
		goto t3;
	if (!nlflg)
		goto rtn;
t6:
	pendt = 0;
	ckul();
rtn:
	nflush = 0;
}


void
nofill(void)
{
	register int j;
	register tchar i, nexti;
	int k, oev;

	if (!pendnf) {
		over = 0;
		tbreak();
		if (trap)
			goto rtn;
		if (nlflg) {
			ch = nflush = 0;
			callsp();
			return;
		}
		adsp = adrem = 0;
		nwd = 10000;
	}
	nexti = GETCH();
#ifndef	NROFF
	if (!ce && !pendnf && lhangtab != NULL && cbits(nexti) != SLANT &&
			!ismot(nexti) && lhangtab[fbits(nexti)] != NULL &&
			(k = lhangtab[fbits(nexti)][cbits(nexti)]) != 0) {
		width(nexti);	/* set xpts */
		k = (k * u2pts(xpts) + (Unitwidth / 2)) / Unitwidth;
		storeline(makem(k), 0);
	}
#endif	/* !NROFF */
	while ((j = (cbits(i = nexti))) != '\n') {
		if (j == ohc) {
			nexti = GETCH();
			continue;
		}
		if (j == CONT) {
			pendnf++;
			nflush = 0;
			flushi();
			ckul();
			return;
		}
		j = width(i);
		widthp = j;
		numtab[HP].val += j;
		storeline(i, j);
		oev = ev;
		nexti = GETCH();
		if (ev == oev) {
			k = kernadjust(i, nexti);
			ne += k;
			nel -= k;
			numtab[HP].val += k;
		}
	}
	if (ce) {
		ce--;
		if ((i = quant(nel / 2, HOR)) > 0)
			un += i;
	}
	if (!nc)
		storeline((tchar)FILLER, 0);
	brflg = 2;
	tbreak();
	ckul();
rtn:
	pendnf = nflush = 0;
}


void
callsp(void)
{
	register int i;

	if (flss)
		i = flss; 
	else 
		i = lss;
	flss = 0;
	casesp(i);
}


void
ckul(void)
{
	if (ul && (--ul == 0)) {
		cu = 0;
		font = sfont;
		mchbits();
	}
	if (it && (--it == 0) && itmac)
		control(itmac, 0);
}


void
storeline(register tchar c, int w)
{
	if (linep >= line + lnsize - 1) {
		if (!over) {
			flusho();
			errprint("Line overflow.");
			over++;
			c = LEFTHAND;
			w = -1;
			goto s1;
		}
		return;
	}
s1:
	if (w == -1)
		w = width(c);
	ne += w;
	nel -= w;
	*linep++ = c;
	nc++;
}


void
newline(int a)
{
	register int i, j, nlss = 0;
	int	opn;

	if (a)
		goto nl1;
	if (dip != d) {
		j = lss;
		pchar1((tchar)FLSS);
		if (flss)
			lss = flss;
		i = lss + dip->blss;
		dip->dnl += i;
		pchar1((tchar)i);
		pchar1((tchar)'\n');
		lss = j;
		dip->blss = flss = 0;
		if (dip->alss) {
			pchar1((tchar)FLSS);
			pchar1((tchar)dip->alss);
			pchar1((tchar)'\n');
			dip->dnl += dip->alss;
			dip->alss = 0;
		}
		if (vpt > 0 && dip->ditrap && !dip->ditf && dip->dnl >= dip->ditrap && dip->dimac)
			if (control(dip->dimac, 0)) {
				trap++; 
				dip->ditf++;
			}
		return;
	}
	j = lss;
	if (flss)
		lss = flss;
	nlss = dip->alss + dip->blss + lss;
	numtab[NL].val += nlss;
#ifndef NROFF
	if (ascii) {
		dip->alss = dip->blss = 0;
	}
#endif
	pchar1((tchar)'\n');
	flss = 0;
	lss = j;
	if (vpt == 0 || numtab[NL].val < pl)
		goto nl2;
nl1:
	ejf = dip->hnl = numtab[NL].val = 0;
	ejl = frame->tail_cnt;
	if (donef) {
		if ((!nc && !wch) || ndone)
			done1(0);
		ndone++;
		donef = 0;
		if (frame == stk)
			nflush++;
	}
	opn = numtab[PN].val;
	numtab[PN].val++;
	if (npnflg) {
		numtab[PN].val = npn;
		npn = npnflg = 0;
	}
nlpn:
	if (numtab[PN].val == pfrom) {
		print++;
		pfrom = -1;
	} else if (opn == pto) {
		print = 0;
		opn = -1;
		chkpn();
		goto nlpn;
	}
	if (print)
		newpage(numtab[PN].val);	/* supposedly in a clean state so can pause */
	if (stop && print) {
		dpn++;
		if (dpn >= stop) {
			dpn = 0;
			dostop();
		}
	}
nl2:
	trap = 0;
	if (vpt <= 0)
		/*EMPTY*/;
	else if (numtab[NL].val == 0) {
		if ((j = findn(0)) != NTRAP)
			trap = control(mlist[j], 0);
	} else if ((i = findt(numtab[NL].val - nlss)) <= nlss) {
		if ((j = findn1(numtab[NL].val - nlss + i)) == NTRAP) {
			flusho();
			errprint("Trap botch.");
			done2(-5);
		}
		trap = control(mlist[j], 0);
	}
}


int 
findn1(int a)
{
	register int i, j;

	for (i = 0; i < NTRAP; i++) {
		if (mlist[i]) {
			if ((j = nlist[i]) < 0)
				j += pl;
			if (j == a)
				break;
		}
	}
	return(i);
}


void
chkpn(void)
{
	pto = *(pnp++);
	pfrom = pto>=0 ? pto : -pto;
	if (pto == -32767) {
		flusho();
		done1(0);
	}
	if (pto < 0) {
		pto = -pto;
		print++;
		pfrom = 0;
	}
}


int 
findt(int a)
{
	register int i, j, k;

	k = MAXMOT;
	if (dip != d) {
		if (dip->dimac && (i = dip->ditrap - a) > 0)
			k = i;
		return(k);
	}
	for (i = 0; i < NTRAP; i++) {
		if (mlist[i]) {
			if ((j = nlist[i]) < 0)
				j += pl;
			if ((j -= a) <= 0)
				continue;
			if (j < k)
				k = j;
		}
	}
	i = pl - a;
	if (k > i)
		k = i;
	return(k);
}


int 
findt1(void)
{
	register int i;

	if (dip != d)
		i = dip->dnl;
	else 
		i = numtab[NL].val;
	return(findt(i));
}


void
eject(struct s *a)
{
	register int savlss;

	if (dip != d)
		return;
	if (vpt == 0) {
		if (donef == 0) {
			errprint("page not ejected because traps are disabled");
			return;
		}
		errprint("page forcefully ejected although traps are disabled");
		vpt = -1;
	}
	ejf++;
	if (a)
		ejl = a->tail_cnt;
	else 
		ejl = frame->tail_cnt;
	if (trap)
		return;
e1:
	savlss = lss;
	lss = findt(numtab[NL].val);
	newline(0);
	lss = savlss;
	if (numtab[NL].val && !trap)
		goto e1;
}


int
movword(void)
{
	register int w;
	register tchar i, *wp, c, *lp, *lastlp, lasti = 0, *tp;
	int	savwch, hys, stretches = 0;
#ifndef	NROFF
	tchar	lgs, lge;
	int	*ip, s, lgw;
#endif

	over = 0;
	wp = wordp;
	if (!nwd) {
#ifndef EUC
		while ((c = cbits(i = *wp++)) == ' ') {
#else
#ifndef NROFF
		while ((c = cbits(i = *wp++)) == ' ') {
#else
		while ((c = cbits(i = *wp++) & ~MBMASK) == ' ') {
#endif /* NROFF */
#endif /* EUC */
			if (iszbit(i))
				break;
			wch--;
			wne -= sps;
		}
		wp--;
		if (wp > wordp)
			wne -= kernadjust(wp[-1], wp[0]);
#ifndef	NROFF
		if (admod != 1 && admod != 2 && lhangtab != NULL &&
				cbits(*wp) != SLANT && !ismot(*wp) &&
				lhangtab[fbits(*wp)] != NULL &&
				(w = lhangtab[fbits(*wp)][cbits(*wp)]) != 0) {
			width(*wp);	/* set xpts */
			w = (w * u2pts(xpts) + (Unitwidth / 2)) / Unitwidth;
			nel -= w;
			storeline(makem(w), 0);
		}
#endif	/* !NROFF */
	}
	if (wne > nel && !hyoff && hyf && (!nwd || nel > 3 * sps) &&
	   (!(hyf & 02) || (findt1() > lss)))
		hyphen(wp);
	savwch = wch;
	hyp = hyptr;
	nhyp = 0;
	while (*hyp && *hyp <= wp)
		hyp++;
	while (wch) {
		tp = (tchar *)((intptr_t)*hyp & ~(intptr_t)03);
		if (hyoff != 1 && tp == wp) {
			if (!wdstart || (wp > wdstart + 1 && wp < wdend &&
			   (!(hyf & 04) || wp < wdend - 1) &&		/* 04 => last 2 */
			   (!(hyf & 010) || wp > wdstart + 2))) {	/* 010 => 1st 2 */
				nhyp++;
				i = IMP;
				setsbits(i, (intptr_t)*hyp & 03);
				storeline(i, 0);
			}
			hyp++;
		}
		i = *wp++;
		w = width(i);
		w += kernadjust(i, *wp ? *wp : ' ' | i&SFMASK);
		wne -= w;
		wch--;
		if (cbits(i) == STRETCH && cbits(lasti) != STRETCH)
			stretches++;
		lasti = i;
		storeline(i, w);
	}
	*linep = *wp;
	lastlp = linep;
	if (nel >= 0) {
		nwd += stretches + 1;
		return(0);	/* line didn't fill up */
	}
#ifndef NROFF
	xbits((tchar)HYPHEN, 1);
#endif
	hys = width((tchar)HYPHEN);
m1:
	if (!nhyp) {
		if (!nwd)
			goto m3;
		if (wch == savwch)
			goto m4;
	}
	if (cbits(*--linep) != IMP)
		goto m5;
#ifndef	NROFF
	if ((s = sbits(*linep)) != 0) {
		i = *(linep + 1);
		if ((ip = lgrevtab[fbits(i)][cbits(i)]) == NULL)
			goto m5;
		lgs = strlg(fbits(i), ip, s) | i & SFMASK | AUTOLIG;
		for (w = 0; ip[s+w]; w++);
		lge = strlg(fbits(i), &ip[s], w) | i & SFMASK | AUTOLIG;
		lgw = width(lgs);
		if (linep - 1 >= wordp) {
			lgw += kernadjust(i, *(linep - 1));
			lgw -= kernadjust(*(linep + 1), *(linep - 1));
		}
		hys += lgw;
	} else {
		lgs = 0;
		lge = 0;
		lgw = 0;
	}
#endif	/* !NROFF */
	if (!(--nhyp))
		if (!nwd)
			goto m2;
	if (nel < hys) {
		nc--;
		goto m1;
	}
m2:
#ifndef	NROFF
	if (lgs != 0) {
		*wp = lge;
		storeline(lgs, lgw);
	}
#endif	/* !NROFF */
	if ((i = cbits(*(linep - 1))) != '-' && i != EMDASH &&
			(*(linep - 1) & BLBIT) == 0) {
		*linep = (*(linep - 1) & SFMASK) | HYPHEN;
		w = -kernadjust(*(linep - 1), *(linep + 1));
		w += kernadjust(*(linep - 1), *linep);
		w += width(*linep);
		w += kernadjust(*linep, ' ' | *linep & SFMASK);
		nel -= w;
		ne += w;
		linep++;
	}
m3:
	nwd++;
m4:
	wordp = wp;
	return(1);	/* line filled up */
m5:
	nc--;
	for (lp = &linep[1]; lp < lastlp && cbits(*lp) == IMP; lp++);
	w = width(*linep);
	w += kernadjust(*linep, *lp ? *lp : ' ' | *linep&SFMASK);
	ne -= w;
	nel += w;
	wne += w;
	wch++;
	wp--;
	goto m1;
}


void
horiz(int i)
{
	vflag = 0;
	if (i)
		pchar(makem(i));
}


void
setnel(void)
{
	if (!nc) {
		linep = line;
		if (un1 >= 0) {
			un = un1;
			un1 = -1;
		}
		nel = ll - un;
		ne = adsp = adrem = 0;
	}
}


int
getword(int x)
{
	register int j, k = 0;
	register tchar i, *wp, nexti, gotspc = 0;
	int noword;
#ifdef EUC
#ifdef NROFF
	wchar_t *wddelim;
	char mbbuf3[MB_LEN_MAX + 1];
	char *mbbuf3p;
	int wbf, n;
	tchar m;
#endif /* NROFF */
#endif /* EUC */

	noword = 0;
	if (x)
		if (pendw) {
			*pendw = 0;
			goto rtn;
		}
	if (wordp = pendw)
		goto g1;
	hyp = hyptr;
	wordp = word;
	over = wne = wch = 0;
	hyoff = 0;
#ifdef EUC
#ifdef NROFF
	mtbufp = mtbuf;
	if (pendmb) {
		while(*mtbufp) {
			switch(*mtbufp & MBMASK) {
			case LASTOFMB:
			case BYTE_CHR:
				storeword(*mtbufp++, -1);
				break;

			default:
				storeword(*mtbufp++, 0);
			}
		}
		mtbufp = mtbuf;
		pendmb = 0;
		goto g1;
	}
#endif /* NROFF */
#endif /* EUC */
	while (1) {	/* picks up 1st char of word */
		j = cbits(i = GETCH());
#ifdef EUC
#ifdef NROFF
		if (multi_locale)
			if (collectmb(i))
				continue;
#endif /* NROFF */
#endif /* EUC */
		if (j == '\n') {
			wne = wch = 0;
			noword = 1;
			goto rtn;
		}
		if (j == ohc) {
			hyoff = 1;	/* 1 => don't hyphenate */
			continue;
		}
		if (j == ' ' && !iszbit(i)) {
			numtab[HP].val += sps;
			widthp = sps;
			storeword(i, sps);
			gotspc = i;
			continue;
		}
		if (gotspc) {
			k = kernadjust(gotspc, i);
			numtab[HP].val += k;
			wne += k;
			widthp += k;
		}
		break;
	}
#ifdef EUC
#ifdef NROFF
	if (!multi_locale)
		goto a0;
	if (wddlm && iswprint(wceoll) && iswprint(cwc) &&
	    (!iswascii(wceoll) || !iswascii(cwc)) &&
	    !iswspace(wceoll) && !iswspace(cwc)) {
		wddelim = (*wddlm)(wceoll, cwc, 1);
		wceoll = 0;
		if (*wddelim != ' ') {
			if (!*wddelim) {
				storeword(((*wdbdg)(wceoll, cwc, 1) < 3) ?
					  ZWDELIM(1) : ZWDELIM(2), 0);
			} else {
				while (*wddelim) {
					if ((n = wctomb(mbbuf3, *wddelim++))
					    > 0) {
						mbbuf3[n] = 0;
						n--;
						mbbuf3p = mbbuf3 + n;
						while(n) {
							m = *(mbbuf3p-- - n--) &
							    0xff | MIDDLEOFMB |
							    ZBIT;
							storeword(m, 0);
						}
						m = *mbbuf3p & 0xff | LASTOFMB;
						storeword(m, -1);
					} else {
						storeword(' ' | chbits, sps);
						break;
					}
				}
			}
			spflg = 0;
			goto g0;
		}
	}
a0:
#endif /* NROFF */
#endif /* EUC */
	storeword(' ' | chbits, sps + k);
	if (spflg) {
		if (xflag == 0 || ses != 0)
			storeword(' ' | chbits, sps);
		spflg = 0;
	}
g0:
	if (j == CONT) {
		pendw = wordp;
		nflush = 0;
		flushi();
		return(1);
	}
	if (hyoff != 1) {
		if (j == ohc) {
			hyoff = 2;
			*hyp++ = wordp;
			if (hyp > (hyptr + NHYP - 1))
				hyp = hyptr + NHYP - 1;
			if (isblbit(i) && wordp > word)
				wordp[-1] |= BLBIT;
			goto g1;
		}
		if (j == '-' || j == EMDASH)
			if (wordp > word + 1) {
				if (!hyext)
					hyoff = 2;
				*hyp++ = wordp + 1;
				if (hyp > (hyptr + NHYP - 1))
					hyp = hyptr + NHYP - 1;
			}
	}
	j = width(i);
	numtab[HP].val += j;
#ifndef EUC
	storeword(i, j);
#else
#ifndef NROFF
	storeword(i, j);
#else
	if (multi_locale) {
		mtbufp = mtbuf;
		while(*mtbufp) {
			switch(*mtbufp & MBMASK) {
			case LASTOFMB:
			case BYTE_CHR:
				storeword(*mtbufp++, j);
				break;

			default:
				storeword(*mtbufp++, 0);
			}
		}
		mtbufp = mtbuf;
	} else {
		storeword(i, j);
	}
#endif /* NROFF */
#endif /* EUC */
	if (1) {
		int	oev = ev;
		nexti = GETCH();
		if (ev == oev) {
			k = kernadjust(i, nexti);
			wne += k;
			widthp += k;
			numtab[HP].val += k;
		}
	} else
g1:		nexti = GETCH();
	j = cbits(i = nexti);
#ifdef EUC
#ifdef NROFF
	if (multi_locale)
		if (collectmb(i))
			goto g1;
#endif /* NROFF */
#endif /* EUC */
	if (j != ' ' || iszbit(i)) {
		static char *sentchar = ".?!:";	/* sentence terminators */
		if (j != '\n')
#ifdef EUC
#ifdef NROFF
			if (!multi_locale)
#endif /* NROFF */
#endif /* EUC */
			goto g0;
#ifdef EUC
#ifdef NROFF
			else {
				if (!wdbdg || (iswascii(cwc) && iswascii(owc)))
					goto g0;
				if ((wbf = (*wdbdg)(owc, cwc, 1)) < 5) {
					pendmb++;
					storeword((wbf < 3) ? ZWDELIM(1) :
						  ZWDELIM(2), 0);
					*wordp = 0;
					goto rtn;
				} else goto g0;
			}
#endif /* NROFF */
#endif /* EUC */
		wp = wordp-1;	/* handle extra space at end of sentence */
		while (wp >= word) {
			j = cbits(*wp--);
			if (j=='"' || j=='\'' || j==')' || j==']' || j=='*' || j==DAGGER)
				continue;
			for (k = 0; sentchar[k]; k++)
				if (j == sentchar[k]) {
					spflg++;
					break;
				}
			break;
		}
	}
#ifdef EUC
#ifdef NROFF
	wceoll = owc;
#endif /* NROFF */
#endif /* EUC */
	*wordp = 0;
	numtab[HP].val += sps;
rtn:
	for (wp = word; *wp; wp++) {
		j = cbits(*wp);
		if ((j == ' ' || j == STRETCH) && !iszbit(j))
			continue;
		if (!ischar(j) || (!isdigit(j) && j != '-'))
			break;
	}
	if (*wp == 0)	/* all numbers, so don't hyphenate */
		hyoff = 1;
	wdstart = 0;
	wordp = word;
	pendw = 0;
	*hyp++ = 0;
	setnel();
	return(noword);
}


void
storeword(register tchar c, register int w)
{

	if (wordp >= &word[WDSIZE - 3]) {
		if (!over) {
			flusho();
			errprint("Word overflow.");
			over++;
			c = LEFTHAND;
			w = -1;
			goto s1;
		}
		return;
	}
s1:
	if (w == -1)
		w = width(c);
	widthp = w;
	wne += w;
	*wordp++ = c;
	wch++;
}


#ifdef NROFF
tchar gettch(void)
{
	extern int c_isalnum;
	tchar i;
	int j;

	i = getch();
	j = cbits(i);
	if (ismot(i) || fbits(i) != ulfont)
		return(i);
	if (cu) {
		if (trtab[j] == ' ') {
			setcbits(i, '_');
			setfbits(i, FT);	/* default */
		}
		return(i);
	}
	/* should test here for characters that ought to be underlined */
	/* in the old nroff, that was the 200 bit on the width! */
	/* for now, just do letters, digits and certain special chars */
	if (j <= 127) {
		if (!isalnum(j))
			setfbits(i, FT);
	} else {
		if (j < c_isalnum)
			setfbits(i, FT);
	}
	return(i);
}


#endif
#ifdef EUC
#ifdef NROFF
int
collectmb(tchar i)
{
	int busy;

	*mtbufp++ = i;
	*mbbuf2p++ = i & BYTEMASK;
	*mtbufp = *mbbuf2p = 0;
	if (ismot(i)) {
		mtbufp = mtbuf;
		mbbuf2p = mbbuf2;
		owc = 0;
		cwc = 0;
		return(busy = 0);
	}
	if ((i & MBMASK) == MIDDLEOFMB) {
		if (mtbufp <= (mtbuf + mb_cur_max)) {
			busy = 1;
		} else {
			*(mtbufp - 1) &= ~MBMASK;
			goto gotmb;
		}
	} else {
		if ((i & MBMASK) == LASTOFMB)
			*(mtbufp - 1) &= ~MBMASK;
gotmb:
		mtbufp = mtbuf;
		owc = cwc;
		if ((*mbbuf2&~(wchar_t)0177) == 0) {
			cwc = *mbbuf2;
		} else if (mbtowc(&cwc, mbbuf2, mb_cur_max) <= 0) {
			mtbufp = mtbuf;
			while (*mtbufp) {
				setcbits(*mtbufp, (*mtbufp & 0x1ff));
				mtbufp++;
			}
			mtbufp = mtbuf;
		}
		mbbuf2p = mbbuf2;
		busy = 0;
	}
	return(busy);
}


#endif /* NROFF */
#endif /* EUC */
