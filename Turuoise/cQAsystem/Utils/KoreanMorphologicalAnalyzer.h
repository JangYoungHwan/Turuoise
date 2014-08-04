#ifndef _KOREAN_MORPHOLOGICAL_ANALYZER_H_
#define _KOREAN_MORPHOLOGICAL_ANALYZER_H_


#include <iostream>
#include <forward_list>
#include <map>
#include <set>
#include "Term.h"
#include "FreqScore.h"

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
	std::forward_list<Term<T_str, T_int>>* extractIndex2Flist(char* text);
	std::set<Term<T_str, T_int>>* extractIndex2Set(char* text);
	std::map<T_str, FreqScore<T_int, T_int>>* extractIndex2Map(char* text);
public:
	KoreanMorphologicalAnalyzer();
	virtual ~KoreanMorphologicalAnalyzer();

protected: /*============================= UTILS =============================*/
	unsigned char *load_text(FILE *fp) const;
};

template <typename T_str, typename T_int>
std::forward_list<Term<T_str, T_int>>* KoreanMorphologicalAnalyzer<T_str, T_int>::extractIndex2Flist(char* text)
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

template <typename T_str, typename T_int>
std::set<Term<T_str, T_int>>* KoreanMorphologicalAnalyzer<T_str, T_int>::extractIndex2Set(char* text)
{
	std::set<Term<T_str, T_int>>* pSetExtractResult = new std::set<Term<T_str, T_int>>();
	int n;
	n = get_terms_text(reinterpret_cast<unsigned char*>(text), mTerm, &mTM, &mMode, 0, 2, 1);

	for(int i=0; i<n; i++)
	{
		Term<T_str, T_int> term;

		std::string currentTerm((const char*)mTM.memTermString+mTerm[i].offset);

		term.setTerm(currentTerm);
		term.setTermFreq(static_cast<T_int>(mTerm[i].tf));
		term.setScore(mTerm[i].score);

		pSetExtractResult->insert(term);
	}

	return pSetExtractResult;
}

template <typename T_str, typename T_int>
std::map<T_str, FreqScore<T_int, T_int>>* KoreanMorphologicalAnalyzer<T_str, T_int>::extractIndex2Map(char* text)
{
	std::map<T_str, FreqScore<T_int, T_int>>* pMapExtractResult = new std::map<T_str, FreqScore<T_int, T_int>>();
	int n;
	n = get_terms_text(reinterpret_cast<unsigned char*>(text), mTerm, &mTM, &mMode, 0, 2, 1);

	for(int i=0; i<n; i++)
	{
		

		std::string currentTerm((const char*)mTM.memTermString+mTerm[i].offset);
		FreqScore<T_int, T_int> freq_score(term.setTermFreq(static_cast<T_int>(mTerm[i].tf)), term.setScore(mTerm[i].score));
		std::pair<T_str, FreqScore<T_int, T_int>> term_pair(currentTerm, freq_score);

		pMapExtractResult->insert(term_pair);
	}

	return pMapExtractResult;
}

template <typename T_str, typename T_int>
KoreanMorphologicalAnalyzer<T_str, T_int>::KoreanMorphologicalAnalyzer()
{
	if (!open_HAM_index(&mMode, NULL, "hdic/KLT2000.ini"))
		std::cerr << "Cannot load dictionary files" << std::endl;
}

template <typename T_str, typename T_int>
KoreanMorphologicalAnalyzer<T_str, T_int>::~KoreanMorphologicalAnalyzer()
{
	close_HAM_index(&mMode);	/* HAM is not used any more */
}

template <typename T_str, typename T_int>
unsigned char* KoreanMorphologicalAnalyzer<T_str, T_int>::load_text(FILE *fp) const
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


#endif