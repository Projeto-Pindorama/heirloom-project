#if __GNUC__ >= 3 && __GNUC_MINOR__ >= 4
#define	USED	__attribute__ ((used))
#elif defined __GNUC__
#define	USED	__attribute__ ((unused))
#else
#define	USED
#endif
#if defined (SU3)
static const char sccsid[] USED = "@(#)sed_su3.sl	2.30 (gritter) 2/6/05";
#elif defined (SUS)
static const char sccsid[] USED = "@(#)sed_sus.sl	2.30 (gritter) 2/6/05";
#elif defined (S42)
static const char sccsid[] USED = "@(#)sed_s42.sl	2.30 (gritter) 2/6/05";
#else	/* !SUS, !SU3, !S42 */
static const char sccsid[] USED = "@(#)sed.sl	2.30 (gritter) 2/6/05";
#endif	/* !SUS, !SU3, !S42 */

/*
sed.h:
	sed.h	1.32 (gritter) 2/6/05
sed0.c:
	sed0.c	1.63 (gritter) 2/6/05
sed1.c:
	sed1.c	1.42 (gritter) 2/6/05
*/