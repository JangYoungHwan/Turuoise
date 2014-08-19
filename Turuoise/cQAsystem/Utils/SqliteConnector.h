#ifndef _SQLITE_CONNECTOR_H_
#define _SQLITE_CONNECTOR_H_


#include <iostream>
#include <io.h>
#include <string>
#include <vector>
#include <sys/stat.h>
#include <Windows.h>
#include <forward_list>
#include <map>
#include <set>
#include "../sqlite/sqlite3.h"
#include "StdRedef.h"
#include "Term.h"
#include "FreqScore.h"


class SqliteConnector
{
private:
	sqlite3 *mSqliteDB;
	String mDbName;

private:
	bool isDbAccessible();
	bool createDB();
	void setDbConfig();
	bool makeDB();	

public:
	SqliteConnector();
	SqliteConnector(String &db_name);
	~SqliteConnector();

	bool openDB();
	bool initDB();
	int getDocCount( );
	bool updateDB( std::string fname);
	bool updateDB( const std::forward_list<Term<String, Integer>>* words, int flag);
	bool updateDB( const std::set<Term<String, Integer>>* words, int strlen, int flag);
	bool updateSynonymTable( int term_id, String synonym);
	bool delete_m1_DB( int flag);
	bool closeDB();

public:
	Integer getWordID(const String &term);
	String getWord( int word_id);
	Integer getTF(const String &term, Integer doc, int flag);
	Integer getDF(const String &term, int flag);
	Real getIDF(const String &term, int flag);
	Integer getCountWordID();
	Integer getSumTermFreq();
	std::forward_list<Term<String, Integer>>* getDocInfoFlist(Integer doc_id, int flag);
	std::vector<Term<String, Integer>> getDocInfoVector(Integer doc_id, int flag);
	std::set<Term<String, Integer>>* getDocInfoSet(Integer doc_id, int flag);
	std::map<String, FreqScore<Integer, Integer>>* getDocInfoMap(Integer doc_id, int flag);
	std::vector<String> getSynonym( int word_id);

	// docid가 존재하지 않으면 -1, 존재하면 TEXTLENGTH반환
	int getDocTextLength( Integer doc_id, int flag);

private: // Utils
	std::vector< std::vector< String>> queryDB(const char* query);
	bool hasEnding(String const &fullString, String const &ending);

public: // Utils
	std::string UTF8ToANSI( const char *pszCode);
	std::string ANSIToUTF8( const char * pszCode);
	std::wstring utf8_to_utf16(const std::string& utf8);

	// 사용법 : get_levenshtein_distance( utf8_to_utf16( ANSIToUTF8( "대한민국")), utf8_to_utf16( ANSIToUTF8( "대A한A민국")))
	// UTF16 문자열 두개를 입력받음.
	double get_levenshtein_distance( const std::wstring& wcs1, const std::wstring& wcs2);
	bool initExistsDB() const;
};


#endif