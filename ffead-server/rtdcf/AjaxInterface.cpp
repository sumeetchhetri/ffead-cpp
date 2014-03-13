#include "AfcInclude.h"

extern "C"
{
string defaultinvokeAjaxMethodForExposesayHello(strVec _inp)
{
string _1 = CastUtil::lexical_cast<string>(_inp.at(0));
int _2 = CastUtil::lexical_cast<int>(_inp.at(1));
float _3 = CastUtil::lexical_cast<float>(_inp.at(2));
Expose _obj;
return JSONSerialize::serialize<YObject>(_obj.sayHello(_1,_2,_3), "default");

}
string defaultinvokeAjaxMethodForExposesayHello1(strVec _inp)
{
string _1 = CastUtil::lexical_cast<string>(_inp.at(0));
int _2 = CastUtil::lexical_cast<int>(_inp.at(1));
float _3 = CastUtil::lexical_cast<float>(_inp.at(2));
Expose _obj;
return _obj.sayHello1(_1,_2,_3);
}
string defaultinvokeAjaxMethodForExposesayHello2(strVec _inp)
{
YObject _1 = JSONSerialize::unserialize<YObject>(_inp.at(0), "default");
int _2 = CastUtil::lexical_cast<int>(_inp.at(1));
float _3 = CastUtil::lexical_cast<float>(_inp.at(2));
Expose _obj;
return JSONSerialize::serialize<YObject>(_obj.sayHello2(_1,_2,_3), "default");

}
string defaultinvokeAjaxMethodForTestPagetextonclick(strVec _inp)
{
int _1 = CastUtil::lexical_cast<int>(_inp.at(0));
string _2 = CastUtil::lexical_cast<string>(_inp.at(1));
string _3 = CastUtil::lexical_cast<string>(_inp.at(2));
TestPage _obj;
return _obj.textonclick(_1,_2,_3);
}
string defaultinvokeAjaxMethodForTestPagelinkonclick(strVec _inp)
{
TestPage _obj;
return CastUtil::lexical_cast<string>(_obj.linkonclick());
}

}
