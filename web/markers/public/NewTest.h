#ifndef NEWTEST_H_
#define NEWTEST_H_
#include "string"

public class NewTest{
NewTest(int a,std::string b);
private:
int a = 1;
std::string b;
protected:
public:
float c = 1.1;
public void setA(int a);
public std::string getstr(int e);
public std::string service(int a,std::string message);
}
#endif