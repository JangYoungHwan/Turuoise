#ifndef _KOREAN_MORPHOLOGICAL_ANALYZER_H_
#define _KOREAN_MORPHOLOGICAL_ANALYZER_H_


#include <iostream>
#include <forward_list>
#include <map>
#include "Term.h"

extern "C"
{
#include "KLT/ham-ndx.h"
}

template <typename T_str, typename T_int>
class KoreanMorphologicalAnalyzer
{
private:
	HAM_TERMLIST mTerm[MAX_TERMS_DOC];	/* extracted terms */
	HAM_TERMMEMORY mTM;					/* termtext.h: memories needed for term extraction */
	HAM_RUNMODE mMode;					/* HAM running mode: 'KLT/runmode.h' */

protected:
	std::forward_list<Term<T_str, T_int>>* extractIndex2List(char* text)
	{
		std::forward_list<Term<T_str, T_int>>* pFlistExtractResult = new std::forward_list<Term<T_str, T_int>>();
		int n;

		/*------------------------------------------------------------------------
		- �ټ���° ���� -- 0/n/-n(�ִ�� ����Ǵ� ����)
		n: �ִ� ���� ����, -n: �� n�̻�(or n �ۼ�Ʈ), 0: ��� ��� ����
		[����] n�� ���� ����: sortflag(���� ���)�� ���� '����ġ' or '���� �ɼ�'
		- sortflag�� ��� 1/2/3/4 -- ����ġ ������ n�� ����
		- sortflag�� ���� -1/-2/-3/-4 -- ���� ��Ŀ� ���� ���� �� n�� ����
		[����] n <= -10 �̸�, -n �ۼ�Ʈ��ŭ ������ �����.

		- ������° ���� -- 0/1/2/3/4(���� ���)
		0: ���� ����, 1: '������'��, 2: �󵵼�, 3: ����ġ, 4: ������ġ
		[����] n�� ���� ������ '���� ���'�� ���� �����Ϸ��� ������ ��

		- ������ ���� -- 0/1(������ġ numbering ���)
		0: ����������, 1: ���庰�� 100���� ����
		========================================================================*/
		n = get_terms_text(reinterpret_cast<unsigned char*>(text), mTerm, &mTM, &mMode, 0, 2, 1);

		auto iter = pFlistExtractResult->before_begin();
		for(int i=0; i<n; i++)
		{
			Term<T_str, T_int> term;

			std::string currentTerm((const char*)mTM.memTermString+mTerm[i].offset);
			
			term.setTerm(currentTerm);
			term.setTermFreq(static_cast<T_int>(mTerm[i].tf));
			term.setScore(mTerm[i].score);

			iter = pFlistExtractResult->insert_after(iter, term);
		}

		return pFlistExtractResult;
	}

public:
	KoreanMorphologicalAnalyzer()
	{
		if (open_HAM_index(&mMode, NULL, "hdic/KLT2000.ini"))
			std::cerr << "Cannot load dictionary files" << std::endl;
	}
	virtual ~KoreanMorphologicalAnalyzer()
	{
		close_HAM_index(&mMode);	/* HAM is not used any more */
	}

protected: /*============================= UTILS =============================*/
	unsigned char *load_text(FILE *fp) const
	{
		long n;
		unsigned char *p;

		fseek(fp, 0L, 2);
		n = ftell(fp);	/* n: byte size of file 'fp' */

		fseek(fp, 0L, 0);
		p = (unsigned char *) malloc(n+1);	/* memory allocation */
		if (p == NULL) return NULL;

		fread(p, sizeof(unsigned char), n, fp);	/* read 'fp' to 'p' */
		*(p+n) = '\0';

		return p;
	}
};


#endif