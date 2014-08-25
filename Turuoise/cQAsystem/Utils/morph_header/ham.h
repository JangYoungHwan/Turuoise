/*
	File Name: ham.h
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
#define WINDOWS

#include <stdio.h>
#include <string.h>

#include "typedefs.h"
#include "sizedefs.h"
#include "runmode.h"
#include "parsek.h"

#define WELCOME \
	fprintf(stderr, "------------------------------------------------------------------\n"); \
	fprintf(stderr, "       Welcome to HAM(Hangul Analysis Module) version 5.0.0\n"); \
	fprintf(stderr, "   <<< BEFORE USING IT, YOU SHALL READ THE FILE LICENSE.TXT >>>\n"); \
	fprintf(stderr, "------------------------------------------------------------------\n")

#define GOODBYE \
	fprintf(stderr, "------------------------------------------------------------------\n"); \
	fprintf(stderr, "(c) 1993-2001 Kookmin Univ. Kang Seung-Shik, Tel.(+82-2)910-4800\n"); \
	fprintf(stderr, "    Email: sskang@kookmin.ac.kr, http://nlp.kookmin.ac.kr/\n"); \
	fprintf(stderr, "------------------------------------------------------------------\n")
/*--------------------------------- end of ham.h ---------------------------------*/
