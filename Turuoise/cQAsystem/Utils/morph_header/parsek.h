/*
	File name: parsek.h
	Description: data structure for Korean parsing
	Written by: Kang, Seung-Shik	1998. 11. 17.
*/
#include "parsed.h"

typedef struct kparword {
	unsigned char *word;	/* input word */
	int wid;	/* i-th 'word' in the sentence */
	int offset;	/* byte-offset of 'word' */

	unsigned char *base;	/* base-form of 'word': with suffix */
		/* '��/��' �� ���ȭ ���̻簡 ���յ� ���� */
		/* 'ȭ/��/��' �� ���� ��Ʈ���� ���Ե� �� �ִ� �� ���� */
		/* '��/��' �� ���� ��Ʈ���� ���Ե� �� ���� ���� ���� ���� */
	unsigned char *stem;	/* stem-form of 'word': no suffixes */
		/* ���¼� �м������ stem�� ���� */
	unsigned char *suffix;
	
	unsigned char bpos;	/* P.O.S for 'base' */
	unsigned char spos;	/* P.O.S for 'stem' */

	unsigned char *josa;	/* ���� */
	unsigned char *eomi;	/* ���� */
	unsigned char *pomi;	/* ������ */
	unsigned char *xverb;	/* ������� */

	unsigned char rel;	/* dependency relation type? */
				/* ��� head: �־�, ������, �λ��, �λ� */
				/* ü�� head: ������, �������, ... */

	int nchild;	/* number of daughters */
	struct kparword *first;	/* first child */
	struct kparword *last;	/* last child */

	struct kparword *mom;	/* pointer to the mother node */
	struct kparword *left;	/* pointer to the left sister */
	struct kparword *right;	/* pointer to the right sister*/
} KPAR_WORD, *KPAR_PWORD;

/*                                                      */
/*       parsing result -- head-daughter structure      */
/*                                                      */
typedef struct kparsent {
	unsigned char *sent;	/* sentence string */
	KPAR_PWORD root;	/* root node */

	DPAR_PSENT dpr;	/* pointer to dep. parsing result */
} KPAR_SENT, *KPAR_PSENT;
/*---------------------- end of parsek.h ------------------------*/
