/*
 * Tempo.h
 *
 *  Created on: Jan 22, 2010
 *      Author: sumeet
 */

#ifndef TEMPO_H_
#define TEMPO_H_
#include "iostream"
using namespace std;
class Tempo {
	void prin();
	int p;
	string h;
public:
	Tempo();
	virtual ~Tempo();
	void prin1();
	template<class T> void func()
	{
		cout << p << flush;
	}
};

#endif /* TEMPO_H_ */
