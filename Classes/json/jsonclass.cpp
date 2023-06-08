#include"jsonclass.h"
#include<fstream>
#include<sstream>

using namespace std;

bool Readjson::JsonClassInit(string pash)
{
    if (!json_reader.parse(pash,json_object))
    {
        return false;
    }
    return true;
}

Readjson::Readjson()
{

}