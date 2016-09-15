/*
 * MarkerHandler.h
 *
 *  Created on: 20-Aug-2014
 *      Author: sumeetc
 */

#ifndef MARKERHANDLER_H_
#define MARKERHANDLER_H_
#include "string"
#include "vector"
#include "map"
#include "StringUtil.h"
#include <stdarg.h>
#include <iostream>


class MarkerHandler;

class Marker {
public:
	const std::map<std::string, bool>& getAttributes() const;
	std::string getAttributeValue(const std::string&);
	const std::string& getName() const;
	const bool isTypeClass() const;
	const bool isTypeProp() const;
	const bool isTypeMeth() const;
	const bool isTypeArg() const;
	static std::string getTypeName(const int&);
	std::string getTypeName();
	Marker();
private:
	int reqAttrSize;
	enum {TYPE_CLASS, TYPE_PROP, TYPE_METH, TYPE_ARG};
	std::string name;
	std::map<std::string, bool> attributes;
	std::map<std::string, std::vector<std::string> > valueSet;
	std::map<std::string, std::string> defValues;
	std::map<std::string, std::string> attributeValues;
	int type;
	Marker(const std::string& name, const int& type);
	Marker(const std::string& name, const int& type, const std::vector<std::string>& attributes);
	Marker(const std::string& name, const int& type, const std::vector<std::string>& attributes, const std::vector<bool>& reqLst);
	friend class MarkerHandler;
	friend class ConfigurationHandler;
};

class MarkerHandler {
	std::vector<Marker> validMarkers;
	void initMarkers();
	Marker getMarker(const std::string& name, const int& where);
	Marker getMarker(const std::string& name);
public:
	static std::vector<std::string> collectStr(int num, ...);
	static std::vector<bool> collectBool(int num, ...);
	void addMarker(const Marker& m);
	MarkerHandler();
	virtual ~MarkerHandler();
	Marker processMarker(std::string markerText, const int& where);
};

#endif /* MARKERHANDLER_H_ */
