#include "ClassInfo.h"
#include "string"
#include "Method.h"
#include "Field.h"
#include "DefTemp.h"
#include "DefaultController.h"
#include "DefaultIOFilter.h"
#include "DefaultOAUTHController.h"
#include "DefaultRestController.h"
#include "DepDependencyBean1Impl.h"
#include "DepDependencyBean2Impl.h"
#include "DependencyBean1.h"
#include "DependencyBean2.h"
#include "DependencyBean3.h"
#include "DependencyBean4.h"
#include "Dview.h"
#include "Expose.h"
#include "Service1.h"
#include "Service2.h"
#include "Tempo.h"
#include "Test2.h"
#include "Test3.h"
#include "Test4.h"
#include "Test.h"
#include "TestBeanCons.h"
#include "TestBeanIntf.h"
#include "TestBeanProp.h"
#include "TestCronBasedJob.h"
#include "TestForm.h"
#include "TestFormController.h"
#include "TestMany.h"
#include "TestObject1.h"
#include "TestPage.h"
#include "TestSTLs.h"
#include "Testing.h"
#include "YObject.h"
#include "TestObject.h"
#include "TestAllInOne.h"
#include "TestingWS.h"
#include "FlexAppController.h"
#include "OAUTH2Controller.h"
#include "OAUTHController.h"
extern "C"
{
ClassInfo defaultgetReflectionCIForDefTemp()
{
ClassInfo classInfo;
classInfo.setClassName("DefTemp");
classInfo.setNamespace("");
classInfo.setBase("public TemplateHandler");
Constructor ctor;
Method me;
Field f;
args argu;
ctor.clear();
me.clear();
ctor.setName("DefTemp");
ctor.setArgumentTypes(argu);
argu.clear();
classInfo.addConstructor(ctor);
ctor.clear();
me.clear();
me.setReturnType("Context");
me.setMethodName("DefTempgetContext");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}

return classInfo;
}
ClassInfo defaultgetReflectionCIForDefaultController()
{
ClassInfo classInfo;
classInfo.setClassName("DefaultController");
classInfo.setNamespace("");
classInfo.setBase("public Controller");
Constructor ctor;
Method me;
Field f;
args argu;
ctor.clear();
me.clear();
ctor.setName("DefaultController");
ctor.setArgumentTypes(argu);
argu.clear();
classInfo.addConstructor(ctor);
ctor.clear();
me.clear();
me.setReturnType("HttpResponse");
argu.push_back("HttpRequest");
me.setMethodName("DefaultControllerserviceHttpRequest");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}

return classInfo;
}
ClassInfo defaultgetReflectionCIForDefaultIOFilter()
{
ClassInfo classInfo;
classInfo.setClassName("DefaultIOFilter");
classInfo.setNamespace("");
classInfo.setBase("public Filter");
Constructor ctor;
Method me;
Field f;
args argu;
ctor.clear();
me.clear();
ctor.setName("DefaultIOFilter");
ctor.setArgumentTypes(argu);
argu.clear();
classInfo.addConstructor(ctor);
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("HttpRequest*");
me.setMethodName("DefaultIOFilterdoInputFilterHttpRequestptr");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("HttpResponse*");
me.setMethodName("DefaultIOFilterdoOutputFilterHttpResponseptr");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("bool");
argu.push_back("HttpRequest*");
argu.push_back("HttpResponse*");
me.setMethodName("DefaultIOFilterdoHandleHttpRequestptrHttpResponseptr");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}

return classInfo;
}
ClassInfo defaultgetReflectionCIForDefaultOAUTHController()
{
ClassInfo classInfo;
classInfo.setClassName("DefaultOAUTHController");
classInfo.setNamespace("");
classInfo.setBase("public AuthController");
Constructor ctor;
Method me;
Field f;
args argu;
ctor.clear();
me.clear();
ctor.setName("DefaultOAUTHController");
ctor.setArgumentTypes(argu);
argu.clear();
classInfo.addConstructor(ctor);
ctor.clear();
me.clear();
me.setReturnType("bool");
argu.push_back("HttpRequest*");
argu.push_back("HttpResponse*");
me.setMethodName("DefaultOAUTHControllerhandleHttpRequestptrHttpResponseptr");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("bool");
argu.push_back("string");
argu.push_back("string");
me.setMethodName("DefaultOAUTHControllerauthenticatestringstring");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("bool");
argu.push_back("string");
argu.push_back("string");
me.setMethodName("DefaultOAUTHControllerauthenticateSecuritystringstring");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("bool");
me.setMethodName("DefaultOAUTHControllerisInitialized");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("bool");
argu.push_back("map<string,string>");
me.setMethodName("DefaultOAUTHControllerauthenticatemaptsstringstringte");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("string");
argu.push_back("string");
me.setMethodName("DefaultOAUTHControllergetUserRolestring");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}

return classInfo;
}
ClassInfo defaultgetReflectionCIForDefaultRestController()
{
ClassInfo classInfo;
classInfo.setClassName("DefaultRestController");
classInfo.setNamespace("");
classInfo.setBase("public RestController");
Constructor ctor;
Method me;
Field f;
args argu;
ctor.clear();
me.clear();
ctor.setName("DefaultRestController");
ctor.setArgumentTypes(argu);
argu.clear();
classInfo.addConstructor(ctor);
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("int");
argu.push_back("int");
me.setMethodName("DefaultRestControlleraddNumbersintint");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("int");
argu.push_back("int");
me.setMethodName("DefaultRestControllerpowerintint");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("vector<int>");
me.setMethodName("DefaultRestControllertestVectorvectortsintte");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("TestMany");
me.setMethodName("DefaultRestControllertestObjectTestMany");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("vector<TestMany>");
me.setMethodName("DefaultRestControllertestVectorObjectvectortsTestManyte");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("ifstream*");
argu.push_back("string");
me.setMethodName("DefaultRestControllertestUploadFileifstreamptrstring");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("ifstream*");
argu.push_back("ifstream*");
argu.push_back("ifstream*");
argu.push_back("string");
me.setMethodName("DefaultRestControllertestUploadFileMulti1ifstreamptrifstreamptrifstreamptrstring");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("vector<ifstream*>");
argu.push_back("string");
me.setMethodName("DefaultRestControllertestUploadFileMulti2vectortsifstreamptrtestring");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}

return classInfo;
}
ClassInfo defaultgetReflectionCIForDepDependencyBean1Impl()
{
ClassInfo classInfo;
classInfo.setClassName("DepDependencyBean1Impl");
classInfo.setNamespace("");
classInfo.setBase("public DependencyIntf1");
Constructor ctor;
Method me;
Field f;
args argu;
ctor.clear();
me.clear();
me.setReturnType("void");
me.setMethodName("DepDependencyBean1Implprint1");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.setName("DepDependencyBean1Impl");
argu.clear();
ctor.setArgumentTypes(argu);
argu.clear();
classInfo.addConstructor(ctor);

return classInfo;
}
ClassInfo defaultgetReflectionCIForDepDependencyBean2Impl()
{
ClassInfo classInfo;
classInfo.setClassName("DepDependencyBean2Impl");
classInfo.setNamespace("");
classInfo.setBase("public DependencyIntf2");
Constructor ctor;
Method me;
Field f;
args argu;
ctor.clear();
me.clear();
me.setReturnType("void");
me.setMethodName("DepDependencyBean2Implprint2");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.setName("DepDependencyBean2Impl");
argu.clear();
ctor.setArgumentTypes(argu);
argu.clear();
classInfo.addConstructor(ctor);

return classInfo;
}
ClassInfo defaultgetReflectionCIForDependencyBean1()
{
ClassInfo classInfo;
classInfo.setClassName("DependencyBean1");
classInfo.setNamespace("");
classInfo.setBase(" ");
Constructor ctor;
Method me;
Field f;
args argu;
ctor.clear();
me.clear();
me.setReturnType("void");
me.setMethodName("DependencyBean1print");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.setName("DependencyBean1");
argu.clear();
ctor.setArgumentTypes(argu);
argu.clear();
classInfo.addConstructor(ctor);

return classInfo;
}
ClassInfo defaultgetReflectionCIForDependencyBean2()
{
ClassInfo classInfo;
classInfo.setClassName("DependencyBean2");
classInfo.setNamespace("");
classInfo.setBase(" ");
Constructor ctor;
Method me;
Field f;
args argu;
ctor.clear();
me.clear();
me.setReturnType("void");
me.setMethodName("DependencyBean2print");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.setName("DependencyBean2");
argu.clear();
ctor.setArgumentTypes(argu);
argu.clear();
classInfo.addConstructor(ctor);

return classInfo;
}
ClassInfo defaultgetReflectionCIForDependencyBean3()
{
ClassInfo classInfo;
classInfo.setClassName("DependencyBean3");
classInfo.setNamespace("");
classInfo.setBase(" ");
Constructor ctor;
Method me;
Field f;
args argu;
ctor.clear();
me.clear();
me.setReturnType("void");
me.setMethodName("DependencyBean3print");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.setName("DependencyBean3");
argu.clear();
ctor.setArgumentTypes(argu);
argu.clear();
classInfo.addConstructor(ctor);

return classInfo;
}
ClassInfo defaultgetReflectionCIForDependencyBean4()
{
ClassInfo classInfo;
classInfo.setClassName("DependencyBean4");
classInfo.setNamespace("");
classInfo.setBase(" ");
Constructor ctor;
Method me;
Field f;
args argu;
ctor.clear();
me.clear();
me.setReturnType("void");
me.setMethodName("DependencyBean4print");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.setName("DependencyBean4");
argu.clear();
ctor.setArgumentTypes(argu);
argu.clear();
classInfo.addConstructor(ctor);

return classInfo;
}
ClassInfo defaultgetReflectionCIForDview()
{
ClassInfo classInfo;
classInfo.setClassName("Dview");
classInfo.setNamespace("");
classInfo.setBase("public DynamicView");
Constructor ctor;
Method me;
Field f;
args argu;
ctor.clear();
me.clear();
ctor.setName("Dview");
ctor.setArgumentTypes(argu);
argu.clear();
classInfo.addConstructor(ctor);
ctor.clear();
me.clear();
me.setReturnType("Document");
me.setMethodName("DviewgetDocument");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}

return classInfo;
}
ClassInfo defaultgetReflectionCIForExpose()
{
ClassInfo classInfo;
classInfo.setClassName("Expose");
classInfo.setNamespace("");
classInfo.setBase(" ");
Constructor ctor;
Method me;
Field f;
args argu;
ctor.clear();
me.clear();
ctor.setName("Expose");
ctor.setArgumentTypes(argu);
argu.clear();
classInfo.addConstructor(ctor);
ctor.clear();
me.clear();
me.setReturnType("YObject");
argu.push_back("string");
argu.push_back("int");
argu.push_back("float");
me.setMethodName("ExposesayHellostringintfloat");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("string");
argu.push_back("string");
argu.push_back("int");
argu.push_back("float");
me.setMethodName("ExposesayHello1stringintfloat");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("YObject");
argu.push_back("YObject");
argu.push_back("int");
argu.push_back("float");
me.setMethodName("ExposesayHello2YObjectintfloat");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}

return classInfo;
}
ClassInfo defaultgetReflectionCIForService1()
{
ClassInfo classInfo;
classInfo.setClassName("Service1");
classInfo.setNamespace("");
classInfo.setBase("public ServiceInt");
Constructor ctor;
Method me;
Field f;
args argu;
ctor.clear();
me.clear();
ctor.setName("Service1");
ctor.setArgumentTypes(argu);
argu.clear();
classInfo.addConstructor(ctor);
ctor.clear();
me.clear();
me.setReturnType("string");
me.setMethodName("Service1service1");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}

return classInfo;
}
ClassInfo defaultgetReflectionCIForService2()
{
ClassInfo classInfo;
classInfo.setClassName("Service2");
classInfo.setNamespace("");
classInfo.setBase("public ServiceInt");
Constructor ctor;
Method me;
Field f;
args argu;
ctor.clear();
me.clear();
ctor.setName("Service2");
ctor.setArgumentTypes(argu);
argu.clear();
classInfo.addConstructor(ctor);
ctor.clear();
me.clear();
me.setReturnType("string");
argu.push_back("string");
me.setMethodName("Service2service2string");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}

return classInfo;
}
ClassInfo defaultgetReflectionCIForTempo()
{
ClassInfo classInfo;
classInfo.setClassName("Tempo");
classInfo.setNamespace("");
classInfo.setBase(" ");
Constructor ctor;
Method me;
Field f;
args argu;
ctor.clear();
me.clear();
ctor.setName("Tempo");
ctor.setArgumentTypes(argu);
argu.clear();
classInfo.addConstructor(ctor);
ctor.clear();
me.clear();
me.setReturnType("void");
me.setMethodName("Tempoprin1");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
f.clear();
f.setType("int");
f.setFieldName("p");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("string");
f.setFieldName("h");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}

return classInfo;
}
ClassInfo defaultgetReflectionCIForTest2()
{
ClassInfo classInfo;
classInfo.setClassName("Test2");
classInfo.setNamespace("");
classInfo.setBase(" ");
Constructor ctor;
Method me;
Field f;
args argu;
ctor.clear();
me.clear();
ctor.setName("Test2");
ctor.setArgumentTypes(argu);
argu.clear();
classInfo.addConstructor(ctor);
ctor.clear();
me.clear();
me.setReturnType("int");
me.setMethodName("Test2getId");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("int");
me.setMethodName("Test2setIdint");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("int");
me.setMethodName("Test2getTest_id");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("int");
me.setMethodName("Test2setTest_idint");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("Test");
me.setMethodName("Test2getTest");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("Test");
me.setMethodName("Test2setTestTest");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
f.clear();
f.setType("int");
f.setFieldName("id");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("int");
f.setFieldName("test_id");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("Test");
f.setFieldName("test");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}

return classInfo;
}
ClassInfo defaultgetReflectionCIForTest3()
{
ClassInfo classInfo;
classInfo.setClassName("Test3");
classInfo.setNamespace("");
classInfo.setBase(" ");
Constructor ctor;
Method me;
Field f;
args argu;
ctor.clear();
me.clear();
ctor.setName("Test3");
ctor.setArgumentTypes(argu);
argu.clear();
classInfo.addConstructor(ctor);
ctor.clear();
me.clear();
me.setReturnType("int");
me.setMethodName("Test3getId");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("int");
me.setMethodName("Test3setIdint");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("vector<Test>");
me.setMethodName("Test3getTests");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("vector<Test>");
me.setMethodName("Test3setTestsvectortsTestte");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("int");
me.setMethodName("Test3getTest_id");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("int");
me.setMethodName("Test3setTest_idint");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
f.clear();
f.setType("int");
f.setFieldName("id");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("int");
f.setFieldName("test_id");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("vector<Test>");
f.setFieldName("tests");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}

return classInfo;
}
ClassInfo defaultgetReflectionCIForTest4()
{
ClassInfo classInfo;
classInfo.setClassName("Test4");
classInfo.setNamespace("");
classInfo.setBase(" ");
Constructor ctor;
Method me;
Field f;
args argu;
ctor.clear();
me.clear();
ctor.setName("Test4");
ctor.setArgumentTypes(argu);
argu.clear();
classInfo.addConstructor(ctor);
ctor.clear();
me.clear();
me.setReturnType("Date");
me.setMethodName("Test4getDate");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("Date");
me.setMethodName("Test4setDateDate");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("Date");
me.setMethodName("Test4getDatt");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("Date");
me.setMethodName("Test4setDattDate");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("Date");
me.setMethodName("Test4getDattm");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("Date");
me.setMethodName("Test4setDattmDate");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("BinaryData");
me.setMethodName("Test4getBinar");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("BinaryData");
me.setMethodName("Test4setBinarBinaryData");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
f.clear();
f.setType("Date");
f.setFieldName("date");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("Date");
f.setFieldName("datt");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("Date");
f.setFieldName("dattm");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("BinaryData");
f.setFieldName("binar");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}

return classInfo;
}
ClassInfo defaultgetReflectionCIForTest()
{
ClassInfo classInfo;
classInfo.setClassName("Test");
classInfo.setNamespace("");
classInfo.setBase(" ");
Constructor ctor;
Method me;
Field f;
args argu;
ctor.clear();
me.clear();
ctor.setName("Test");
ctor.setArgumentTypes(argu);
argu.clear();
classInfo.addConstructor(ctor);
ctor.clear();
me.clear();
me.setReturnType("int");
me.setMethodName("TestgetId");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("int");
me.setMethodName("TestsetIdint");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("string");
me.setMethodName("TestgetName");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("string");
me.setMethodName("TestsetNamestring");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
f.clear();
f.setType("int");
f.setFieldName("id");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("string");
f.setFieldName("name");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}

return classInfo;
}
ClassInfo defaultgetReflectionCIForTestBeanCons()
{
ClassInfo classInfo;
classInfo.setClassName("TestBeanCons");
classInfo.setNamespace("");
classInfo.setBase(" ");
Constructor ctor;
Method me;
Field f;
args argu;
ctor.clear();
me.clear();
ctor.setName("TestBeanCons");
ctor.setArgumentTypes(argu);
argu.clear();
classInfo.addConstructor(ctor);
ctor.clear();
me.clear();
argu.push_back("int");
argu.push_back("bool");
argu.push_back("DependencyBean4");
ctor.setName("TestBeanConsintboolDependencyBean4");
ctor.setArgumentTypes(argu);
argu.clear();
classInfo.addConstructor(ctor);
ctor.clear();
me.clear();
me.setReturnType("int");
me.setMethodName("TestBeanConsgetIntProp");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("bool");
me.setMethodName("TestBeanConsgetBoolProp");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("DependencyBean4");
me.setMethodName("TestBeanConsgetDependencyBean4");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
me.setMethodName("TestBeanConsprint");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
f.clear();
f.setType("int");
f.setFieldName("intProp");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("bool");
f.setFieldName("boolProp");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("DependencyBean4");
f.setFieldName("dependencyBean4");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}

return classInfo;
}
ClassInfo defaultgetReflectionCIForTestBeanIntf()
{
ClassInfo classInfo;
classInfo.setClassName("TestBeanIntf");
classInfo.setNamespace("");
classInfo.setBase(" ");
Constructor ctor;
Method me;
Field f;
args argu;
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("DependencyIntf1*");
me.setMethodName("TestBeanIntfsetDependencyIntf1DependencyIntf1ptr");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("DependencyIntf2*");
me.setMethodName("TestBeanIntfsetDependencyIntf2DependencyIntf2ptr");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
me.setMethodName("TestBeanIntfprint");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
f.clear();
f.setType("DependencyIntf1");
f.setFieldName("dependencyIntf1");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("DependencyIntf2");
f.setFieldName("dependencyIntf2");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
ctor.setName("TestBeanIntf");
argu.clear();
ctor.setArgumentTypes(argu);
argu.clear();
classInfo.addConstructor(ctor);

return classInfo;
}
ClassInfo defaultgetReflectionCIForTestBeanProp()
{
ClassInfo classInfo;
classInfo.setClassName("TestBeanProp");
classInfo.setNamespace("");
classInfo.setBase(" ");
Constructor ctor;
Method me;
Field f;
args argu;
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("string*");
me.setMethodName("TestBeanPropsetStrPropstringptr");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("string*");
me.setMethodName("TestBeanPropgetStrProp");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("DependencyBean1*");
me.setMethodName("TestBeanPropsetDependencyBean1DependencyBean1ptr");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("DependencyBean1*");
me.setMethodName("TestBeanPropgetDependencyBean1");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("DependencyBean2*");
me.setMethodName("TestBeanPropsetDependencyBean2DependencyBean2ptr");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("DependencyBean2*");
me.setMethodName("TestBeanPropgetDependencyBean2");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("DependencyBean3*");
me.setMethodName("TestBeanPropsetDependencyBean3DependencyBean3ptr");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("DependencyBean3*");
me.setMethodName("TestBeanPropgetDependencyBean3");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
me.setMethodName("TestBeanPropprint");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
f.clear();
f.setType("string");
f.setFieldName("strProp");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("DependencyBean1");
f.setFieldName("dependencyBean1");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("DependencyBean2");
f.setFieldName("dependencyBean2");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("DependencyBean3");
f.setFieldName("dependencyBean3");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
ctor.setName("TestBeanProp");
argu.clear();
ctor.setArgumentTypes(argu);
argu.clear();
classInfo.addConstructor(ctor);

return classInfo;
}
ClassInfo defaultgetReflectionCIForTestCronBasedJob()
{
ClassInfo classInfo;
classInfo.setClassName("TestCronBasedJob");
classInfo.setNamespace("");
classInfo.setBase(" ");
Constructor ctor;
Method me;
Field f;
args argu;
ctor.clear();
me.clear();
ctor.setName("TestCronBasedJob");
ctor.setArgumentTypes(argu);
argu.clear();
classInfo.addConstructor(ctor);
ctor.clear();
me.clear();
me.setReturnType("void");
me.setMethodName("TestCronBasedJobrunJob");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
f.clear();
f.setType("int");
f.setFieldName("counter");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}

return classInfo;
}
ClassInfo defaultgetReflectionCIForTestForm()
{
ClassInfo classInfo;
classInfo.setClassName("TestForm");
classInfo.setNamespace("");
classInfo.setBase(" ");
Constructor ctor;
Method me;
Field f;
args argu;
ctor.clear();
me.clear();
ctor.setName("TestForm");
ctor.setArgumentTypes(argu);
argu.clear();
classInfo.addConstructor(ctor);
ctor.clear();
me.clear();
me.setReturnType("string");
me.setMethodName("TestFormgetChe");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("int");
me.setMethodName("TestFormgetNum");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("string");
me.setMethodName("TestFormgetTxt");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("string");
me.setMethodName("TestFormsetChestring");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("int");
me.setMethodName("TestFormsetNumint");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("string");
me.setMethodName("TestFormsetTxtstring");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
f.clear();
f.setType("int");
f.setFieldName("num");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("string");
f.setFieldName("txt");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("string");
f.setFieldName("che");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}

return classInfo;
}
ClassInfo defaultgetReflectionCIForTestFormController()
{
ClassInfo classInfo;
classInfo.setClassName("TestFormController");
classInfo.setNamespace("");
classInfo.setBase(" ");
Constructor ctor;
Method me;
Field f;
args argu;
ctor.clear();
me.clear();
ctor.setName("TestFormController");
ctor.setArgumentTypes(argu);
argu.clear();
classInfo.addConstructor(ctor);
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("void*");
argu.push_back("HttpResponse*");
me.setMethodName("TestFormControlleronSubmitvoidptrHttpResponseptr");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}

return classInfo;
}
ClassInfo defaultgetReflectionCIForTestMany()
{
ClassInfo classInfo;
classInfo.setClassName("TestMany");
classInfo.setNamespace("");
classInfo.setBase(" ");
Constructor ctor;
Method me;
Field f;
args argu;
f.clear();
f.setType("Test");
f.setFieldName("t");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("int");
f.setFieldName("y");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("vector<int>");
f.setFieldName("vi");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("vector<string>");
f.setFieldName("vs");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("vector<double>");
f.setFieldName("vd");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("vector<long>");
f.setFieldName("vl");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("vector<bool>");
f.setFieldName("vb");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("vector<short>");
f.setFieldName("vsh");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("vector<YObject>");
f.setFieldName("vyo");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("list<int>");
f.setFieldName("li");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("std::queue<short>");
f.setFieldName("qsh");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("vector<int>");
me.setMethodName("TestManysetVpivectortsintte");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("vector<int>");
me.setMethodName("TestManygetVpi");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
ctor.setName("TestMany");
ctor.setArgumentTypes(argu);
argu.clear();
classInfo.addConstructor(ctor);
f.clear();
f.setType("vector<int>");
f.setFieldName("vpi");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}

return classInfo;
}
ClassInfo defaultgetReflectionCIForTestObject()
{
ClassInfo classInfo;
classInfo.setClassName("TestObject");
classInfo.setNamespace("");
classInfo.setBase(" ");
Constructor ctor;
Method me;
Field f;
args argu;
ctor.clear();
me.clear();
ctor.setName("TestObject");
ctor.setArgumentTypes(argu);
argu.clear();
classInfo.addConstructor(ctor);
ctor.clear();
me.clear();
me.setReturnType("int");
me.setMethodName("TestObjectgetA");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("int");
me.setMethodName("TestObjectsetAint");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("long");
me.setMethodName("TestObjectgetB");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("long");
me.setMethodName("TestObjectsetBlong");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("string");
me.setMethodName("TestObjectgetC");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("string");
me.setMethodName("TestObjectsetCstring");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("float");
me.setMethodName("TestObjectgetD");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("float");
me.setMethodName("TestObjectsetDfloat");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("double");
me.setMethodName("TestObjectgetE");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("double");
me.setMethodName("TestObjectsetEdouble");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("bool");
me.setMethodName("TestObjectisF");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("bool");
me.setMethodName("TestObjectsetFbool");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("long long");
me.setMethodName("TestObjectgetG");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("long long");
me.setMethodName("TestObjectsetGlonglong");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("short");
me.setMethodName("TestObjectgetH");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("short");
me.setMethodName("TestObjectsetHshort");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("unsigned short");
me.setMethodName("TestObjectgetI");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("unsigned short");
me.setMethodName("TestObjectsetIunsignedshort");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("unsigned int");
me.setMethodName("TestObjectgetJ");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("unsigned int");
me.setMethodName("TestObjectsetJunsignedint");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("unsigned long");
me.setMethodName("TestObjectgetK");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("unsigned long");
me.setMethodName("TestObjectsetKunsignedlong");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("unsigned long long");
me.setMethodName("TestObjectgetL");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("unsigned long long");
me.setMethodName("TestObjectsetLunsignedlonglong");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("string");
me.setMethodName("TestObjecttoString");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
f.clear();
f.setType("int");
f.setFieldName("a");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("long");
f.setFieldName("b");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("string");
f.setFieldName("c");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("float");
f.setFieldName("d");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("double");
f.setFieldName("e");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("bool");
f.setFieldName("f");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("long long");
f.setFieldName("g");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("short");
f.setFieldName("h");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("unsigned short");
f.setFieldName("i");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("unsigned int");
f.setFieldName("j");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("unsigned long");
f.setFieldName("k");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("unsigned long long");
f.setFieldName("l");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}

return classInfo;
}
ClassInfo defaultgetReflectionCIForTestPage()
{
ClassInfo classInfo;
classInfo.setClassName("TestPage");
classInfo.setNamespace("");
classInfo.setBase(" ");
Constructor ctor;
Method me;
Field f;
args argu;
ctor.clear();
me.clear();
ctor.setName("TestPage");
ctor.setArgumentTypes(argu);
argu.clear();
classInfo.addConstructor(ctor);
ctor.clear();
me.clear();
me.setReturnType("string");
argu.push_back("int");
argu.push_back("string");
argu.push_back("string");
me.setMethodName("TestPagetextonclickintstringstring");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("int");
me.setMethodName("TestPagelinkonclick");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}

return classInfo;
}
ClassInfo defaultgetReflectionCIForTestSTLs()
{
ClassInfo classInfo;
classInfo.setClassName("TestSTLs");
classInfo.setNamespace("");
classInfo.setBase(" ");
Constructor ctor;
Method me;
Field f;
args argu;
f.clear();
f.setType("int");
f.setFieldName("mi");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("list<int>");
f.setFieldName("vli");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("list<string>");
f.setFieldName("vls");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("list<double>");
f.setFieldName("vld");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("list<long>");
f.setFieldName("vll");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("list<bool>");
f.setFieldName("vlb");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("list<short>");
f.setFieldName("vlsh");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("list<YObject>");
f.setFieldName("vlyo");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("vector<int>");
f.setFieldName("vvi");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("vector<string>");
f.setFieldName("vvs");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("vector<double>");
f.setFieldName("vvd");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("vector<long>");
f.setFieldName("vvl");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("vector<bool>");
f.setFieldName("vvb");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("vector<short>");
f.setFieldName("vvsh");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("vector<YObject>");
f.setFieldName("vvyo");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("deque<int>");
f.setFieldName("vdi");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("deque<string>");
f.setFieldName("vds");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("deque<double>");
f.setFieldName("vdd");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("deque<long>");
f.setFieldName("vdl");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("deque<bool>");
f.setFieldName("vdb");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("deque<short>");
f.setFieldName("vdsh");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("deque<YObject>");
f.setFieldName("vdyo");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("set<int>");
f.setFieldName("vsi");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("set<string>");
f.setFieldName("vss");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("set<double>");
f.setFieldName("vsd");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("set<long>");
f.setFieldName("vsl");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("set<short>");
f.setFieldName("vssh");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("set<Test>");
f.setFieldName("vsyo");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("multiset<int>");
f.setFieldName("vmsi");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("multiset<string>");
f.setFieldName("vmss");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("multiset<double>");
f.setFieldName("vmsd");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("multiset<long>");
f.setFieldName("vmsl");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("multiset<short>");
f.setFieldName("vmssh");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("multiset<Test>");
f.setFieldName("vmsyo");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("std::queue<int>");
f.setFieldName("vqi");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("std::queue<string>");
f.setFieldName("vqs");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("std::queue<double>");
f.setFieldName("vqd");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("std::queue<long>");
f.setFieldName("vql");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("std::queue<bool>");
f.setFieldName("vqb");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("std::queue<short>");
f.setFieldName("vqsh");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("std::queue<YObject>");
f.setFieldName("vqyo");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("list<int>");
f.setFieldName("vpppli");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("list<string>");
f.setFieldName("vpppls");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("list<double>");
f.setFieldName("vpppld");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("list<long>");
f.setFieldName("vpppll");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("list<bool>");
f.setFieldName("vppplb");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("list<short>");
f.setFieldName("vppplsh");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("list<YObject>");
f.setFieldName("vppplyo");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("vector<int>");
f.setFieldName("vpppvi");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("vector<string>");
f.setFieldName("vpppvs");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("vector<double>");
f.setFieldName("vpppvd");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("vector<long>");
f.setFieldName("vpppvl");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("vector<bool>");
f.setFieldName("vpppvb");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("vector<short>");
f.setFieldName("vpppvsh");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("vector<YObject>");
f.setFieldName("vpppvyo");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("deque<int>");
f.setFieldName("vpppdi");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("deque<string>");
f.setFieldName("vpppds");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("deque<double>");
f.setFieldName("vpppdd");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("deque<long>");
f.setFieldName("vpppdl");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("deque<bool>");
f.setFieldName("vpppdb");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("deque<short>");
f.setFieldName("vpppdsh");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("deque<YObject>");
f.setFieldName("vpppdyo");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("set<int>");
f.setFieldName("vpppsi");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("set<string>");
f.setFieldName("vpppss");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("set<double>");
f.setFieldName("vpppsd");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("set<long>");
f.setFieldName("vpppsl");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("set<short>");
f.setFieldName("vpppssh");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("set<Test>");
f.setFieldName("vpppsyo");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("multiset<int>");
f.setFieldName("vpppmsi");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("multiset<string>");
f.setFieldName("vpppmss");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("multiset<double>");
f.setFieldName("vpppmsd");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("multiset<long>");
f.setFieldName("vpppmsl");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("multiset<short>");
f.setFieldName("vpppmssh");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("multiset<Test>");
f.setFieldName("vpppmsyo");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("std::queue<int>");
f.setFieldName("vpppqi");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("std::queue<string>");
f.setFieldName("vpppqs");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("std::queue<double>");
f.setFieldName("vpppqd");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("std::queue<long>");
f.setFieldName("vpppql");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("std::queue<bool>");
f.setFieldName("vpppqb");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("std::queue<short>");
f.setFieldName("vpppqsh");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("std::queue<YObject>");
f.setFieldName("vpppqyo");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("YObject");
f.setFieldName("yobjectp");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("YObject");
f.setFieldName("yobject");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
ctor.clear();
me.clear();
me.setReturnType("deque<int>");
me.setMethodName("TestSTLsgetVpdi");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("deque<int>");
me.setMethodName("TestSTLssetVpdidequetsintte");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("list<int>");
me.setMethodName("TestSTLsgetVpli");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("list<int>");
me.setMethodName("TestSTLssetVplilisttsintte");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("set<int>");
me.setMethodName("TestSTLsgetVpls");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("set<int>");
me.setMethodName("TestSTLssetVplssettsintte");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("multiset<int>");
me.setMethodName("TestSTLsgetVpmsi");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("multiset<int>");
me.setMethodName("TestSTLssetVpmsimultisettsintte");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("std::queue<int>");
me.setMethodName("TestSTLsgetVpqi");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("std::queue<int>");
me.setMethodName("TestSTLssetVpqistd_queuetsintte");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("vector<int>");
me.setMethodName("TestSTLsgetVpvi");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("vector<int>");
me.setMethodName("TestSTLssetVpvivectortsintte");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("deque<int>*");
me.setMethodName("TestSTLsgetVppdi");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("deque<int>*");
me.setMethodName("TestSTLssetVppdidequetsintteptr");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("list<int>*");
me.setMethodName("TestSTLsgetVppli");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("list<int>*");
me.setMethodName("TestSTLssetVpplilisttsintteptr");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("set<int>*");
me.setMethodName("TestSTLsgetVppls");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("set<int>*");
me.setMethodName("TestSTLssetVpplssettsintteptr");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("multiset<int>*");
me.setMethodName("TestSTLsgetVppmsi");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("multiset<int>*");
me.setMethodName("TestSTLssetVppmsimultisettsintteptr");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("std::queue<int>*");
me.setMethodName("TestSTLsgetVppqi");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("std::queue<int>*");
me.setMethodName("TestSTLssetVppqistd_queuetsintteptr");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("vector<int>*");
me.setMethodName("TestSTLsgetVppvi");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("vector<int>*");
me.setMethodName("TestSTLssetVppvivectortsintteptr");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
ctor.clear();
me.clear();
ctor.setName("TestSTLs");
ctor.setArgumentTypes(argu);
argu.clear();
classInfo.addConstructor(ctor);
ctor.clear();
me.clear();
me.setReturnType("string");
me.setMethodName("TestSTLstoString");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
f.clear();
f.setType("list<int>");
f.setFieldName("vpli");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("vector<int>");
f.setFieldName("vpvi");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("set<int>");
f.setFieldName("vpls");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("multiset<int>");
f.setFieldName("vpmsi");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("std::queue<int>");
f.setFieldName("vpqi");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("deque<int>");
f.setFieldName("vpdi");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("list<int>");
f.setFieldName("vppli");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("vector<int>");
f.setFieldName("vppvi");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("set<int>");
f.setFieldName("vppls");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("multiset<int>");
f.setFieldName("vppmsi");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("std::queue<int>");
f.setFieldName("vppqi");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("deque<int>");
f.setFieldName("vppdi");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}

return classInfo;
}
ClassInfo defaultgetReflectionCIForTesting()
{
ClassInfo classInfo;
classInfo.setClassName("Testing");
classInfo.setNamespace("");
classInfo.setBase(" ");
Constructor ctor;
Method me;
Field f;
args argu;
ctor.clear();
me.clear();
ctor.setName("Testing");
ctor.setArgumentTypes(argu);
argu.clear();
classInfo.addConstructor(ctor);
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("string");
me.setMethodName("Testingtest1string");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("string");
me.setMethodName("Testingtest2");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("Test");
me.setMethodName("Testingtest3Test");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("Test");
argu.push_back("string");
me.setMethodName("Testingtest4string");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}

return classInfo;
}
ClassInfo defaultgetReflectionCIForYObject()
{
ClassInfo classInfo;
classInfo.setClassName("YObject");
classInfo.setNamespace("");
classInfo.setBase(" ");
Constructor ctor;
Method me;
Field f;
args argu;
f.clear();
f.setType("int");
f.setFieldName("i");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("string");
f.setFieldName("j");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("float");
f.setFieldName("c");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
ctor.setName("YObject");
argu.clear();
ctor.setArgumentTypes(argu);
argu.clear();
classInfo.addConstructor(ctor);

return classInfo;
}
ClassInfo defaultgetReflectionCIForcom_obj_TestObject()
{
ClassInfo classInfo;
classInfo.setClassName("TestObject");
classInfo.setNamespace("com::obj::");
classInfo.setBase(" ");
Constructor ctor;
Method me;
Field f;
args argu;
ctor.clear();
me.clear();
ctor.setName("com_obj_TestObject");
ctor.setArgumentTypes(argu);
argu.clear();
classInfo.addConstructor(ctor);
ctor.clear();
me.clear();
me.setReturnType("vector<short>");
me.setMethodName("com_obj_TestObjectgetA");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("vector<short>");
me.setMethodName("com_obj_TestObjectsetAvectortsshortte");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("vector<int>");
me.setMethodName("com_obj_TestObjectgetB");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("vector<int>");
me.setMethodName("com_obj_TestObjectsetBvectortsintte");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("vector<long>");
me.setMethodName("com_obj_TestObjectgetC");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("vector<long>");
me.setMethodName("com_obj_TestObjectsetCvectortslongte");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("vector<long long>");
me.setMethodName("com_obj_TestObjectgetD");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("vector<long long>");
me.setMethodName("com_obj_TestObjectsetDvectortslonglongte");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("vector<unsigned short>");
me.setMethodName("com_obj_TestObjectgetE");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("vector<unsigned short>");
me.setMethodName("com_obj_TestObjectsetEvectortsunsignedshortte");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("vector<unsigned int>");
me.setMethodName("com_obj_TestObjectgetF");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("vector<unsigned int>");
me.setMethodName("com_obj_TestObjectsetFvectortsunsignedintte");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("vector<unsigned long>");
me.setMethodName("com_obj_TestObjectgetG");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("vector<unsigned long>");
me.setMethodName("com_obj_TestObjectsetGvectortsunsignedlongte");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("vector<unsigned long long>");
me.setMethodName("com_obj_TestObjectgetH");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("vector<unsigned long long>");
me.setMethodName("com_obj_TestObjectsetHvectortsunsignedlonglongte");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("vector<float>");
me.setMethodName("com_obj_TestObjectgetI");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("vector<float>");
me.setMethodName("com_obj_TestObjectsetIvectortsfloatte");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("vector<double>");
me.setMethodName("com_obj_TestObjectgetJ");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("vector<double>");
me.setMethodName("com_obj_TestObjectsetJvectortsdoublete");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("vector<bool>");
me.setMethodName("com_obj_TestObjectgetK");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("vector<bool>");
me.setMethodName("com_obj_TestObjectsetKvectortsboolte");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("vector<string>");
me.setMethodName("com_obj_TestObjectgetL");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("vector<string>");
me.setMethodName("com_obj_TestObjectsetLvectortsstringte");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("string");
me.setMethodName("com_obj_TestObjecttoString");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
f.clear();
f.setType("vector<short>");
f.setFieldName("a");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("vector<int>");
f.setFieldName("b");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("vector<long>");
f.setFieldName("c");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("vector<long long>");
f.setFieldName("d");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("vector<unsigned short>");
f.setFieldName("e");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("vector<unsigned int>");
f.setFieldName("f");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("vector<unsigned long>");
f.setFieldName("g");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("vector<unsigned long long>");
f.setFieldName("h");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("vector<float>");
f.setFieldName("i");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("vector<double>");
f.setFieldName("j");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("vector<bool>");
f.setFieldName("k");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("vector<string>");
f.setFieldName("l");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}

return classInfo;
}
ClassInfo defaultgetReflectionCIFortest_all_in_one_TestAllInOne()
{
ClassInfo classInfo;
classInfo.setClassName("TestAllInOne");
classInfo.setNamespace("test::all::in::one::");
classInfo.setBase(" ");
Constructor ctor;
Method me;
Field f;
args argu;
f.clear();
f.setType("TestObject");
f.setFieldName("to");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("com::obj::TestObject");
f.setFieldName("coto");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("vector<TestObject>");
f.setFieldName("vto");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("vector<com::obj::TestObject>");
f.setFieldName("vcoto");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("vector<vector<TestObject> >");
f.setFieldName("vvto");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
f.clear();
f.setType("vector<vector<com::obj::TestObject> >");
f.setFieldName("vvcoto");
if(f.getFieldName()!="")
{
classInfo.addField(f);
}
ctor.clear();
me.clear();
me.setReturnType("string");
me.setMethodName("test_all_in_one_TestAllInOnetoString");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.setName("test_all_in_one_TestAllInOne");
argu.clear();
ctor.setArgumentTypes(argu);
argu.clear();
classInfo.addConstructor(ctor);

return classInfo;
}
ClassInfo defaultgetReflectionCIForws_test_TestingWS()
{
ClassInfo classInfo;
classInfo.setClassName("TestingWS");
classInfo.setNamespace("ws::test::");
classInfo.setBase(" ");
Constructor ctor;
Method me;
Field f;
args argu;
ctor.clear();
me.clear();
ctor.setName("ws_test_TestingWS");
ctor.setArgumentTypes(argu);
argu.clear();
classInfo.addConstructor(ctor);
ctor.clear();
me.clear();
me.setReturnType("void");
argu.push_back("int");
argu.push_back("string");
argu.push_back("long");
me.setMethodName("ws_test_TestingWSwsmeth1intstringlong");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("string");
argu.push_back("string");
argu.push_back("vector<int>");
me.setMethodName("ws_test_TestingWSwsmeth2stringvectortsintte");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("TestObject");
argu.push_back("string");
me.setMethodName("ws_test_TestingWSwsmeth3string");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("com::obj::TestObject");
argu.push_back("bool");
me.setMethodName("ws_test_TestingWSwsmeth4bool");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("string");
argu.push_back("TestObject");
me.setMethodName("ws_test_TestingWSwsmeth5TestObject");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}
ctor.clear();
me.clear();
me.setReturnType("long");
argu.push_back("com::obj::TestObject");
me.setMethodName("ws_test_TestingWSwsmeth6com_obj_TestObject");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}

return classInfo;
}
ClassInfo flexAppgetReflectionCIForFlexAppController()
{
ClassInfo classInfo;
classInfo.setClassName("FlexAppController");
classInfo.setNamespace("");
classInfo.setBase("public Controller");
Constructor ctor;
Method me;
Field f;
args argu;
ctor.clear();
me.clear();
ctor.setName("FlexAppController");
ctor.setArgumentTypes(argu);
argu.clear();
classInfo.addConstructor(ctor);
ctor.clear();
me.clear();
me.setReturnType("HttpResponse");
argu.push_back("HttpRequest");
me.setMethodName("FlexAppControllerserviceHttpRequest");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}

return classInfo;
}
ClassInfo oauthAppgetReflectionCIForOAUTH2Controller()
{
ClassInfo classInfo;
classInfo.setClassName("OAUTH2Controller");
classInfo.setNamespace("");
classInfo.setBase("public Controller");
Constructor ctor;
Method me;
Field f;
args argu;
ctor.clear();
me.clear();
ctor.setName("OAUTH2Controller");
ctor.setArgumentTypes(argu);
argu.clear();
classInfo.addConstructor(ctor);
ctor.clear();
me.clear();
me.setReturnType("HttpResponse");
argu.push_back("HttpRequest");
me.setMethodName("OAUTH2ControllerserviceHttpRequest");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}

return classInfo;
}
ClassInfo oauthAppgetReflectionCIForOAUTHController()
{
ClassInfo classInfo;
classInfo.setClassName("OAUTHController");
classInfo.setNamespace("");
classInfo.setBase("public Controller");
Constructor ctor;
Method me;
Field f;
args argu;
ctor.clear();
me.clear();
ctor.setName("OAUTHController");
ctor.setArgumentTypes(argu);
argu.clear();
classInfo.addConstructor(ctor);
ctor.clear();
me.clear();
me.setReturnType("HttpResponse");
argu.push_back("HttpRequest");
me.setMethodName("OAUTHControllerserviceHttpRequest");
me.setArgumentTypes(argu);
argu.clear();
if(me.getMethodName()!="")
{
classInfo.addMethod(me);
}

return classInfo;
}

void* defaultinvokeReflectionCICtorForDefTemp(vals values)
{
	DefTemp *_retVal = NULL;

	_retVal = (new DefTemp());
	return _retVal;
}
void defaultinvokeReflectionCIDtorForDefTemp(void* instance)
{
	DefTemp *_obj = (DefTemp*)instance;
	_obj->~DefTemp();
}
void* defaultinvokeReflectionCIMethodForDefTempgetContext(void* instance,vals values)
{
	DefTemp *_obj = (DefTemp*)instance;

	Context *_retVal=new Context;

	*_retVal = (_obj->getContext());
	return _retVal;
}
void* defaultinvokeReflectionCICtorForDefaultController(vals values)
{
	DefaultController *_retVal = NULL;

	_retVal = (new DefaultController());
	return _retVal;
}
void defaultinvokeReflectionCIDtorForDefaultController(void* instance)
{
	DefaultController *_obj = (DefaultController*)instance;
	_obj->~DefaultController();
}
void* defaultinvokeReflectionCIMethodForDefaultControllerserviceHttpRequest(void* instance,vals values)
{
	DefaultController *_obj = (DefaultController*)instance;

	HttpResponse *_retVal=new HttpResponse;
		HttpRequest *_0 = (HttpRequest*)values.at(0);
	*_retVal = (_obj->service(*_0));
	return _retVal;
}
void* defaultinvokeReflectionCICtorForDefaultIOFilter(vals values)
{
	DefaultIOFilter *_retVal = NULL;

	_retVal = (new DefaultIOFilter());
	return _retVal;
}
void defaultinvokeReflectionCIDtorForDefaultIOFilter(void* instance)
{
	DefaultIOFilter *_obj = (DefaultIOFilter*)instance;
	_obj->~DefaultIOFilter();
}
void* defaultinvokeReflectionCIMethodForDefaultIOFilterdoInputFilterHttpRequestptr(void* instance,vals values)
{
	DefaultIOFilter *_obj = (DefaultIOFilter*)instance;
		HttpRequest *_0 = (HttpRequest*)values.at(0);
	void* returnValue=NULL;
	_obj->doInputFilter(_0);
	return returnValue;
}
void* defaultinvokeReflectionCIMethodForDefaultIOFilterdoOutputFilterHttpResponseptr(void* instance,vals values)
{
	DefaultIOFilter *_obj = (DefaultIOFilter*)instance;
		HttpResponse *_0 = (HttpResponse*)values.at(0);
	void* returnValue=NULL;
	_obj->doOutputFilter(_0);
	return returnValue;
}
void* defaultinvokeReflectionCIMethodForDefaultIOFilterdoHandleHttpRequestptrHttpResponseptr(void* instance,vals values)
{
	DefaultIOFilter *_obj = (DefaultIOFilter*)instance;

	bool *_retVal=new bool;
		HttpRequest *_0 = (HttpRequest*)values.at(0);		HttpResponse *_1 = (HttpResponse*)values.at(1);
	*_retVal = (_obj->doHandle(_0,_1));
	return _retVal;
}
void* defaultinvokeReflectionCICtorForDefaultOAUTHController(vals values)
{
	DefaultOAUTHController *_retVal = NULL;

	_retVal = (new DefaultOAUTHController());
	return _retVal;
}
void defaultinvokeReflectionCIDtorForDefaultOAUTHController(void* instance)
{
	DefaultOAUTHController *_obj = (DefaultOAUTHController*)instance;
	_obj->~DefaultOAUTHController();
}
void* defaultinvokeReflectionCIMethodForDefaultOAUTHControllerhandleHttpRequestptrHttpResponseptr(void* instance,vals values)
{
	DefaultOAUTHController *_obj = (DefaultOAUTHController*)instance;

	bool *_retVal=new bool;
		HttpRequest *_0 = (HttpRequest*)values.at(0);		HttpResponse *_1 = (HttpResponse*)values.at(1);
	*_retVal = (_obj->handle(_0,_1));
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForDefaultOAUTHControllerauthenticatestringstring(void* instance,vals values)
{
	DefaultOAUTHController *_obj = (DefaultOAUTHController*)instance;

	bool *_retVal=new bool;
		string *_0 = (string*)values.at(0);		string *_1 = (string*)values.at(1);
	*_retVal = (_obj->authenticate(*_0,*_1));
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForDefaultOAUTHControllerauthenticateSecuritystringstring(void* instance,vals values)
{
	DefaultOAUTHController *_obj = (DefaultOAUTHController*)instance;

	bool *_retVal=new bool;
		string *_0 = (string*)values.at(0);		string *_1 = (string*)values.at(1);
	*_retVal = (_obj->authenticateSecurity(*_0,*_1));
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForDefaultOAUTHControllerisInitialized(void* instance,vals values)
{
	DefaultOAUTHController *_obj = (DefaultOAUTHController*)instance;

	bool *_retVal=new bool;

	*_retVal = (_obj->isInitialized());
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForDefaultOAUTHControllerauthenticatemaptsstringstringte(void* instance,vals values)
{
	DefaultOAUTHController *_obj = (DefaultOAUTHController*)instance;

	bool *_retVal=new bool;
		map<string,string> *_0 = (map<string,string>*)values.at(0);
	*_retVal = (_obj->authenticate(*_0));
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForDefaultOAUTHControllergetUserRolestring(void* instance,vals values)
{
	DefaultOAUTHController *_obj = (DefaultOAUTHController*)instance;

	string *_retVal=new string;
		string *_0 = (string*)values.at(0);
	*_retVal = (_obj->getUserRole(*_0));
	return _retVal;
}
void* defaultinvokeReflectionCICtorForDefaultRestController(vals values)
{
	DefaultRestController *_retVal = NULL;

	_retVal = (new DefaultRestController());
	return _retVal;
}
void defaultinvokeReflectionCIDtorForDefaultRestController(void* instance)
{
	DefaultRestController *_obj = (DefaultRestController*)instance;
	_obj->~DefaultRestController();
}
void* defaultinvokeReflectionCIMethodForDefaultRestControlleraddNumbersintint(void* instance,vals values)
{
	DefaultRestController *_obj = (DefaultRestController*)instance;
		int *_0 = (int*)values.at(0);		int *_1 = (int*)values.at(1);
	void* returnValue=NULL;
	_obj->addNumbers(*_0,*_1);
	return returnValue;
}
void* defaultinvokeReflectionCIMethodForDefaultRestControllerpowerintint(void* instance,vals values)
{
	DefaultRestController *_obj = (DefaultRestController*)instance;
		int *_0 = (int*)values.at(0);		int *_1 = (int*)values.at(1);
	void* returnValue=NULL;
	_obj->power(*_0,*_1);
	return returnValue;
}
void* defaultinvokeReflectionCIMethodForDefaultRestControllertestVectorvectortsintte(void* instance,vals values)
{
	DefaultRestController *_obj = (DefaultRestController*)instance;
		vector<int> *_0 = (vector<int>*)values.at(0);
	void* returnValue=NULL;
	_obj->testVector(*_0);
	return returnValue;
}
void* defaultinvokeReflectionCIMethodForDefaultRestControllertestObjectTestMany(void* instance,vals values)
{
	DefaultRestController *_obj = (DefaultRestController*)instance;
		TestMany *_0 = (TestMany*)values.at(0);
	void* returnValue=NULL;
	_obj->testObject(*_0);
	return returnValue;
}
void* defaultinvokeReflectionCIMethodForDefaultRestControllertestVectorObjectvectortsTestManyte(void* instance,vals values)
{
	DefaultRestController *_obj = (DefaultRestController*)instance;
		vector<TestMany> *_0 = (vector<TestMany>*)values.at(0);
	void* returnValue=NULL;
	_obj->testVectorObject(*_0);
	return returnValue;
}
void* defaultinvokeReflectionCIMethodForDefaultRestControllertestUploadFileifstreamptrstring(void* instance,vals values)
{
	DefaultRestController *_obj = (DefaultRestController*)instance;
		ifstream *_0 = (ifstream*)values.at(0);		string *_1 = (string*)values.at(1);
	void* returnValue=NULL;
	_obj->testUploadFile(_0,*_1);
	return returnValue;
}
void* defaultinvokeReflectionCIMethodForDefaultRestControllertestUploadFileMulti1ifstreamptrifstreamptrifstreamptrstring(void* instance,vals values)
{
	DefaultRestController *_obj = (DefaultRestController*)instance;
		ifstream *_0 = (ifstream*)values.at(0);		ifstream *_1 = (ifstream*)values.at(1);		ifstream *_2 = (ifstream*)values.at(2);		string *_3 = (string*)values.at(3);
	void* returnValue=NULL;
	_obj->testUploadFileMulti1(_0,_1,_2,*_3);
	return returnValue;
}
void* defaultinvokeReflectionCIMethodForDefaultRestControllertestUploadFileMulti2vectortsifstreamptrtestring(void* instance,vals values)
{
	DefaultRestController *_obj = (DefaultRestController*)instance;
		vector<ifstream*> *_0 = (vector<ifstream*>*)values.at(0);		string *_1 = (string*)values.at(1);
	void* returnValue=NULL;
	_obj->testUploadFileMulti2(*_0,*_1);
	return returnValue;
}
void* defaultinvokeReflectionCIMethodForDepDependencyBean1Implprint1(void* instance,vals values)
{
	DepDependencyBean1Impl *_obj = (DepDependencyBean1Impl*)instance;

	void* returnValue=NULL;
	_obj->print1();
	return returnValue;
}
void* defaultinvokeReflectionCICtorForDepDependencyBean1Impl(vals values)
{
	DepDependencyBean1Impl *_retVal = NULL;

	_retVal = (new DepDependencyBean1Impl());
	return _retVal;
}
void defaultinvokeReflectionCIDtorForDepDependencyBean1Impl(void* instance)
{
	DepDependencyBean1Impl *_obj = (DepDependencyBean1Impl*)instance;
	_obj->~DepDependencyBean1Impl();
}
void* defaultinvokeReflectionCIMethodForDepDependencyBean2Implprint2(void* instance,vals values)
{
	DepDependencyBean2Impl *_obj = (DepDependencyBean2Impl*)instance;

	void* returnValue=NULL;
	_obj->print2();
	return returnValue;
}
void* defaultinvokeReflectionCICtorForDepDependencyBean2Impl(vals values)
{
	DepDependencyBean2Impl *_retVal = NULL;

	_retVal = (new DepDependencyBean2Impl());
	return _retVal;
}
void defaultinvokeReflectionCIDtorForDepDependencyBean2Impl(void* instance)
{
	DepDependencyBean2Impl *_obj = (DepDependencyBean2Impl*)instance;
	_obj->~DepDependencyBean2Impl();
}
void* defaultinvokeReflectionCIMethodForDependencyBean1print(void* instance,vals values)
{
	DependencyBean1 *_obj = (DependencyBean1*)instance;

	void* returnValue=NULL;
	_obj->print();
	return returnValue;
}
void* defaultinvokeReflectionCICtorForDependencyBean1(vals values)
{
	DependencyBean1 *_retVal = NULL;

	_retVal = (new DependencyBean1());
	return _retVal;
}
void defaultinvokeReflectionCIDtorForDependencyBean1(void* instance)
{
	DependencyBean1 *_obj = (DependencyBean1*)instance;
	_obj->~DependencyBean1();
}
void* defaultinvokeReflectionCIMethodForDependencyBean2print(void* instance,vals values)
{
	DependencyBean2 *_obj = (DependencyBean2*)instance;

	void* returnValue=NULL;
	_obj->print();
	return returnValue;
}
void* defaultinvokeReflectionCICtorForDependencyBean2(vals values)
{
	DependencyBean2 *_retVal = NULL;

	_retVal = (new DependencyBean2());
	return _retVal;
}
void defaultinvokeReflectionCIDtorForDependencyBean2(void* instance)
{
	DependencyBean2 *_obj = (DependencyBean2*)instance;
	_obj->~DependencyBean2();
}
void* defaultinvokeReflectionCIMethodForDependencyBean3print(void* instance,vals values)
{
	DependencyBean3 *_obj = (DependencyBean3*)instance;

	void* returnValue=NULL;
	_obj->print();
	return returnValue;
}
void* defaultinvokeReflectionCICtorForDependencyBean3(vals values)
{
	DependencyBean3 *_retVal = NULL;

	_retVal = (new DependencyBean3());
	return _retVal;
}
void defaultinvokeReflectionCIDtorForDependencyBean3(void* instance)
{
	DependencyBean3 *_obj = (DependencyBean3*)instance;
	_obj->~DependencyBean3();
}
void* defaultinvokeReflectionCIMethodForDependencyBean4print(void* instance,vals values)
{
	DependencyBean4 *_obj = (DependencyBean4*)instance;

	void* returnValue=NULL;
	_obj->print();
	return returnValue;
}
void* defaultinvokeReflectionCICtorForDependencyBean4(vals values)
{
	DependencyBean4 *_retVal = NULL;

	_retVal = (new DependencyBean4());
	return _retVal;
}
void defaultinvokeReflectionCIDtorForDependencyBean4(void* instance)
{
	DependencyBean4 *_obj = (DependencyBean4*)instance;
	_obj->~DependencyBean4();
}
void* defaultinvokeReflectionCICtorForDview(vals values)
{
	Dview *_retVal = NULL;

	_retVal = (new Dview());
	return _retVal;
}
void defaultinvokeReflectionCIDtorForDview(void* instance)
{
	Dview *_obj = (Dview*)instance;
	_obj->~Dview();
}
void* defaultinvokeReflectionCIMethodForDviewgetDocument(void* instance,vals values)
{
	Dview *_obj = (Dview*)instance;

	Document *_retVal=new Document;

	*_retVal = (_obj->getDocument());
	return _retVal;
}
void* defaultinvokeReflectionCICtorForExpose(vals values)
{
	Expose *_retVal = NULL;

	_retVal = (new Expose());
	return _retVal;
}
void defaultinvokeReflectionCIDtorForExpose(void* instance)
{
	Expose *_obj = (Expose*)instance;
	_obj->~Expose();
}
void* defaultinvokeReflectionCIMethodForExposesayHellostringintfloat(void* instance,vals values)
{
	Expose *_obj = (Expose*)instance;

	YObject *_retVal=new YObject;
		string *_0 = (string*)values.at(0);		int *_1 = (int*)values.at(1);		float *_2 = (float*)values.at(2);
	*_retVal = (_obj->sayHello(*_0,*_1,*_2));
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForExposesayHello1stringintfloat(void* instance,vals values)
{
	Expose *_obj = (Expose*)instance;

	string *_retVal=new string;
		string *_0 = (string*)values.at(0);		int *_1 = (int*)values.at(1);		float *_2 = (float*)values.at(2);
	*_retVal = (_obj->sayHello1(*_0,*_1,*_2));
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForExposesayHello2YObjectintfloat(void* instance,vals values)
{
	Expose *_obj = (Expose*)instance;

	YObject *_retVal=new YObject;
		YObject *_0 = (YObject*)values.at(0);		int *_1 = (int*)values.at(1);		float *_2 = (float*)values.at(2);
	*_retVal = (_obj->sayHello2(*_0,*_1,*_2));
	return _retVal;
}
void* defaultinvokeReflectionCICtorForService1(vals values)
{
	Service1 *_retVal = NULL;

	_retVal = (new Service1());
	return _retVal;
}
void defaultinvokeReflectionCIDtorForService1(void* instance)
{
	Service1 *_obj = (Service1*)instance;
	_obj->~Service1();
}
void* defaultinvokeReflectionCIMethodForService1service1(void* instance,vals values)
{
	Service1 *_obj = (Service1*)instance;

	string *_retVal=new string;

	*_retVal = (_obj->service1());
	return _retVal;
}
void* defaultinvokeReflectionCICtorForService2(vals values)
{
	Service2 *_retVal = NULL;

	_retVal = (new Service2());
	return _retVal;
}
void defaultinvokeReflectionCIDtorForService2(void* instance)
{
	Service2 *_obj = (Service2*)instance;
	_obj->~Service2();
}
void* defaultinvokeReflectionCIMethodForService2service2string(void* instance,vals values)
{
	Service2 *_obj = (Service2*)instance;

	string *_retVal=new string;
		string *_0 = (string*)values.at(0);
	*_retVal = (_obj->service2(*_0));
	return _retVal;
}
void* defaultinvokeReflectionCICtorForTempo(vals values)
{
	Tempo *_retVal = NULL;

	_retVal = (new Tempo());
	return _retVal;
}
void defaultinvokeReflectionCIDtorForTempo(void* instance)
{
	Tempo *_obj = (Tempo*)instance;
	_obj->~Tempo();
}
void* defaultinvokeReflectionCIMethodForTempoprin1(void* instance,vals values)
{
	Tempo *_obj = (Tempo*)instance;

	void* returnValue=NULL;
	_obj->prin1();
	return returnValue;
}
void* defaultinvokeReflectionCICtorForTest2(vals values)
{
	Test2 *_retVal = NULL;

	_retVal = (new Test2());
	return _retVal;
}
void defaultinvokeReflectionCIDtorForTest2(void* instance)
{
	Test2 *_obj = (Test2*)instance;
	_obj->~Test2();
}
void* defaultinvokeReflectionCIMethodForTest2getId(void* instance,vals values)
{
	Test2 *_obj = (Test2*)instance;

	int *_retVal=new int;

	*_retVal = (_obj->getId());
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForTest2setIdint(void* instance,vals values)
{
	Test2 *_obj = (Test2*)instance;
		int *_0 = (int*)values.at(0);
	void* returnValue=NULL;
	_obj->setId(*_0);
	return returnValue;
}
void* defaultinvokeReflectionCIMethodForTest2getTest_id(void* instance,vals values)
{
	Test2 *_obj = (Test2*)instance;

	int *_retVal=new int;

	*_retVal = (_obj->getTest_id());
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForTest2setTest_idint(void* instance,vals values)
{
	Test2 *_obj = (Test2*)instance;
		int *_0 = (int*)values.at(0);
	void* returnValue=NULL;
	_obj->setTest_id(*_0);
	return returnValue;
}
void* defaultinvokeReflectionCIMethodForTest2getTest(void* instance,vals values)
{
	Test2 *_obj = (Test2*)instance;

	Test *_retVal=new Test;

	*_retVal = (_obj->getTest());
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForTest2setTestTest(void* instance,vals values)
{
	Test2 *_obj = (Test2*)instance;
		Test *_0 = (Test*)values.at(0);
	void* returnValue=NULL;
	_obj->setTest(*_0);
	return returnValue;
}
void* defaultinvokeReflectionCICtorForTest3(vals values)
{
	Test3 *_retVal = NULL;

	_retVal = (new Test3());
	return _retVal;
}
void defaultinvokeReflectionCIDtorForTest3(void* instance)
{
	Test3 *_obj = (Test3*)instance;
	_obj->~Test3();
}
void* defaultinvokeReflectionCIMethodForTest3getId(void* instance,vals values)
{
	Test3 *_obj = (Test3*)instance;

	int *_retVal=new int;

	*_retVal = (_obj->getId());
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForTest3setIdint(void* instance,vals values)
{
	Test3 *_obj = (Test3*)instance;
		int *_0 = (int*)values.at(0);
	void* returnValue=NULL;
	_obj->setId(*_0);
	return returnValue;
}
void* defaultinvokeReflectionCIMethodForTest3getTests(void* instance,vals values)
{
	Test3 *_obj = (Test3*)instance;

	vector<Test> *_retVal=new vector<Test>;

	*_retVal = (_obj->getTests());
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForTest3setTestsvectortsTestte(void* instance,vals values)
{
	Test3 *_obj = (Test3*)instance;
		vector<Test> *_0 = (vector<Test>*)values.at(0);
	void* returnValue=NULL;
	_obj->setTests(*_0);
	return returnValue;
}
void* defaultinvokeReflectionCIMethodForTest3getTest_id(void* instance,vals values)
{
	Test3 *_obj = (Test3*)instance;

	int *_retVal=new int;

	*_retVal = (_obj->getTest_id());
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForTest3setTest_idint(void* instance,vals values)
{
	Test3 *_obj = (Test3*)instance;
		int *_0 = (int*)values.at(0);
	void* returnValue=NULL;
	_obj->setTest_id(*_0);
	return returnValue;
}
void* defaultinvokeReflectionCICtorForTest4(vals values)
{
	Test4 *_retVal = NULL;

	_retVal = (new Test4());
	return _retVal;
}
void defaultinvokeReflectionCIDtorForTest4(void* instance)
{
	Test4 *_obj = (Test4*)instance;
	_obj->~Test4();
}
void* defaultinvokeReflectionCIMethodForTest4getDate(void* instance,vals values)
{
	Test4 *_obj = (Test4*)instance;

	Date *_retVal=new Date;

	*_retVal = (_obj->getDate());
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForTest4setDateDate(void* instance,vals values)
{
	Test4 *_obj = (Test4*)instance;
		Date *_0 = (Date*)values.at(0);
	void* returnValue=NULL;
	_obj->setDate(*_0);
	return returnValue;
}
void* defaultinvokeReflectionCIMethodForTest4getDatt(void* instance,vals values)
{
	Test4 *_obj = (Test4*)instance;

	Date *_retVal=new Date;

	*_retVal = (_obj->getDatt());
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForTest4setDattDate(void* instance,vals values)
{
	Test4 *_obj = (Test4*)instance;
		Date *_0 = (Date*)values.at(0);
	void* returnValue=NULL;
	_obj->setDatt(*_0);
	return returnValue;
}
void* defaultinvokeReflectionCIMethodForTest4getDattm(void* instance,vals values)
{
	Test4 *_obj = (Test4*)instance;

	Date *_retVal=new Date;

	*_retVal = (_obj->getDattm());
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForTest4setDattmDate(void* instance,vals values)
{
	Test4 *_obj = (Test4*)instance;
		Date *_0 = (Date*)values.at(0);
	void* returnValue=NULL;
	_obj->setDattm(*_0);
	return returnValue;
}
void* defaultinvokeReflectionCIMethodForTest4getBinar(void* instance,vals values)
{
	Test4 *_obj = (Test4*)instance;

	BinaryData *_retVal=new BinaryData;

	*_retVal = (_obj->getBinar());
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForTest4setBinarBinaryData(void* instance,vals values)
{
	Test4 *_obj = (Test4*)instance;
		BinaryData *_0 = (BinaryData*)values.at(0);
	void* returnValue=NULL;
	_obj->setBinar(*_0);
	return returnValue;
}
void* defaultinvokeReflectionCICtorForTest(vals values)
{
	Test *_retVal = NULL;

	_retVal = (new Test());
	return _retVal;
}
void defaultinvokeReflectionCIDtorForTest(void* instance)
{
	Test *_obj = (Test*)instance;
	_obj->~Test();
}
void* defaultinvokeReflectionCIMethodForTestgetId(void* instance,vals values)
{
	Test *_obj = (Test*)instance;

	int *_retVal=new int;

	*_retVal = (_obj->getId());
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForTestsetIdint(void* instance,vals values)
{
	Test *_obj = (Test*)instance;
		int *_0 = (int*)values.at(0);
	void* returnValue=NULL;
	_obj->setId(*_0);
	return returnValue;
}
void* defaultinvokeReflectionCIMethodForTestgetName(void* instance,vals values)
{
	Test *_obj = (Test*)instance;

	string *_retVal=new string;

	*_retVal = (_obj->getName());
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForTestsetNamestring(void* instance,vals values)
{
	Test *_obj = (Test*)instance;
		string *_0 = (string*)values.at(0);
	void* returnValue=NULL;
	_obj->setName(*_0);
	return returnValue;
}
void* defaultinvokeReflectionCICtorForTestBeanCons(vals values)
{
	TestBeanCons *_retVal = NULL;

	_retVal = (new TestBeanCons());
	return _retVal;
}
void* defaultinvokeReflectionCICtorForTestBeanConsintboolDependencyBean4(vals values)
{
	TestBeanCons *_retVal = NULL;
		int *_0 = (int*)values.at(0);		bool *_1 = (bool*)values.at(1);		DependencyBean4 *_2 = (DependencyBean4*)values.at(2);
	_retVal = (new TestBeanCons(*_0,*_1,*_2));
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForTestBeanConsgetIntProp(void* instance,vals values)
{
	TestBeanCons *_obj = (TestBeanCons*)instance;

	int *_retVal=new int;

	*_retVal = (_obj->getIntProp());
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForTestBeanConsgetBoolProp(void* instance,vals values)
{
	TestBeanCons *_obj = (TestBeanCons*)instance;

	bool *_retVal=new bool;

	*_retVal = (_obj->getBoolProp());
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForTestBeanConsgetDependencyBean4(void* instance,vals values)
{
	TestBeanCons *_obj = (TestBeanCons*)instance;

	DependencyBean4 *_retVal=new DependencyBean4;

	*_retVal = (_obj->getDependencyBean4());
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForTestBeanConsprint(void* instance,vals values)
{
	TestBeanCons *_obj = (TestBeanCons*)instance;

	void* returnValue=NULL;
	_obj->print();
	return returnValue;
}
void defaultinvokeReflectionCIDtorForTestBeanCons(void* instance)
{
	TestBeanCons *_obj = (TestBeanCons*)instance;
	_obj->~TestBeanCons();
}
void* defaultinvokeReflectionCIMethodForTestBeanIntfsetDependencyIntf1DependencyIntf1ptr(void* instance,vals values)
{
	TestBeanIntf *_obj = (TestBeanIntf*)instance;
		DependencyIntf1 *_0 = (DependencyIntf1*)values.at(0);
	void* returnValue=NULL;
	_obj->setDependencyIntf1(_0);
	return returnValue;
}
void* defaultinvokeReflectionCIMethodForTestBeanIntfsetDependencyIntf2DependencyIntf2ptr(void* instance,vals values)
{
	TestBeanIntf *_obj = (TestBeanIntf*)instance;
		DependencyIntf2 *_0 = (DependencyIntf2*)values.at(0);
	void* returnValue=NULL;
	_obj->setDependencyIntf2(_0);
	return returnValue;
}
void* defaultinvokeReflectionCIMethodForTestBeanIntfprint(void* instance,vals values)
{
	TestBeanIntf *_obj = (TestBeanIntf*)instance;

	void* returnValue=NULL;
	_obj->print();
	return returnValue;
}
void* defaultinvokeReflectionCICtorForTestBeanIntf(vals values)
{
	TestBeanIntf *_retVal = NULL;

	_retVal = (new TestBeanIntf());
	return _retVal;
}
void defaultinvokeReflectionCIDtorForTestBeanIntf(void* instance)
{
	TestBeanIntf *_obj = (TestBeanIntf*)instance;
	_obj->~TestBeanIntf();
}
void* defaultinvokeReflectionCIMethodForTestBeanPropsetStrPropstringptr(void* instance,vals values)
{
	TestBeanProp *_obj = (TestBeanProp*)instance;
		string *_0 = (string*)values.at(0);
	void* returnValue=NULL;
	_obj->setStrProp(_0);
	return returnValue;
}
void* defaultinvokeReflectionCIMethodForTestBeanPropgetStrProp(void* instance,vals values)
{
	TestBeanProp *_obj = (TestBeanProp*)instance;

	string* *_retVal=new string*;

	*_retVal = (_obj->getStrProp());
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForTestBeanPropsetDependencyBean1DependencyBean1ptr(void* instance,vals values)
{
	TestBeanProp *_obj = (TestBeanProp*)instance;
		DependencyBean1 *_0 = (DependencyBean1*)values.at(0);
	void* returnValue=NULL;
	_obj->setDependencyBean1(_0);
	return returnValue;
}
void* defaultinvokeReflectionCIMethodForTestBeanPropgetDependencyBean1(void* instance,vals values)
{
	TestBeanProp *_obj = (TestBeanProp*)instance;

	DependencyBean1* *_retVal=new DependencyBean1*;

	*_retVal = (_obj->getDependencyBean1());
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForTestBeanPropsetDependencyBean2DependencyBean2ptr(void* instance,vals values)
{
	TestBeanProp *_obj = (TestBeanProp*)instance;
		DependencyBean2 *_0 = (DependencyBean2*)values.at(0);
	void* returnValue=NULL;
	_obj->setDependencyBean2(_0);
	return returnValue;
}
void* defaultinvokeReflectionCIMethodForTestBeanPropgetDependencyBean2(void* instance,vals values)
{
	TestBeanProp *_obj = (TestBeanProp*)instance;

	DependencyBean2* *_retVal=new DependencyBean2*;

	*_retVal = (_obj->getDependencyBean2());
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForTestBeanPropsetDependencyBean3DependencyBean3ptr(void* instance,vals values)
{
	TestBeanProp *_obj = (TestBeanProp*)instance;
		DependencyBean3 *_0 = (DependencyBean3*)values.at(0);
	void* returnValue=NULL;
	_obj->setDependencyBean3(_0);
	return returnValue;
}
void* defaultinvokeReflectionCIMethodForTestBeanPropgetDependencyBean3(void* instance,vals values)
{
	TestBeanProp *_obj = (TestBeanProp*)instance;

	DependencyBean3* *_retVal=new DependencyBean3*;

	*_retVal = (_obj->getDependencyBean3());
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForTestBeanPropprint(void* instance,vals values)
{
	TestBeanProp *_obj = (TestBeanProp*)instance;

	void* returnValue=NULL;
	_obj->print();
	return returnValue;
}
void* defaultinvokeReflectionCICtorForTestBeanProp(vals values)
{
	TestBeanProp *_retVal = NULL;

	_retVal = (new TestBeanProp());
	return _retVal;
}
void defaultinvokeReflectionCIDtorForTestBeanProp(void* instance)
{
	TestBeanProp *_obj = (TestBeanProp*)instance;
	_obj->~TestBeanProp();
}
void* defaultinvokeReflectionCICtorForTestCronBasedJob(vals values)
{
	TestCronBasedJob *_retVal = NULL;

	_retVal = (new TestCronBasedJob());
	return _retVal;
}
void defaultinvokeReflectionCIDtorForTestCronBasedJob(void* instance)
{
	TestCronBasedJob *_obj = (TestCronBasedJob*)instance;
	_obj->~TestCronBasedJob();
}
void* defaultinvokeReflectionCIMethodForTestCronBasedJobrunJob(void* instance,vals values)
{
	TestCronBasedJob *_obj = (TestCronBasedJob*)instance;

	void* returnValue=NULL;
	_obj->runJob();
	return returnValue;
}
void* defaultinvokeReflectionCICtorForTestForm(vals values)
{
	TestForm *_retVal = NULL;

	_retVal = (new TestForm());
	return _retVal;
}
void defaultinvokeReflectionCIDtorForTestForm(void* instance)
{
	TestForm *_obj = (TestForm*)instance;
	_obj->~TestForm();
}
void* defaultinvokeReflectionCIMethodForTestFormgetChe(void* instance,vals values)
{
	TestForm *_obj = (TestForm*)instance;

	string *_retVal=new string;

	*_retVal = (_obj->getChe());
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForTestFormgetNum(void* instance,vals values)
{
	TestForm *_obj = (TestForm*)instance;

	int *_retVal=new int;

	*_retVal = (_obj->getNum());
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForTestFormgetTxt(void* instance,vals values)
{
	TestForm *_obj = (TestForm*)instance;

	string *_retVal=new string;

	*_retVal = (_obj->getTxt());
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForTestFormsetChestring(void* instance,vals values)
{
	TestForm *_obj = (TestForm*)instance;
		string *_0 = (string*)values.at(0);
	void* returnValue=NULL;
	_obj->setChe(*_0);
	return returnValue;
}
void* defaultinvokeReflectionCIMethodForTestFormsetNumint(void* instance,vals values)
{
	TestForm *_obj = (TestForm*)instance;
		int *_0 = (int*)values.at(0);
	void* returnValue=NULL;
	_obj->setNum(*_0);
	return returnValue;
}
void* defaultinvokeReflectionCIMethodForTestFormsetTxtstring(void* instance,vals values)
{
	TestForm *_obj = (TestForm*)instance;
		string *_0 = (string*)values.at(0);
	void* returnValue=NULL;
	_obj->setTxt(*_0);
	return returnValue;
}
void* defaultinvokeReflectionCICtorForTestFormController(vals values)
{
	TestFormController *_retVal = NULL;

	_retVal = (new TestFormController());
	return _retVal;
}
void defaultinvokeReflectionCIDtorForTestFormController(void* instance)
{
	TestFormController *_obj = (TestFormController*)instance;
	_obj->~TestFormController();
}
void* defaultinvokeReflectionCIMethodForTestFormControlleronSubmitvoidptrHttpResponseptr(void* instance,vals values)
{
	TestFormController *_obj = (TestFormController*)instance;
		void *_0 = (void*)values.at(0);		HttpResponse *_1 = (HttpResponse*)values.at(1);
	void* returnValue=NULL;
	_obj->onSubmit(_0,_1);
	return returnValue;
}
Test defaultinvokeReflectionCIFieldForTestManyt(void* instance)
{
	TestMany *_obj = (TestMany*)instance;
	return _obj->t;
}

int defaultinvokeReflectionCIFieldForTestManyy(void* instance)
{
	TestMany *_obj = (TestMany*)instance;
	return _obj->y;
}

vector<int> defaultinvokeReflectionCIFieldForTestManyvi(void* instance)
{
	TestMany *_obj = (TestMany*)instance;
	return _obj->vi;
}

vector<string> defaultinvokeReflectionCIFieldForTestManyvs(void* instance)
{
	TestMany *_obj = (TestMany*)instance;
	return _obj->vs;
}

vector<double> defaultinvokeReflectionCIFieldForTestManyvd(void* instance)
{
	TestMany *_obj = (TestMany*)instance;
	return _obj->vd;
}

vector<long> defaultinvokeReflectionCIFieldForTestManyvl(void* instance)
{
	TestMany *_obj = (TestMany*)instance;
	return _obj->vl;
}

vector<bool> defaultinvokeReflectionCIFieldForTestManyvb(void* instance)
{
	TestMany *_obj = (TestMany*)instance;
	return _obj->vb;
}

vector<short> defaultinvokeReflectionCIFieldForTestManyvsh(void* instance)
{
	TestMany *_obj = (TestMany*)instance;
	return _obj->vsh;
}

vector<YObject> defaultinvokeReflectionCIFieldForTestManyvyo(void* instance)
{
	TestMany *_obj = (TestMany*)instance;
	return _obj->vyo;
}

list<int> defaultinvokeReflectionCIFieldForTestManyli(void* instance)
{
	TestMany *_obj = (TestMany*)instance;
	return _obj->li;
}

std::queue<short> defaultinvokeReflectionCIFieldForTestManyqsh(void* instance)
{
	TestMany *_obj = (TestMany*)instance;
	return _obj->qsh;
}

void* defaultinvokeReflectionCIMethodForTestManysetVpivectortsintte(void* instance,vals values)
{
	TestMany *_obj = (TestMany*)instance;
		vector<int> *_0 = (vector<int>*)values.at(0);
	void* returnValue=NULL;
	_obj->setVpi(*_0);
	return returnValue;
}
void* defaultinvokeReflectionCIMethodForTestManygetVpi(void* instance,vals values)
{
	TestMany *_obj = (TestMany*)instance;

	vector<int> *_retVal=new vector<int>;

	*_retVal = (_obj->getVpi());
	return _retVal;
}
void* defaultinvokeReflectionCICtorForTestMany(vals values)
{
	TestMany *_retVal = NULL;

	_retVal = (new TestMany());
	return _retVal;
}
void defaultinvokeReflectionCIDtorForTestMany(void* instance)
{
	TestMany *_obj = (TestMany*)instance;
	_obj->~TestMany();
}
void* defaultinvokeReflectionCICtorForTestObject(vals values)
{
	TestObject *_retVal = NULL;

	_retVal = (new TestObject());
	return _retVal;
}
void defaultinvokeReflectionCIDtorForTestObject(void* instance)
{
	TestObject *_obj = (TestObject*)instance;
	_obj->~TestObject();
}
void* defaultinvokeReflectionCIMethodForTestObjectgetA(void* instance,vals values)
{
	TestObject *_obj = (TestObject*)instance;

	int *_retVal=new int;

	*_retVal = (_obj->getA());
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForTestObjectsetAint(void* instance,vals values)
{
	TestObject *_obj = (TestObject*)instance;
		int *_0 = (int*)values.at(0);
	void* returnValue=NULL;
	_obj->setA(*_0);
	return returnValue;
}
void* defaultinvokeReflectionCIMethodForTestObjectgetB(void* instance,vals values)
{
	TestObject *_obj = (TestObject*)instance;

	long *_retVal=new long;

	*_retVal = (_obj->getB());
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForTestObjectsetBlong(void* instance,vals values)
{
	TestObject *_obj = (TestObject*)instance;
		long *_0 = (long*)values.at(0);
	void* returnValue=NULL;
	_obj->setB(*_0);
	return returnValue;
}
void* defaultinvokeReflectionCIMethodForTestObjectgetC(void* instance,vals values)
{
	TestObject *_obj = (TestObject*)instance;

	string *_retVal=new string;

	*_retVal = (_obj->getC());
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForTestObjectsetCstring(void* instance,vals values)
{
	TestObject *_obj = (TestObject*)instance;
		string *_0 = (string*)values.at(0);
	void* returnValue=NULL;
	_obj->setC(*_0);
	return returnValue;
}
void* defaultinvokeReflectionCIMethodForTestObjectgetD(void* instance,vals values)
{
	TestObject *_obj = (TestObject*)instance;

	float *_retVal=new float;

	*_retVal = (_obj->getD());
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForTestObjectsetDfloat(void* instance,vals values)
{
	TestObject *_obj = (TestObject*)instance;
		float *_0 = (float*)values.at(0);
	void* returnValue=NULL;
	_obj->setD(*_0);
	return returnValue;
}
void* defaultinvokeReflectionCIMethodForTestObjectgetE(void* instance,vals values)
{
	TestObject *_obj = (TestObject*)instance;

	double *_retVal=new double;

	*_retVal = (_obj->getE());
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForTestObjectsetEdouble(void* instance,vals values)
{
	TestObject *_obj = (TestObject*)instance;
		double *_0 = (double*)values.at(0);
	void* returnValue=NULL;
	_obj->setE(*_0);
	return returnValue;
}
void* defaultinvokeReflectionCIMethodForTestObjectisF(void* instance,vals values)
{
	TestObject *_obj = (TestObject*)instance;

	bool *_retVal=new bool;

	*_retVal = (_obj->isF());
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForTestObjectsetFbool(void* instance,vals values)
{
	TestObject *_obj = (TestObject*)instance;
		bool *_0 = (bool*)values.at(0);
	void* returnValue=NULL;
	_obj->setF(*_0);
	return returnValue;
}
void* defaultinvokeReflectionCIMethodForTestObjectgetG(void* instance,vals values)
{
	TestObject *_obj = (TestObject*)instance;

	long long *_retVal=new long long;

	*_retVal = (_obj->getG());
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForTestObjectsetGlonglong(void* instance,vals values)
{
	TestObject *_obj = (TestObject*)instance;
		long long *_0 = (long long*)values.at(0);
	void* returnValue=NULL;
	_obj->setG(*_0);
	return returnValue;
}
void* defaultinvokeReflectionCIMethodForTestObjectgetH(void* instance,vals values)
{
	TestObject *_obj = (TestObject*)instance;

	short *_retVal=new short;

	*_retVal = (_obj->getH());
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForTestObjectsetHshort(void* instance,vals values)
{
	TestObject *_obj = (TestObject*)instance;
		short *_0 = (short*)values.at(0);
	void* returnValue=NULL;
	_obj->setH(*_0);
	return returnValue;
}
void* defaultinvokeReflectionCIMethodForTestObjectgetI(void* instance,vals values)
{
	TestObject *_obj = (TestObject*)instance;

	unsigned short *_retVal=new unsigned short;

	*_retVal = (_obj->getI());
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForTestObjectsetIunsignedshort(void* instance,vals values)
{
	TestObject *_obj = (TestObject*)instance;
		unsigned short *_0 = (unsigned short*)values.at(0);
	void* returnValue=NULL;
	_obj->setI(*_0);
	return returnValue;
}
void* defaultinvokeReflectionCIMethodForTestObjectgetJ(void* instance,vals values)
{
	TestObject *_obj = (TestObject*)instance;

	unsigned int *_retVal=new unsigned int;

	*_retVal = (_obj->getJ());
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForTestObjectsetJunsignedint(void* instance,vals values)
{
	TestObject *_obj = (TestObject*)instance;
		unsigned int *_0 = (unsigned int*)values.at(0);
	void* returnValue=NULL;
	_obj->setJ(*_0);
	return returnValue;
}
void* defaultinvokeReflectionCIMethodForTestObjectgetK(void* instance,vals values)
{
	TestObject *_obj = (TestObject*)instance;

	unsigned long *_retVal=new unsigned long;

	*_retVal = (_obj->getK());
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForTestObjectsetKunsignedlong(void* instance,vals values)
{
	TestObject *_obj = (TestObject*)instance;
		unsigned long *_0 = (unsigned long*)values.at(0);
	void* returnValue=NULL;
	_obj->setK(*_0);
	return returnValue;
}
void* defaultinvokeReflectionCIMethodForTestObjectgetL(void* instance,vals values)
{
	TestObject *_obj = (TestObject*)instance;

	unsigned long long *_retVal=new unsigned long long;

	*_retVal = (_obj->getL());
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForTestObjectsetLunsignedlonglong(void* instance,vals values)
{
	TestObject *_obj = (TestObject*)instance;
		unsigned long long *_0 = (unsigned long long*)values.at(0);
	void* returnValue=NULL;
	_obj->setL(*_0);
	return returnValue;
}
void* defaultinvokeReflectionCIMethodForTestObjecttoString(void* instance,vals values)
{
	TestObject *_obj = (TestObject*)instance;

	string *_retVal=new string;

	*_retVal = (_obj->toString());
	return _retVal;
}
void* defaultinvokeReflectionCICtorForTestPage(vals values)
{
	TestPage *_retVal = NULL;

	_retVal = (new TestPage());
	return _retVal;
}
void defaultinvokeReflectionCIDtorForTestPage(void* instance)
{
	TestPage *_obj = (TestPage*)instance;
	_obj->~TestPage();
}
void* defaultinvokeReflectionCIMethodForTestPagetextonclickintstringstring(void* instance,vals values)
{
	TestPage *_obj = (TestPage*)instance;

	string *_retVal=new string;
		int *_0 = (int*)values.at(0);		string *_1 = (string*)values.at(1);		string *_2 = (string*)values.at(2);
	*_retVal = (_obj->textonclick(*_0,*_1,*_2));
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForTestPagelinkonclick(void* instance,vals values)
{
	TestPage *_obj = (TestPage*)instance;

	int *_retVal=new int;

	*_retVal = (_obj->linkonclick());
	return _retVal;
}
int defaultinvokeReflectionCIFieldForTestSTLsmi(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->mi;
}

list<int> defaultinvokeReflectionCIFieldForTestSTLsvli(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vli;
}

list<string> defaultinvokeReflectionCIFieldForTestSTLsvls(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vls;
}

list<double> defaultinvokeReflectionCIFieldForTestSTLsvld(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vld;
}

list<long> defaultinvokeReflectionCIFieldForTestSTLsvll(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vll;
}

list<bool> defaultinvokeReflectionCIFieldForTestSTLsvlb(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vlb;
}

list<short> defaultinvokeReflectionCIFieldForTestSTLsvlsh(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vlsh;
}

list<YObject> defaultinvokeReflectionCIFieldForTestSTLsvlyo(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vlyo;
}

vector<int> defaultinvokeReflectionCIFieldForTestSTLsvvi(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vvi;
}

vector<string> defaultinvokeReflectionCIFieldForTestSTLsvvs(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vvs;
}

vector<double> defaultinvokeReflectionCIFieldForTestSTLsvvd(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vvd;
}

vector<long> defaultinvokeReflectionCIFieldForTestSTLsvvl(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vvl;
}

vector<bool> defaultinvokeReflectionCIFieldForTestSTLsvvb(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vvb;
}

vector<short> defaultinvokeReflectionCIFieldForTestSTLsvvsh(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vvsh;
}

vector<YObject> defaultinvokeReflectionCIFieldForTestSTLsvvyo(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vvyo;
}

deque<int> defaultinvokeReflectionCIFieldForTestSTLsvdi(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vdi;
}

deque<string> defaultinvokeReflectionCIFieldForTestSTLsvds(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vds;
}

deque<double> defaultinvokeReflectionCIFieldForTestSTLsvdd(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vdd;
}

deque<long> defaultinvokeReflectionCIFieldForTestSTLsvdl(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vdl;
}

deque<bool> defaultinvokeReflectionCIFieldForTestSTLsvdb(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vdb;
}

deque<short> defaultinvokeReflectionCIFieldForTestSTLsvdsh(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vdsh;
}

deque<YObject> defaultinvokeReflectionCIFieldForTestSTLsvdyo(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vdyo;
}

set<int> defaultinvokeReflectionCIFieldForTestSTLsvsi(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vsi;
}

set<string> defaultinvokeReflectionCIFieldForTestSTLsvss(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vss;
}

set<double> defaultinvokeReflectionCIFieldForTestSTLsvsd(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vsd;
}

set<long> defaultinvokeReflectionCIFieldForTestSTLsvsl(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vsl;
}

set<short> defaultinvokeReflectionCIFieldForTestSTLsvssh(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vssh;
}

set<Test> defaultinvokeReflectionCIFieldForTestSTLsvsyo(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vsyo;
}

multiset<int> defaultinvokeReflectionCIFieldForTestSTLsvmsi(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vmsi;
}

multiset<string> defaultinvokeReflectionCIFieldForTestSTLsvmss(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vmss;
}

multiset<double> defaultinvokeReflectionCIFieldForTestSTLsvmsd(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vmsd;
}

multiset<long> defaultinvokeReflectionCIFieldForTestSTLsvmsl(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vmsl;
}

multiset<short> defaultinvokeReflectionCIFieldForTestSTLsvmssh(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vmssh;
}

multiset<Test> defaultinvokeReflectionCIFieldForTestSTLsvmsyo(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vmsyo;
}

std::queue<int> defaultinvokeReflectionCIFieldForTestSTLsvqi(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vqi;
}

std::queue<string> defaultinvokeReflectionCIFieldForTestSTLsvqs(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vqs;
}

std::queue<double> defaultinvokeReflectionCIFieldForTestSTLsvqd(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vqd;
}

std::queue<long> defaultinvokeReflectionCIFieldForTestSTLsvql(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vql;
}

std::queue<bool> defaultinvokeReflectionCIFieldForTestSTLsvqb(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vqb;
}

std::queue<short> defaultinvokeReflectionCIFieldForTestSTLsvqsh(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vqsh;
}

std::queue<YObject> defaultinvokeReflectionCIFieldForTestSTLsvqyo(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vqyo;
}

list<int>* defaultinvokeReflectionCIFieldForTestSTLsvpppli(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vpppli;
}

list<string>* defaultinvokeReflectionCIFieldForTestSTLsvpppls(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vpppls;
}

list<double>* defaultinvokeReflectionCIFieldForTestSTLsvpppld(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vpppld;
}

list<long>* defaultinvokeReflectionCIFieldForTestSTLsvpppll(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vpppll;
}

list<bool>* defaultinvokeReflectionCIFieldForTestSTLsvppplb(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vppplb;
}

list<short>* defaultinvokeReflectionCIFieldForTestSTLsvppplsh(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vppplsh;
}

list<YObject>* defaultinvokeReflectionCIFieldForTestSTLsvppplyo(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vppplyo;
}

vector<int>* defaultinvokeReflectionCIFieldForTestSTLsvpppvi(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vpppvi;
}

vector<string>* defaultinvokeReflectionCIFieldForTestSTLsvpppvs(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vpppvs;
}

vector<double>* defaultinvokeReflectionCIFieldForTestSTLsvpppvd(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vpppvd;
}

vector<long>* defaultinvokeReflectionCIFieldForTestSTLsvpppvl(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vpppvl;
}

vector<bool>* defaultinvokeReflectionCIFieldForTestSTLsvpppvb(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vpppvb;
}

vector<short>* defaultinvokeReflectionCIFieldForTestSTLsvpppvsh(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vpppvsh;
}

vector<YObject>* defaultinvokeReflectionCIFieldForTestSTLsvpppvyo(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vpppvyo;
}

deque<int>* defaultinvokeReflectionCIFieldForTestSTLsvpppdi(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vpppdi;
}

deque<string>* defaultinvokeReflectionCIFieldForTestSTLsvpppds(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vpppds;
}

deque<double>* defaultinvokeReflectionCIFieldForTestSTLsvpppdd(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vpppdd;
}

deque<long>* defaultinvokeReflectionCIFieldForTestSTLsvpppdl(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vpppdl;
}

deque<bool>* defaultinvokeReflectionCIFieldForTestSTLsvpppdb(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vpppdb;
}

deque<short>* defaultinvokeReflectionCIFieldForTestSTLsvpppdsh(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vpppdsh;
}

deque<YObject>* defaultinvokeReflectionCIFieldForTestSTLsvpppdyo(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vpppdyo;
}

set<int>* defaultinvokeReflectionCIFieldForTestSTLsvpppsi(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vpppsi;
}

set<string>* defaultinvokeReflectionCIFieldForTestSTLsvpppss(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vpppss;
}

set<double>* defaultinvokeReflectionCIFieldForTestSTLsvpppsd(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vpppsd;
}

set<long>* defaultinvokeReflectionCIFieldForTestSTLsvpppsl(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vpppsl;
}

set<short>* defaultinvokeReflectionCIFieldForTestSTLsvpppssh(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vpppssh;
}

set<Test>* defaultinvokeReflectionCIFieldForTestSTLsvpppsyo(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vpppsyo;
}

multiset<int>* defaultinvokeReflectionCIFieldForTestSTLsvpppmsi(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vpppmsi;
}

multiset<string>* defaultinvokeReflectionCIFieldForTestSTLsvpppmss(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vpppmss;
}

multiset<double>* defaultinvokeReflectionCIFieldForTestSTLsvpppmsd(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vpppmsd;
}

multiset<long>* defaultinvokeReflectionCIFieldForTestSTLsvpppmsl(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vpppmsl;
}

multiset<short>* defaultinvokeReflectionCIFieldForTestSTLsvpppmssh(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vpppmssh;
}

multiset<Test>* defaultinvokeReflectionCIFieldForTestSTLsvpppmsyo(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vpppmsyo;
}

std::queue<int>* defaultinvokeReflectionCIFieldForTestSTLsvpppqi(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vpppqi;
}

std::queue<string>* defaultinvokeReflectionCIFieldForTestSTLsvpppqs(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vpppqs;
}

std::queue<double>* defaultinvokeReflectionCIFieldForTestSTLsvpppqd(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vpppqd;
}

std::queue<long>* defaultinvokeReflectionCIFieldForTestSTLsvpppql(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vpppql;
}

std::queue<bool>* defaultinvokeReflectionCIFieldForTestSTLsvpppqb(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vpppqb;
}

std::queue<short>* defaultinvokeReflectionCIFieldForTestSTLsvpppqsh(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vpppqsh;
}

std::queue<YObject>* defaultinvokeReflectionCIFieldForTestSTLsvpppqyo(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->vpppqyo;
}

YObject* defaultinvokeReflectionCIFieldForTestSTLsyobjectp(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->yobjectp;
}

YObject defaultinvokeReflectionCIFieldForTestSTLsyobject(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	return _obj->yobject;
}

void* defaultinvokeReflectionCIMethodForTestSTLsgetVpdi(void* instance,vals values)
{
	TestSTLs *_obj = (TestSTLs*)instance;

	deque<int> *_retVal=new deque<int>;

	*_retVal = (_obj->getVpdi());
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForTestSTLssetVpdidequetsintte(void* instance,vals values)
{
	TestSTLs *_obj = (TestSTLs*)instance;
		deque<int> *_0 = (deque<int>*)values.at(0);
	void* returnValue=NULL;
	_obj->setVpdi(*_0);
	return returnValue;
}
void* defaultinvokeReflectionCIMethodForTestSTLsgetVpli(void* instance,vals values)
{
	TestSTLs *_obj = (TestSTLs*)instance;

	list<int> *_retVal=new list<int>;

	*_retVal = (_obj->getVpli());
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForTestSTLssetVplilisttsintte(void* instance,vals values)
{
	TestSTLs *_obj = (TestSTLs*)instance;
		list<int> *_0 = (list<int>*)values.at(0);
	void* returnValue=NULL;
	_obj->setVpli(*_0);
	return returnValue;
}
void* defaultinvokeReflectionCIMethodForTestSTLsgetVpls(void* instance,vals values)
{
	TestSTLs *_obj = (TestSTLs*)instance;

	set<int> *_retVal=new set<int>;

	*_retVal = (_obj->getVpls());
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForTestSTLssetVplssettsintte(void* instance,vals values)
{
	TestSTLs *_obj = (TestSTLs*)instance;
		set<int> *_0 = (set<int>*)values.at(0);
	void* returnValue=NULL;
	_obj->setVpls(*_0);
	return returnValue;
}
void* defaultinvokeReflectionCIMethodForTestSTLsgetVpmsi(void* instance,vals values)
{
	TestSTLs *_obj = (TestSTLs*)instance;

	multiset<int> *_retVal=new multiset<int>;

	*_retVal = (_obj->getVpmsi());
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForTestSTLssetVpmsimultisettsintte(void* instance,vals values)
{
	TestSTLs *_obj = (TestSTLs*)instance;
		multiset<int> *_0 = (multiset<int>*)values.at(0);
	void* returnValue=NULL;
	_obj->setVpmsi(*_0);
	return returnValue;
}
void* defaultinvokeReflectionCIMethodForTestSTLsgetVpqi(void* instance,vals values)
{
	TestSTLs *_obj = (TestSTLs*)instance;

	std::queue<int> *_retVal=new std::queue<int>;

	*_retVal = (_obj->getVpqi());
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForTestSTLssetVpqistd_queuetsintte(void* instance,vals values)
{
	TestSTLs *_obj = (TestSTLs*)instance;
		std::queue<int> *_0 = (std::queue<int>*)values.at(0);
	void* returnValue=NULL;
	_obj->setVpqi(*_0);
	return returnValue;
}
void* defaultinvokeReflectionCIMethodForTestSTLsgetVpvi(void* instance,vals values)
{
	TestSTLs *_obj = (TestSTLs*)instance;

	vector<int> *_retVal=new vector<int>;

	*_retVal = (_obj->getVpvi());
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForTestSTLssetVpvivectortsintte(void* instance,vals values)
{
	TestSTLs *_obj = (TestSTLs*)instance;
		vector<int> *_0 = (vector<int>*)values.at(0);
	void* returnValue=NULL;
	_obj->setVpvi(*_0);
	return returnValue;
}
void* defaultinvokeReflectionCIMethodForTestSTLsgetVppdi(void* instance,vals values)
{
	TestSTLs *_obj = (TestSTLs*)instance;

	deque<int>* *_retVal=new deque<int>*;

	*_retVal = (_obj->getVppdi());
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForTestSTLssetVppdidequetsintteptr(void* instance,vals values)
{
	TestSTLs *_obj = (TestSTLs*)instance;
		deque<int> *_0 = (deque<int>*)values.at(0);
	void* returnValue=NULL;
	_obj->setVppdi(_0);
	return returnValue;
}
void* defaultinvokeReflectionCIMethodForTestSTLsgetVppli(void* instance,vals values)
{
	TestSTLs *_obj = (TestSTLs*)instance;

	list<int>* *_retVal=new list<int>*;

	*_retVal = (_obj->getVppli());
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForTestSTLssetVpplilisttsintteptr(void* instance,vals values)
{
	TestSTLs *_obj = (TestSTLs*)instance;
		list<int> *_0 = (list<int>*)values.at(0);
	void* returnValue=NULL;
	_obj->setVppli(_0);
	return returnValue;
}
void* defaultinvokeReflectionCIMethodForTestSTLsgetVppls(void* instance,vals values)
{
	TestSTLs *_obj = (TestSTLs*)instance;

	set<int>* *_retVal=new set<int>*;

	*_retVal = (_obj->getVppls());
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForTestSTLssetVpplssettsintteptr(void* instance,vals values)
{
	TestSTLs *_obj = (TestSTLs*)instance;
		set<int> *_0 = (set<int>*)values.at(0);
	void* returnValue=NULL;
	_obj->setVppls(_0);
	return returnValue;
}
void* defaultinvokeReflectionCIMethodForTestSTLsgetVppmsi(void* instance,vals values)
{
	TestSTLs *_obj = (TestSTLs*)instance;

	multiset<int>* *_retVal=new multiset<int>*;

	*_retVal = (_obj->getVppmsi());
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForTestSTLssetVppmsimultisettsintteptr(void* instance,vals values)
{
	TestSTLs *_obj = (TestSTLs*)instance;
		multiset<int> *_0 = (multiset<int>*)values.at(0);
	void* returnValue=NULL;
	_obj->setVppmsi(_0);
	return returnValue;
}
void* defaultinvokeReflectionCIMethodForTestSTLsgetVppqi(void* instance,vals values)
{
	TestSTLs *_obj = (TestSTLs*)instance;

	std::queue<int>* *_retVal=new std::queue<int>*;

	*_retVal = (_obj->getVppqi());
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForTestSTLssetVppqistd_queuetsintteptr(void* instance,vals values)
{
	TestSTLs *_obj = (TestSTLs*)instance;
		std::queue<int> *_0 = (std::queue<int>*)values.at(0);
	void* returnValue=NULL;
	_obj->setVppqi(_0);
	return returnValue;
}
void* defaultinvokeReflectionCIMethodForTestSTLsgetVppvi(void* instance,vals values)
{
	TestSTLs *_obj = (TestSTLs*)instance;

	vector<int>* *_retVal=new vector<int>*;

	*_retVal = (_obj->getVppvi());
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForTestSTLssetVppvivectortsintteptr(void* instance,vals values)
{
	TestSTLs *_obj = (TestSTLs*)instance;
		vector<int> *_0 = (vector<int>*)values.at(0);
	void* returnValue=NULL;
	_obj->setVppvi(_0);
	return returnValue;
}
void* defaultinvokeReflectionCICtorForTestSTLs(vals values)
{
	TestSTLs *_retVal = NULL;

	_retVal = (new TestSTLs());
	return _retVal;
}
void defaultinvokeReflectionCIDtorForTestSTLs(void* instance)
{
	TestSTLs *_obj = (TestSTLs*)instance;
	_obj->~TestSTLs();
}
void* defaultinvokeReflectionCIMethodForTestSTLstoString(void* instance,vals values)
{
	TestSTLs *_obj = (TestSTLs*)instance;

	string *_retVal=new string;

	*_retVal = (_obj->toString());
	return _retVal;
}
void* defaultinvokeReflectionCICtorForTesting(vals values)
{
	Testing *_retVal = NULL;

	_retVal = (new Testing());
	return _retVal;
}
void defaultinvokeReflectionCIDtorForTesting(void* instance)
{
	Testing *_obj = (Testing*)instance;
	_obj->~Testing();
}
void* defaultinvokeReflectionCIMethodForTestingtest1string(void* instance,vals values)
{
	Testing *_obj = (Testing*)instance;
		string *_0 = (string*)values.at(0);
	void* returnValue=NULL;
	_obj->test1(*_0);
	return returnValue;
}
void* defaultinvokeReflectionCIMethodForTestingtest2(void* instance,vals values)
{
	Testing *_obj = (Testing*)instance;

	string *_retVal=new string;

	*_retVal = (_obj->test2());
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForTestingtest3Test(void* instance,vals values)
{
	Testing *_obj = (Testing*)instance;
		Test *_0 = (Test*)values.at(0);
	void* returnValue=NULL;
	_obj->test3(*_0);
	return returnValue;
}
void* defaultinvokeReflectionCIMethodForTestingtest4string(void* instance,vals values)
{
	Testing *_obj = (Testing*)instance;

	Test *_retVal=new Test;
		string *_0 = (string*)values.at(0);
	*_retVal = (_obj->test4(*_0));
	return _retVal;
}
int defaultinvokeReflectionCIFieldForYObjecti(void* instance)
{
	YObject *_obj = (YObject*)instance;
	return _obj->i;
}

string defaultinvokeReflectionCIFieldForYObjectj(void* instance)
{
	YObject *_obj = (YObject*)instance;
	return _obj->j;
}

float defaultinvokeReflectionCIFieldForYObjectc(void* instance)
{
	YObject *_obj = (YObject*)instance;
	return _obj->c;
}

void* defaultinvokeReflectionCICtorForYObject(vals values)
{
	YObject *_retVal = NULL;

	_retVal = (new YObject());
	return _retVal;
}
void defaultinvokeReflectionCIDtorForYObject(void* instance)
{
	YObject *_obj = (YObject*)instance;
	_obj->~YObject();
}
void* defaultinvokeReflectionCICtorForcom_obj_TestObject(vals values)
{
	com::obj::TestObject *_retVal = NULL;

	_retVal = (new com::obj::TestObject());
	return _retVal;
}
void defaultinvokeReflectionCIDtorForcom_obj_TestObject(void* instance)
{
	com::obj::TestObject *_obj = (com::obj::TestObject*)instance;
	_obj->~TestObject();
}
void* defaultinvokeReflectionCIMethodForcom_obj_TestObjectgetA(void* instance,vals values)
{
	com::obj::TestObject *_obj = (com::obj::TestObject*)instance;

	vector<short> *_retVal=new vector<short>;

	*_retVal = (_obj->getA());
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForcom_obj_TestObjectsetAvectortsshortte(void* instance,vals values)
{
	com::obj::TestObject *_obj = (com::obj::TestObject*)instance;
		vector<short> *_0 = (vector<short>*)values.at(0);
	void* returnValue=NULL;
	_obj->setA(*_0);
	return returnValue;
}
void* defaultinvokeReflectionCIMethodForcom_obj_TestObjectgetB(void* instance,vals values)
{
	com::obj::TestObject *_obj = (com::obj::TestObject*)instance;

	vector<int> *_retVal=new vector<int>;

	*_retVal = (_obj->getB());
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForcom_obj_TestObjectsetBvectortsintte(void* instance,vals values)
{
	com::obj::TestObject *_obj = (com::obj::TestObject*)instance;
		vector<int> *_0 = (vector<int>*)values.at(0);
	void* returnValue=NULL;
	_obj->setB(*_0);
	return returnValue;
}
void* defaultinvokeReflectionCIMethodForcom_obj_TestObjectgetC(void* instance,vals values)
{
	com::obj::TestObject *_obj = (com::obj::TestObject*)instance;

	vector<long> *_retVal=new vector<long>;

	*_retVal = (_obj->getC());
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForcom_obj_TestObjectsetCvectortslongte(void* instance,vals values)
{
	com::obj::TestObject *_obj = (com::obj::TestObject*)instance;
		vector<long> *_0 = (vector<long>*)values.at(0);
	void* returnValue=NULL;
	_obj->setC(*_0);
	return returnValue;
}
void* defaultinvokeReflectionCIMethodForcom_obj_TestObjectgetD(void* instance,vals values)
{
	com::obj::TestObject *_obj = (com::obj::TestObject*)instance;

	vector<long long> *_retVal=new vector<long long>;

	*_retVal = (_obj->getD());
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForcom_obj_TestObjectsetDvectortslonglongte(void* instance,vals values)
{
	com::obj::TestObject *_obj = (com::obj::TestObject*)instance;
		vector<long long> *_0 = (vector<long long>*)values.at(0);
	void* returnValue=NULL;
	_obj->setD(*_0);
	return returnValue;
}
void* defaultinvokeReflectionCIMethodForcom_obj_TestObjectgetE(void* instance,vals values)
{
	com::obj::TestObject *_obj = (com::obj::TestObject*)instance;

	vector<unsigned short> *_retVal=new vector<unsigned short>;

	*_retVal = (_obj->getE());
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForcom_obj_TestObjectsetEvectortsunsignedshortte(void* instance,vals values)
{
	com::obj::TestObject *_obj = (com::obj::TestObject*)instance;
		vector<unsigned short> *_0 = (vector<unsigned short>*)values.at(0);
	void* returnValue=NULL;
	_obj->setE(*_0);
	return returnValue;
}
void* defaultinvokeReflectionCIMethodForcom_obj_TestObjectgetF(void* instance,vals values)
{
	com::obj::TestObject *_obj = (com::obj::TestObject*)instance;

	vector<unsigned int> *_retVal=new vector<unsigned int>;

	*_retVal = (_obj->getF());
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForcom_obj_TestObjectsetFvectortsunsignedintte(void* instance,vals values)
{
	com::obj::TestObject *_obj = (com::obj::TestObject*)instance;
		vector<unsigned int> *_0 = (vector<unsigned int>*)values.at(0);
	void* returnValue=NULL;
	_obj->setF(*_0);
	return returnValue;
}
void* defaultinvokeReflectionCIMethodForcom_obj_TestObjectgetG(void* instance,vals values)
{
	com::obj::TestObject *_obj = (com::obj::TestObject*)instance;

	vector<unsigned long> *_retVal=new vector<unsigned long>;

	*_retVal = (_obj->getG());
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForcom_obj_TestObjectsetGvectortsunsignedlongte(void* instance,vals values)
{
	com::obj::TestObject *_obj = (com::obj::TestObject*)instance;
		vector<unsigned long> *_0 = (vector<unsigned long>*)values.at(0);
	void* returnValue=NULL;
	_obj->setG(*_0);
	return returnValue;
}
void* defaultinvokeReflectionCIMethodForcom_obj_TestObjectgetH(void* instance,vals values)
{
	com::obj::TestObject *_obj = (com::obj::TestObject*)instance;

	vector<unsigned long long> *_retVal=new vector<unsigned long long>;

	*_retVal = (_obj->getH());
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForcom_obj_TestObjectsetHvectortsunsignedlonglongte(void* instance,vals values)
{
	com::obj::TestObject *_obj = (com::obj::TestObject*)instance;
		vector<unsigned long long> *_0 = (vector<unsigned long long>*)values.at(0);
	void* returnValue=NULL;
	_obj->setH(*_0);
	return returnValue;
}
void* defaultinvokeReflectionCIMethodForcom_obj_TestObjectgetI(void* instance,vals values)
{
	com::obj::TestObject *_obj = (com::obj::TestObject*)instance;

	vector<float> *_retVal=new vector<float>;

	*_retVal = (_obj->getI());
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForcom_obj_TestObjectsetIvectortsfloatte(void* instance,vals values)
{
	com::obj::TestObject *_obj = (com::obj::TestObject*)instance;
		vector<float> *_0 = (vector<float>*)values.at(0);
	void* returnValue=NULL;
	_obj->setI(*_0);
	return returnValue;
}
void* defaultinvokeReflectionCIMethodForcom_obj_TestObjectgetJ(void* instance,vals values)
{
	com::obj::TestObject *_obj = (com::obj::TestObject*)instance;

	vector<double> *_retVal=new vector<double>;

	*_retVal = (_obj->getJ());
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForcom_obj_TestObjectsetJvectortsdoublete(void* instance,vals values)
{
	com::obj::TestObject *_obj = (com::obj::TestObject*)instance;
		vector<double> *_0 = (vector<double>*)values.at(0);
	void* returnValue=NULL;
	_obj->setJ(*_0);
	return returnValue;
}
void* defaultinvokeReflectionCIMethodForcom_obj_TestObjectgetK(void* instance,vals values)
{
	com::obj::TestObject *_obj = (com::obj::TestObject*)instance;

	vector<bool> *_retVal=new vector<bool>;

	*_retVal = (_obj->getK());
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForcom_obj_TestObjectsetKvectortsboolte(void* instance,vals values)
{
	com::obj::TestObject *_obj = (com::obj::TestObject*)instance;
		vector<bool> *_0 = (vector<bool>*)values.at(0);
	void* returnValue=NULL;
	_obj->setK(*_0);
	return returnValue;
}
void* defaultinvokeReflectionCIMethodForcom_obj_TestObjectgetL(void* instance,vals values)
{
	com::obj::TestObject *_obj = (com::obj::TestObject*)instance;

	vector<string> *_retVal=new vector<string>;

	*_retVal = (_obj->getL());
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForcom_obj_TestObjectsetLvectortsstringte(void* instance,vals values)
{
	com::obj::TestObject *_obj = (com::obj::TestObject*)instance;
		vector<string> *_0 = (vector<string>*)values.at(0);
	void* returnValue=NULL;
	_obj->setL(*_0);
	return returnValue;
}
void* defaultinvokeReflectionCIMethodForcom_obj_TestObjecttoString(void* instance,vals values)
{
	com::obj::TestObject *_obj = (com::obj::TestObject*)instance;

	string *_retVal=new string;

	*_retVal = (_obj->toString());
	return _retVal;
}
TestObject defaultinvokeReflectionCIFieldFortest_all_in_one_TestAllInOneto(void* instance)
{
	test::all::in::one::TestAllInOne *_obj = (test::all::in::one::TestAllInOne*)instance;
	return _obj->to;
}

com::obj::TestObject defaultinvokeReflectionCIFieldFortest_all_in_one_TestAllInOnecoto(void* instance)
{
	test::all::in::one::TestAllInOne *_obj = (test::all::in::one::TestAllInOne*)instance;
	return _obj->coto;
}

vector<TestObject> defaultinvokeReflectionCIFieldFortest_all_in_one_TestAllInOnevto(void* instance)
{
	test::all::in::one::TestAllInOne *_obj = (test::all::in::one::TestAllInOne*)instance;
	return _obj->vto;
}

vector<com::obj::TestObject> defaultinvokeReflectionCIFieldFortest_all_in_one_TestAllInOnevcoto(void* instance)
{
	test::all::in::one::TestAllInOne *_obj = (test::all::in::one::TestAllInOne*)instance;
	return _obj->vcoto;
}

vector<vector<TestObject> > defaultinvokeReflectionCIFieldFortest_all_in_one_TestAllInOnevvto(void* instance)
{
	test::all::in::one::TestAllInOne *_obj = (test::all::in::one::TestAllInOne*)instance;
	return _obj->vvto;
}

vector<vector<com::obj::TestObject> > defaultinvokeReflectionCIFieldFortest_all_in_one_TestAllInOnevvcoto(void* instance)
{
	test::all::in::one::TestAllInOne *_obj = (test::all::in::one::TestAllInOne*)instance;
	return _obj->vvcoto;
}

void* defaultinvokeReflectionCIMethodFortest_all_in_one_TestAllInOnetoString(void* instance,vals values)
{
	test::all::in::one::TestAllInOne *_obj = (test::all::in::one::TestAllInOne*)instance;

	string *_retVal=new string;

	*_retVal = (_obj->toString());
	return _retVal;
}
void* defaultinvokeReflectionCICtorFortest_all_in_one_TestAllInOne(vals values)
{
	test::all::in::one::TestAllInOne *_retVal = NULL;

	_retVal = (new test::all::in::one::TestAllInOne());
	return _retVal;
}
void defaultinvokeReflectionCIDtorFortest_all_in_one_TestAllInOne(void* instance)
{
	test::all::in::one::TestAllInOne *_obj = (test::all::in::one::TestAllInOne*)instance;
	_obj->~TestAllInOne();
}
void* defaultinvokeReflectionCICtorForws_test_TestingWS(vals values)
{
	ws::test::TestingWS *_retVal = NULL;

	_retVal = (new ws::test::TestingWS());
	return _retVal;
}
void defaultinvokeReflectionCIDtorForws_test_TestingWS(void* instance)
{
	ws::test::TestingWS *_obj = (ws::test::TestingWS*)instance;
	_obj->~TestingWS();
}
void* defaultinvokeReflectionCIMethodForws_test_TestingWSwsmeth1intstringlong(void* instance,vals values)
{
	ws::test::TestingWS *_obj = (ws::test::TestingWS*)instance;
		int *_0 = (int*)values.at(0);		string *_1 = (string*)values.at(1);		long *_2 = (long*)values.at(2);
	void* returnValue=NULL;
	_obj->wsmeth1(*_0,*_1,*_2);
	return returnValue;
}
void* defaultinvokeReflectionCIMethodForws_test_TestingWSwsmeth2stringvectortsintte(void* instance,vals values)
{
	ws::test::TestingWS *_obj = (ws::test::TestingWS*)instance;

	string *_retVal=new string;
		string *_0 = (string*)values.at(0);		vector<int> *_1 = (vector<int>*)values.at(1);
	*_retVal = (_obj->wsmeth2(*_0,*_1));
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForws_test_TestingWSwsmeth3string(void* instance,vals values)
{
	ws::test::TestingWS *_obj = (ws::test::TestingWS*)instance;

	TestObject *_retVal=new TestObject;
		string *_0 = (string*)values.at(0);
	*_retVal = (_obj->wsmeth3(*_0));
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForws_test_TestingWSwsmeth4bool(void* instance,vals values)
{
	ws::test::TestingWS *_obj = (ws::test::TestingWS*)instance;

	com::obj::TestObject *_retVal=new com::obj::TestObject;
		bool *_0 = (bool*)values.at(0);
	*_retVal = (_obj->wsmeth4(*_0));
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForws_test_TestingWSwsmeth5TestObject(void* instance,vals values)
{
	ws::test::TestingWS *_obj = (ws::test::TestingWS*)instance;

	string *_retVal=new string;
		TestObject *_0 = (TestObject*)values.at(0);
	*_retVal = (_obj->wsmeth5(*_0));
	return _retVal;
}
void* defaultinvokeReflectionCIMethodForws_test_TestingWSwsmeth6com_obj_TestObject(void* instance,vals values)
{
	ws::test::TestingWS *_obj = (ws::test::TestingWS*)instance;

	long *_retVal=new long;
		com::obj::TestObject *_0 = (com::obj::TestObject*)values.at(0);
	*_retVal = (_obj->wsmeth6(*_0));
	return _retVal;
}
void* flexAppinvokeReflectionCICtorForFlexAppController(vals values)
{
	FlexAppController *_retVal = NULL;

	_retVal = (new FlexAppController());
	return _retVal;
}
void flexAppinvokeReflectionCIDtorForFlexAppController(void* instance)
{
	FlexAppController *_obj = (FlexAppController*)instance;
	_obj->~FlexAppController();
}
void* flexAppinvokeReflectionCIMethodForFlexAppControllerserviceHttpRequest(void* instance,vals values)
{
	FlexAppController *_obj = (FlexAppController*)instance;

	HttpResponse *_retVal=new HttpResponse;
		HttpRequest *_0 = (HttpRequest*)values.at(0);
	*_retVal = (_obj->service(*_0));
	return _retVal;
}
void* oauthAppinvokeReflectionCICtorForOAUTH2Controller(vals values)
{
	OAUTH2Controller *_retVal = NULL;

	_retVal = (new OAUTH2Controller());
	return _retVal;
}
void oauthAppinvokeReflectionCIDtorForOAUTH2Controller(void* instance)
{
	OAUTH2Controller *_obj = (OAUTH2Controller*)instance;
	_obj->~OAUTH2Controller();
}
void* oauthAppinvokeReflectionCIMethodForOAUTH2ControllerserviceHttpRequest(void* instance,vals values)
{
	OAUTH2Controller *_obj = (OAUTH2Controller*)instance;

	HttpResponse *_retVal=new HttpResponse;
		HttpRequest *_0 = (HttpRequest*)values.at(0);
	*_retVal = (_obj->service(*_0));
	return _retVal;
}
void* oauthAppinvokeReflectionCICtorForOAUTHController(vals values)
{
	OAUTHController *_retVal = NULL;

	_retVal = (new OAUTHController());
	return _retVal;
}
void oauthAppinvokeReflectionCIDtorForOAUTHController(void* instance)
{
	OAUTHController *_obj = (OAUTHController*)instance;
	_obj->~OAUTHController();
}
void* oauthAppinvokeReflectionCIMethodForOAUTHControllerserviceHttpRequest(void* instance,vals values)
{
	OAUTHController *_obj = (OAUTHController*)instance;

	HttpResponse *_retVal=new HttpResponse;
		HttpRequest *_0 = (HttpRequest*)values.at(0);
	*_retVal = (_obj->service(*_0));
	return _retVal;
}
void* defaultoperatorTestLT(void* instance,vals values)
{
	Test *_obj = (Test*)instance;

	bool *_retVal=new bool;
		Test *_0 = (Test*)values.at(0);
	*_retVal = (*_obj<*_0);
	return _retVal;
}
void* defaultoperatorTestSTLsLT(void* instance,vals values)
{
	TestSTLs *_obj = (TestSTLs*)instance;

	bool *_retVal=new bool;
		TestSTLs *_0 = (TestSTLs*)values.at(0);
	*_retVal = (*_obj<*_0);
	return _retVal;
}
}
