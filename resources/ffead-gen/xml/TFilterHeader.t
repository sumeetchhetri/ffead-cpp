
#ifndef @TCLASSU@_H_
#define @TCLASSU@_H_
#include "Controller.h"

class @TCLASS@ {
public:
	@TCLASS@();
	virtual ~@TCLASS@();
	void doInputFilter(HttpRequest *req);
	void doOutputFilter(HttpResponse *res);
	bool doHandle(HttpRequest *req, HttpResponse* res);
};

#endif /* @TCLASSU@_H_ */