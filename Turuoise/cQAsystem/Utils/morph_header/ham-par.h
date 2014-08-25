/*
	File Name: ham-par.h
	Description: header file collections for HAM
	Written by: Kang, Seung-Shik	04/11/1997
*/

/*-------------------------------------------------------------
	Compile options
---------------------------------------------------------------
	1. ����95���� HAM�� compile & link�� ���� ���ǵǾ�� ��!

		#define WINDOWS  --> unix�迭�� �ʿ� ����!

	2. COM���� ���۵Ǵ� macro ���Ǵ� �ʿ�ġ ����
		���̺귯�� ���� ���� ���ǹǷ�...
-------------------------------------------------------------*/
/*#define WINDOWS*/

#include <stdio.h>
#include <string.h>

//#include "sizedefs.h"	/* size definitions */
#include "typedefs.h"	/* type definitions */
#include "runmode.h"	/* running options */
#include "hamout1.h"	/* morph. analysis structure */
#include "parsek.h"		/* parsing result structure */

#include "pos-tag.h"	/* POS tagset for parsing result */

/*
	Following external functions are defined at HAM library.
	For Windows 95 DLL, external functions are imported.
*/
extern _declspec(dllimport) short open_HAM_parse(HAM_RUNMODE *, char *, char *);
extern _declspec(dllimport) KPAR_PSENT parsing(unsigned char *, HAM_RUNMODE *);
extern _declspec(dllimport) void close_HAM_parse(void);

/* USED ONLY FOR 'ADJUST_WORD_SPACING' OF INPUT SENTENCE */
extern _declspec(dllimport) int load_bigram_data(char *filename);
extern _declspec(dllimport) void adjust_word_spacing(unsigned char sent[], int);
extern _declspec(dllimport) void free_bigram_data();
/*--------------------------------- end of ham-par.h ---------------------------------*/
