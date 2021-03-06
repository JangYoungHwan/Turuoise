#ifndef _QA_SYSTEM_H_
#define _QA_SYSTEM_H_


#include <string>
#include <vector>
#include "Utils/StdRedef.h"
#include "Utils/DocInfo.h"


class QAsystem
{
public:
	QAsystem() { };
	virtual ~QAsystem() { };
	
	// Phase 1 : Begin specified machine learning.
	virtual void beginTraning(String& srcDir, bool isTrained) = 0;

	// Phase 2 : Decode or analyze a query that the user ask an answer.
	virtual void analyzeQuery(String& query) = 0;

	// Phase 3 : Get score of all Q-A pairs in the list.
	virtual void calculateScore() = 0;
	virtual void calculateScore( const int scoring_func, const double mv_weight, const double mp_weight, const double libenstein_weight, const double synonym_weight) = 0;

	// Phase 4 : Refine score and print Q-A pairs in descending order.
	virtual const std::vector<DocInfo>* sortResult(const Integer show_limit) = 0;
	virtual void displayResult(const Integer show_limit) = 0;
	virtual void writeResult(const String &query, const Integer show_limit) = 0;
};


#endif