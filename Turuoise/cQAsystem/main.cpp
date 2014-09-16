/*
 * Title	: CQA system implemented in C++
 * Author	: YW. Jang, JI. Jo
 */


#include <iostream>
#include <string>
#include <time.h>
#include <codecvt>
#include "QAsystem.h"
#include "CQAsystem.h"
#include "Utils/stringutil.h"
#include "Utils\MorphemeAnalyzer.hpp"


std::vector< SynonymTerm> mSynonymTable;
std::map< Integer, String> mWordIDTable;

//#define _TRAINING_MODE_
#define OPTION_FILE_NAME "option.txt"
enum { FUNC_COSINE = 0, FUNC_BM25};

struct QueryInput
{
	int qum;
	String query;
};

class Optstruct {
	public:
		bool re_training;
		String training_xml_folder_path;
		bool get_synonym_data;
		String synonym_data_file_name;
		String db_name;
		int scoring_func;
		double question_weight;
		double answer_weight;
		double liebenstein_weight;
		double synonym_weight;
		int display_limit;
		
		Optstruct() {
			re_training = false;
			training_xml_folder_path = "../../../../training_data/";
			get_synonym_data = false;
			synonym_data_file_name = "synonym.txt";
			db_name = "Turuoise.db";
			scoring_func = FUNC_COSINE;
			question_weight = 0.8;
			answer_weight = 0.2;
			liebenstein_weight = 1;
			synonym_weight = 0.7;
			display_limit = 10;
		};

		void createDefalutOptionFile( ) { 
			std::ofstream fout( OPTION_FILE_NAME);
			char *outString = "####�ɼ� ���� ����.\n\n" \
								"#Ʈ���̴��� �ٽ� �Ϸ��� ���� 1����\n" \
								"RE_TRAINING 0\n\n"	\
								"#xml���ϵ��� ����ִ� ������ ���\n" \
								"TRAINING_XML_FOLDER_PATH ../../../../training_data/\n\n"	\
								"#���ͳݿ��� �ٽ� ���Ǿ �����Ϸ��� ���� 1����. ���ξ �������� ���� �� ����.\n" \
								"GET_SYNONYM_DATA 0\n\n"	\
								"#���ͳݿ��� ������ ���Ǿ ������ �����̸�.\n" \
								"SYNONYM_DATA_FILE_NAME synonym.txt\n\n"	\
								"#db �̸�.\n" \
								"DB_NAME Turuoise.db\n\n"	\
								"#����� ���ھ �Լ�. �ڻ��� ���絵 = 0, BM25 = 1\n" \
								"SCORING_FUNC 0\n\n"	\
								"#���� �м����� ������ ����ġ\n" \
								"QUESTION_WEIGHT 0.8\n\n"	\
								"#���� �м����� �亯�� ����ġ\n" \
								"ANSWER_WEIGHT 0.2\n\n"	\
								"#���� �м����� ������Ÿ�� ����ġ. ������� �������� 0\n" \
								"LIEBENSTEIN_WEIGHT 1\n\n"	\
								"#���� �м����� ���Ǿ� ����ġ. ������� �������� 0\n" \
								"SYNONYM_WEIGHT 0.7\n\n"	\
								"#��� xml���� ������ �� ����\n" \
								"DISPLAY_LIMIT 10";
			fout << outString;
			fout.close();
		}

		void printOptionState( ) { 
			std::cout << "-- ���� �ɼ� --" << std::endl;
			std::cout << "RE_TRAINING " << re_training << std::endl;
			std::cout << "TRAINING_XML_FOLDER_PATH " << training_xml_folder_path << std::endl;
			std::cout << "GET_SYNONYM_DATA 0 " << get_synonym_data << std::endl;
			std::cout << "SYNONYM_DATA_FILE_NAME " << synonym_data_file_name << std::endl;
			std::cout << "DB_NAME " << db_name << std::endl;
			std::cout << "SCORING_FUNC " << scoring_func << std::endl;
			std::cout << "QUESTION_WEIGHT " << question_weight << std::endl;
			std::cout << "ANSWER_WEIGHT " << answer_weight << std::endl;
			std::cout << "LIEBENSTEIN_WEIGHT " << liebenstein_weight << std::endl;
			std::cout << "SYNONYM_WEIGHT " << synonym_weight << std::endl;
			std::cout << "DISPLAY_LIMIT " << display_limit << std::endl << std::endl;
		}
};

std::list<QueryInput>* readQueryXml(const String &query_xml_file)
{
	rapidxml::file<> xml_file(query_xml_file.c_str());
	rapidxml::xml_document<> xml_doc;

	xml_doc.parse<0>(xml_file.data());
	std::list<QueryInput>* pQueryInput = new std::list<QueryInput>();

	rapidxml::xml_node<char> *curNode = xml_doc.first_node("query");
	while(curNode != nullptr)
	{
		rapidxml::xml_node<char> *qnum_node = curNode->first_node("qnum");
		rapidxml::xml_node<char> *text_node = curNode->first_node("text");

		QueryInput query;
		query.qum	= atoi(convertFromUTF8ToANSI(qnum_node->value()).c_str());
		query.query = convertFromUTF8ToANSI(text_node->value());
		//std::cout << query.qum << std::endl << query.query << std::endl;
		curNode = curNode->next_sibling();

		pQueryInput->push_back(query);
	}

	return pQueryInput;
}

bool writeResultXml(int qnum, const std::vector<DocInfo> *cqa_result)
{
	const String QUERY_BEGIN_TAG		= "<query>";
	const String QUERY_END_TAG			= "</query>";
	const String QUERY_NUM_BEGIN_TAG	= "<qnum>";
	const String QUERY_NUM_END_TAG		= "</qnum>";
	const String RANK_BEGIN_TAG			= "<rank>";
	const String RANK_END_TAG			= "</rank>";

	std::ofstream outFile;
	outFile.exceptions(std::ifstream::eofbit | std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		outFile.open("result.xml", std::ios::app);
		if(outFile.fail())
			std::cerr << "Cannot make a file : result.xml" << std::endl;

		outFile << QUERY_BEGIN_TAG << QUERY_NUM_BEGIN_TAG << qnum << QUERY_NUM_END_TAG << std::endl;
		outFile << RANK_BEGIN_TAG << std::endl;
		for(auto iter=cqa_result->begin(); iter!=cqa_result->end(); iter++)
		{
			outFile << *iter << std::endl;
		}
		outFile << RANK_END_TAG << std::endl;
		outFile << QUERY_END_TAG << std::endl;
	}
	catch(const std::ifstream::failure& e)
	{
		std::cerr << "Exception is occured when reading a file!" << std::endl;
		std::cerr << "Exception: " << e.what() << std::endl;
		return false;
	}
	catch(const std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
		return false;
	}
	catch(...)
	{
		std::cerr << "Undefined exception!" << std::endl;
		return false;
	}

	return true;
}

bool readOptionFile( Optstruct& option);



// main -----------------------------------------------------------------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
#ifndef LSA
	String db_name = "Turuoise.db";
	SqliteConnector *mSqliteConnector = new SqliteConnector( db_name);
	mSqliteConnector->initDB();
	
	mSqliteConnector->getLSAtestfileTF();		// TF
	mSqliteConnector->getLSAtestfileTF_MUL_IDF();	// TF ���ϱ� IDF
	delete mSqliteConnector;
#else
	std::cout << "=== Run Turuoise ===" << std::endl;
	Optstruct option;
	
	if( _access( OPTION_FILE_NAME, 0) == 0) {
		if( readOptionFile( option) == false) {
			std::cout << "�߸��� �ɼ� ����. option.txt�� ������ �ּ���." << std::endl;
			return 0;
		}		
	}
	else
		option.createDefalutOptionFile();
	
	option.printOptionState();


	QAsystem *pQAsystem = new CQAsystem( option.db_name);
	
	if( option.re_training == 1) {
		remove( option.db_name.c_str());

		clock_t start_time, end_time;
		start_time = clock();
		pQAsystem->beginTraning( option.training_xml_folder_path, false);
		end_time = clock();
		std::cout<< "Machine Learning Time : " << static_cast<Real>(end_time-start_time)/CLOCKS_PER_SEC << std::endl << std::endl;
	}
	else
	pQAsystem->beginTraning( option.training_xml_folder_path, true);


	SqliteConnector *mSqliteConnector = new SqliteConnector( option.db_name);
	mSqliteConnector->initDB();

	if( option.get_synonym_data == 1) {

		std::ofstream fout("word.txt");
		int wordnum = mSqliteConnector->getCountWordID();
		for( int i = 0 ; i < wordnum ; i++)
			fout << mSqliteConnector->getWord( i) << std::endl;
		fout.close();

		system("getSynonym.exe");

		std::cout << "=== db�� ���Ǿ� ���̺��� ������Ʈ�ϴ� �κ��� ===" << std::endl;
		std::ifstream fin( option.synonym_data_file_name);
		char buf[ 2046];
		wchar_t wbuf[ 2046];
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> utfconv;
	
		mSqliteConnector->createSynonymTable();	// ���Ǿ� ���̺��� ���� ��� ������.

		while( fin.getline( buf, 2046)) {		
			std::wstring wstr = mSqliteConnector->utf8_to_utf16( buf);
			wcsncpy( wbuf, wstr.c_str(), wstr.length() + 1);
			wchar_t* token = wcstok( wbuf, L" \t\n\r");

			std::string narrow = utfconv.to_bytes( token);     
			int word_id = mSqliteConnector->getWordID( mSqliteConnector->UTF8ToANSI( narrow.c_str()));
			if( word_id != -1) {
				if( ( token = wcstok( NULL, L" \t\n\r")) != NULL) {
					mSqliteConnector->updateSynonymTable( word_id, narrow);
					mSqliteConnector->updateSynonymTable( word_id, utfconv.to_bytes( token));
				}
				while( ( token = wcstok( NULL, L" \t\n\r")) != NULL)
					mSqliteConnector->updateSynonymTable( word_id, utfconv.to_bytes( token));
			}
		}
	}
	
	//std::cout << "=== ���Ǿ� ���̺��� �޸𸮷� �ø�. ===" << std::endl;
	mSynonymTable = mSqliteConnector->getSynonymTable();
	//std::cout << "=== WORDID ���̺��� �޸𸮷� �ø�. ===" << std::endl;
	mWordIDTable = mSqliteConnector->getWordIDTable();

	delete mSqliteConnector;

	auto *qry_info = readQueryXml("query.xml");
	for(auto iter = qry_info->begin(); iter!=qry_info->end(); iter++)
	{
		pQAsystem->analyzeQuery(iter->query);
		pQAsystem->calculateScore( option.scoring_func, option.question_weight, option.answer_weight, option.liebenstein_weight, option.synonym_weight);
		auto *result = pQAsystem->sortResult( option.display_limit);
		writeResultXml(iter->qum, result);
		delete result;
		pQAsystem->writeResult(iter->query, option.display_limit);
	}
	delete qry_info;

	delete pQAsystem;
#endif	
	return 0;
}
// main -----------------------------------------------------------------------------------------------------------------------------------------------



bool readOptionFile( Optstruct& option) {
	char buf[ 1024];
	std::ifstream fin( OPTION_FILE_NAME);
		
	while( fin.getline( buf, 1024)) {
		char* token = strtok( buf, " \t\n");

		if( token == NULL)
			continue;

		if( token[ 0] == '#')
			continue;

		if( strncmp( token, "RE_TRAINING", strlen( "RE_TRAINING")) == 0) {
			token = strtok( NULL, " \t\n");
			if( token == NULL)
				return false;
			else {
				if( atoi( token) == 0)
					option.re_training = false;
				else
					option.re_training = true;
			}
		}
		else if( strncmp( token, "TRAINING_XML_FOLDER_PATH", strlen( "TRAINING_XML_FOLDER_PATH")) == 0) {
			token = strtok( NULL, " \t\n");
			if( token == NULL)
				return false;
			else
				option.training_xml_folder_path = token;
		}
		else if( strncmp( token, "GET_SYNONYM_DATA", strlen( "GET_SYNONYM_DATA")) == 0) {
			token = strtok( NULL, " \t\n");
			if( token == NULL)
				return false;
			else {
				if( atoi( token) == 0)
					option.get_synonym_data = false;
				else
					option.get_synonym_data = true;
			}
		}
		else if( strncmp( token, "SYNONYM_DATA_FILE_NAME", strlen( "SYNONYM_DATA_FILE_NAME")) == 0) {
			token = strtok( NULL, " \t\n");
			if( token == NULL)
				return false;
			else
				option.synonym_data_file_name = token;
		}
		else if( strncmp( token, "DB_NAME", strlen( "DB_NAME")) == 0) {
			token = strtok( NULL, " \t\n");
			if( token == NULL)
				return false;
			else
				option.db_name = token;
		}
		else if( strncmp( token, "SCORING_FUNC", strlen( "SCORING_FUNC")) == 0) {
			token = strtok( NULL, " \t\n");
			if( token == NULL)
				return false;
			else {
				if( atoi( token) == 0)
					option.scoring_func = FUNC_COSINE;
				else
					option.scoring_func = FUNC_BM25;
			}
		}
		else if( strncmp( token, "QUESTION_WEIGHT", strlen( "QUESTION_RATIO")) == 0) {
			token = strtok( NULL, " \t\n");
			if( token == NULL)
				return false;
			else
				option.question_weight = atof( token);
		}
		else if( strncmp( token, "ANSWER_WEIGHT", strlen( "ANSWER_RATIO")) == 0) {
			token = strtok( NULL, " \t\n");
			if( token == NULL)
				return false;
			else
				option.answer_weight = atof( token);
		}
		else if( strncmp( token, "LIEBENSTEIN_WEIGHT", strlen( "LIEBENSTEIN_WEIGHT")) == 0) {
			token = strtok( NULL, " \t\n");
			if( token == NULL)
				return false;
			else
				option.liebenstein_weight = atof( token);
		}
		else if( strncmp( token, "SYNONYM_WEIGHT", strlen( "SYNONYM_WEIGHT")) == 0) {
			token = strtok( NULL, " \t\n");
			if( token == NULL)
				return false;
			else
				option.synonym_weight = atof( token);
		}
		else if( strncmp( token, "DISPLAY_LIMIT", strlen( "DISPLAY_LIMIT")) == 0) {
			token = strtok( NULL, " \t\n");
			if( token == NULL)
				return false;
			else
				option.display_limit = atoi( token);
		}
		else
			return false;
	}
	fin.close();

	return true;
}



#ifdef SAVE
int main(int argc, char* argv[])
{
	std::cout << "=== Run Turuoise ===" << std::endl;

	String TRAINING_DATA_PATH = "../../../../training_data/";
	String TRAINING_DB_NAME = "Turuoise.db";
	String QUERY = "���� super�� �ѱ۷� ǥ���� �� ���۶�� �ؾ� �ϳ���, ���۶�� �ؾ� �ϳ���?Superman, supermarket ���� ���۸�, ���۸����ΰ���, ���۸�, ���۸����ΰ���";
	//String QUERY = "���� ���� �Ѿ�߸��ٿ��� ������ [��ó, ����, ����] �� ��� �߼��ϴ� ���� �´� ���ΰ���?";
	Integer DISPLAY_LIMIT = 10;

	// ���¼� �м��� �����Ѵ�.
	/*
	MA::MorphemeAnalyzer analyzer;
	std::vector< MA::VECMorpheme> test = analyzer.Extract( "�ٶ��� �Բ� �������.");
	*/

	/* ���Ǿ������� ���� word.txt�� ��ºκ���. ��¹��� word.txt
	std::cout << "=== ���Ǿ������� ���� word.txt�� ��ºκ��� ===" << std::endl;
	std::ofstream fout("word.txt");
	SqliteConnector *mSqliteConnector = new SqliteConnector( TRAINING_DB_NAME);
	mSqliteConnector->initDB();
	int wordnum = mSqliteConnector->getCountWordID();
	for( int i = 0 ; i < wordnum ; i++)
		fout << mSqliteConnector->getWord( i) << std::endl;
	fout.close();
	*/


	/*
	// ����� ���Ǿ� ���Ϸ� ���� db�� ���Ǿ� ���̺� �߰���. ���Ǿ ����� ������ synonym.txt
	SqliteConnector *mSqliteConnector = new SqliteConnector( TRAINING_DB_NAME);
	mSqliteConnector->initDB();
	std::cout << "=== db�� ���Ǿ� ���̺��� ������Ʈ�ϴ� �κ��� ===" << std::endl;
	std::ifstream fin("synonym.txt");
	char buf[ 2046];
	wchar_t wbuf[ 2046];
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> utfconv;
	
	mSqliteConnector->createSynonymTable();	// ���Ǿ� ���̺��� ���� ��� ������.

	while( fin.getline( buf, 2046)) {		
		std::wstring wstr = mSqliteConnector->utf8_to_utf16( buf);
		wcsncpy( wbuf, wstr.c_str(), wstr.length() + 1);
		wchar_t* token = wcstok( wbuf, L" \t\n\r");

		std::string narrow = utfconv.to_bytes( token);     
		int word_id = mSqliteConnector->getWordID( mSqliteConnector->UTF8ToANSI( narrow.c_str()));
		if( word_id != -1) {
			if( ( token = wcstok( NULL, L" \t\n\r")) != NULL) {
				mSqliteConnector->updateSynonymTable( word_id, narrow);
				mSqliteConnector->updateSynonymTable( word_id, utfconv.to_bytes( token));
			}
			while( ( token = wcstok( NULL, L" \t\n\r")) != NULL)
				mSqliteConnector->updateSynonymTable( word_id, utfconv.to_bytes( token));
		}
	}
	delete mSqliteConnector;
	*/


	
	SqliteConnector *mSqliteConnector = new SqliteConnector( TRAINING_DB_NAME);
	mSqliteConnector->initDB();
	//std::cout << "=== ���Ǿ� ���̺��� �޸𸮷� �ø�. ===" << std::endl;
	mSynonymTable = mSqliteConnector->getSynonymTable();
	//std::cout << "=== WORDID ���̺��� �޸𸮷� �ø�. ===" << std::endl;
	mWordIDTable = mSqliteConnector->getWordIDTable();

	QAsystem *pQAsystem = new CQAsystem(TRAINING_DB_NAME);

	#ifdef _TRAINING_MODE_
	clock_t start_time, end_time;
	start_time = clock();
	pQAsystem->beginTraning(TRAINING_DATA_PATH, false);
	end_time = clock();
	std::cout<< "Machine Learning Time : " << static_cast<Real>(end_time-start_time)/CLOCKS_PER_SEC << std::endl << std::endl;
	#else
	pQAsystem->beginTraning(TRAINING_DATA_PATH, true);
	#endif

/*	pQAsystem->analyzeQuery( QUERY);
	clock_t start_time, end_time;
	start_time = clock();
	pQAsystem->calculateScore();
	end_time = clock();
	std::cout<< "Machine Learning Time : " << static_cast<Real>(end_time-start_time)/CLOCKS_PER_SEC << std::endl << std::endl;
	pQAsystem->dispalyResult(DISPLAY_LIMIT);
*/

	auto *qry_info = readQueryXml("query.xml");
	for(auto iter = qry_info->begin(); iter!=qry_info->end(); iter++)
	{
		pQAsystem->analyzeQuery(iter->query);
		pQAsystem->calculateScore();
		auto *result = pQAsystem->sortResult(DISPLAY_LIMIT);
		//pQAsystem->dispalyResult(DISPLAY_LIMIT);
		writeResultXml(iter->qum, result);
		delete result;
		//pQAsystem->dispalyResult(DISPLAY_LIMIT);
		pQAsystem->writeResult(iter->query, DISPLAY_LIMIT);
	}
	delete qry_info;

	delete pQAsystem;
	
	return 0;
}
#endif