#include "NewTest.h"

NewTest::NewTest(int a,std::string b)
{
}
void NewTest::setA(int a)
{
return a;
}
std::string NewTest::getstr(int e)
{
return "Hi!!!";
}
std::string NewTest::service(int a,std::string message)
{
std::string ret = "Sumeet\n\n\t\s";
std::string ret2;
ret2 = "How are you???";
ret = temp + ret;
ret = getstr(2) + ret + "\nWhere have you been these days?";
return ret;
}
