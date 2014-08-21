#include "FreqBasedTrainer.h"

FreqBasedTrainer::FreqBasedTrainer()
{

}

FreqBasedTrainer::FreqBasedTrainer(SqliteConnector* SqlConnector)
	:BaseTrainer(SqlConnector)
{

}

FreqBasedTrainer::~FreqBasedTrainer()
{
	
}


bool FreqBasedTrainer::beginTraning(String& trainingPath, std::map<Integer, String>& docID2Path)
{
	std::cout << "beginTraning" << std::endl;

	// traverse all files 
	//std::forward_list<String>* pFlist = new std::forward_list<String>();
	std::list<String>* pFlist = new std::list<String>();
	readDirectory(trainingPath.c_str(), pFlist);

	std::cout << "XML�Ľ���!" << std::endl;
	auto cur_progress = 0;
	auto numOfFiles = std::distance(pFlist->begin(), pFlist->end())-1;
	for(String& xmlFile : *pFlist)
	{
		std::pair<Integer, String> int2str_pair(cur_progress, xmlFile);
		docID2Path.insert(int2str_pair);

		mProgressBar->dispalyPrgressBar(cur_progress++, numOfFiles);
		mXmlParser = new RapidXmlParser(xmlFile);
		mXmlParser->runParsing();

		if(!mSqlConnector->updateDB( xmlFile))
			return false;

		const std::set<Term<String, Integer>>* extractWords = nullptr;

		extractWords = extractIndex2Set(mSqlConnector->UTF8ToANSI(mXmlParser->getQuestionColumn()).c_str());
		if(!mSqlConnector->updateDB(extractWords, strlen( mSqlConnector->UTF8ToANSI(mXmlParser->getQuestionColumn()).c_str()), QUESTION))
			return false;

		extractWords = extractIndex2Set(mSqlConnector->UTF8ToANSI(mXmlParser->getAnswerColumn()).c_str());
		if(!mSqlConnector->updateDB(extractWords, strlen( mSqlConnector->UTF8ToANSI(mXmlParser->getAnswerColumn()).c_str()), ANSWER))
			return false;

		delete extractWords;
		delete mXmlParser;
	}
	delete pFlist;

	mSqlConnector->delete_m1_DB( QUESTION);
	mSqlConnector->delete_m1_DB( ANSWER);
	std::cout<<std::endl;
	/*
#pragma comment( lib, "urlmon.lib")
#pragma comment( lib, "OleAut32.lib")

	std::cout << "���Ǿ������߱��ڹٲ�" << std::endl;
	LPWSTR dic_url = L"http://dic.naver.com/search.nhn?&query=";
	int wordnum = mSqlConnector->getCountWordID();
	for( int i = 0 ; i < wordnum ; i++) {
		mProgressBar->dispalyPrgressBar(i, wordnum-1);
		
		String term = mSqlConnector->getWord( i);

		std::wstring url = dic_url;
		url += mSqlConnector->utf8_to_utf16( term);

		HRESULT hr = URLDownloadToFile( NULL, url.c_str(), L"Result.html", NULL, NULL);

		if( hr == S_OK) {
		}
		else {
			std::cout << "���ͳ�Ȯ���ض�!!!!!!!!!!" << std::endl;
		}

		std::ifstream fin( "Result.html");
		if( fin.is_open() == false)
			return 0;

		std::vector< std::string> vec_synonym;
		char buf[ 4096];
		while( fin.getline( buf, 4096)) {
			if( *buf == '\0' || *buf == '\n' || *buf == '\r')
				continue;

			char* ptrbuf = buf;
			int n = 0;
			for( ; ( *ptrbuf == ' ' || *ptrbuf == '\t') && n < strlen( buf) ; n++, ptrbuf++);
		
			if( n == strlen( buf))
				continue;

			std::string line_str = mSqlConnector->UTF8ToANSI( ptrbuf);

			if( strstr( line_str.c_str(), "[���Ǿ�]") != NULL) {   // ���Ǿ ���� ��� ù��° [���Ǿ�]�� ��� �ι�°�� ����... �׿�
				while( fin.getline( buf, 4096)) {
				
					if( strstr( buf, "</p>") != NULL)
						break;

					line_str = mSqlConnector->UTF8ToANSI( buf);

					std::tr1::cmatch res;
					std::tr1::regex rx("<a.*\">([^<]+)");
					int num = std::tr1::regex_search( line_str.c_str(), res, rx);
												
					if( num > 0)
						vec_synonym.push_back( res[ 1]);
				}

				break;	// ��� ���Ǿ ����
			}
		}

		//std::wcout << url << "!!!" << std::endl;
		for( int n = 0 ; n < vec_synonym.size() ; n++)
			mSqlConnector->updateSynonymTable( i, mSqlConnector->ANSIToUTF8( vec_synonym[ n].c_str()));

		fin.close();
	}
	remove( "Result.html");*/

	return true;
}