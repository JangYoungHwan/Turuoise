/*
 * Title	: It just my memo
 * Author	: YW. Jang, Zizo
 */
// DEBUG : io-defs.h(78) unicode redef

#include <iostream>
#include <string>
#include "QAsystem.h"
#include "CQAsystem.h"


int main(int argc, char* argv[])
{
	std::cout << "=== Run Turuoise ===" << std::endl;

	String TRAINING_DATA_PATH = "../../../../training_data/";
	String TRAINING_DB_NAME = "Turuoise.db";
	String QUERY = "���ϴٴ� ������ �� �˾Ҵµ�, �����ε� ���δٰ� �ϳ׿�. ����� ���ϴٰ� ��� ���̴��� �� ���������� �ʴµ�, �� �˷� �ּ���.";

	QAsystem *pQAsystem = new CQAsystem(TRAINING_DB_NAME);

	pQAsystem->beginTraning(TRAINING_DATA_PATH);
	pQAsystem->analyzeQuery(QUERY);
	pQAsystem->calculateScore();
	pQAsystem->dispalyResult();

	delete pQAsystem;

	return 0;
}
