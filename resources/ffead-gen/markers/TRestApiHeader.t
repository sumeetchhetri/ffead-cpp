
#ifndef @TCLASSU@_H_
#define @TCLASSU@_H_

#pragma @RestController path="/@TCLASS@"
class @TCLASS@ {
public:
	@TCLASS@();
	virtual ~@TCLASS@();
	#pragma @GET path="/serve" statusCode="200"
	int serve();
};

#endif /* @TCLASSU@_H_ */