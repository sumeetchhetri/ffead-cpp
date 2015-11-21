
#ifndef @TCLASSU@_H_
#define @TCLASSU@_H_
#include "Controller.h"

#pragma @Controller path="*"
class @TCLASS@ {
public:
	@TCLASS@();
	virtual ~@TCLASS@();
	bool service(HttpRequest* req, HttpResponse* res);
};

#endif /* @TCLASSU@_H_ */