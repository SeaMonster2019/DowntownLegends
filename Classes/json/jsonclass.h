#pragma once
#include"json.h"
#include <string>

using namespace std;

class Readjson
{
public:
	Readjson();
	bool JsonClassInit(string pash);
	Json::Value json_object;
	Json::Reader json_reader;
};