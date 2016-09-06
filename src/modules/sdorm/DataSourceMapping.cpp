/*
 * DataSourceMapping.cpp
 *
 *  Created on: 18-May-2014
 *      Author: sumeetc
 */

#include "DataSourceMapping.h"

const string& DataSourceInternalRelation::getClsName() const {
	return clsName;
}

void DataSourceInternalRelation::setClsName(const string& clsName) {
	this->clsName = clsName;
}

const string& DataSourceInternalRelation::getField() const {
	return field;
}

void DataSourceInternalRelation::setField(const string& field) {
	this->field = field;
}

const string& DataSourceInternalRelation::getDfk() const {
	return dfk;
}

void DataSourceInternalRelation::setDfk(const string& dfk) {
	this->dfk = dfk;
}

const string& DataSourceInternalRelation::getDmappedBy() const {
	return dmappedBy;
}

void DataSourceInternalRelation::setDmappedBy(const string& dmappedBy) {
	this->dmappedBy = dmappedBy;
}

int DataSourceInternalRelation::getType() const {
	return type;
}

void DataSourceInternalRelation::setType(const int& type) {
	this->type = type;
}


string Mapping::getClassForTable(const string& table) {
	if(tableClassMapping.find(table)!=tableClassMapping.end())
	{
		return tableClassMapping[table];
	}
	return "";
}

string Mapping::getTableForClass(const string& clas) {
	if(dseMap.find(clas)!=dseMap.end())
	{
		return dseMap[clas].tableName;
	}
	return "";
}


const string& Mapping::getAppName() const {
	return appName;
}

void Mapping::addDataSourceEntityMapping(const DataSourceEntityMapping& dsemp) {
	if(dsemp.getClassName()!="" && dseMap.find(dsemp.getClassName())==dseMap.end()) {
		dseMap[dsemp.getClassName()] = dsemp;
		if(!dsemp.isEmbedded()) {
			tableClassMapping[dsemp.getTableName()] = dsemp.getClassName();
		}
	}
}

string Mapping::getPropertyForColumn(const string& tableName, const string& columnName) {
	if(tableClassMapping.find(tableName)!=tableClassMapping.end())
	{
		string cn = StringUtil::toLowerCopy(columnName);
		DataSourceEntityMapping dsemp = dseMap[tableClassMapping[tableName]];
		if(dsemp.columnPropertyMapping.find(cn)!=dsemp.columnPropertyMapping.end())
		{
			return dsemp.columnPropertyMapping[cn];
		}
	}
	return "";
}

strMap Mapping::getMappingForTable(const string& tableName) {
	if(tableClassMapping.find(tableName)!=tableClassMapping.end())
	{
		DataSourceEntityMapping dsemp = dseMap[tableClassMapping[tableName]];
		return dsemp.columnPropertyMapping;
	}
	strMap s;
	return s;
}

map<string, DataSourceEntityMapping>& Mapping::getDseMap() {
	return dseMap;
}

void Mapping::setAppName(const string& appName) {
	this->appName = appName;
}

const string& DataSourceEntityMapping::getClassName() const {
	return className;
}

void DataSourceEntityMapping::setClassName(const string& className) {
	this->className = className;
}

const string& DataSourceEntityMapping::getIdPropertyName() const {
	return idPropertyName;
}

void DataSourceEntityMapping::setIdPropertyName(const string& idPropertyName) {
	this->idPropertyName = idPropertyName;
}

bool DataSourceEntityMapping::isEmbedded() const {
	return embedded;
}

void DataSourceEntityMapping::setIsEmbedded(const bool& embedded) {
	this->embedded = embedded;
}

const string& DataSourceEntityMapping::getTableName() const {
	return tableName;
}

DataSourceEntityMapping::DataSourceEntityMapping() {
	embedded = false;
	idGenerate = false;
	idgenlowValue = 32767;
}

DataSourceEntityMapping::~DataSourceEntityMapping() {
}

void DataSourceEntityMapping::setTableName(const string& tableName) {
	this->tableName = tableName;
}

void DataSourceEntityMapping::addPropertyColumnMapping(const string& property, const string& column) {
	if(property!="" && column!="")
	{
		string cn = StringUtil::toLowerCopy(column);
		propertyColumnMapping[property] = cn;
		columnPropertyMapping[cn] = property;
	}
}

void DataSourceEntityMapping::addRelation(const DataSourceInternalRelation& relation) {
	relations.push_back(relation);
}

string DataSourceEntityMapping::getColumnForProperty(const string& property) {
	if(propertyColumnMapping.find(property)!=propertyColumnMapping.end())
	{
		return propertyColumnMapping[property];
	}
	return "";
}

const strMap& DataSourceEntityMapping::getColumnPropertyMapping() const {
	return columnPropertyMapping;
}

string DataSourceEntityMapping::getPropertyForColumn(const string& column) {
	string cn = StringUtil::toLowerCopy(column);
	if(columnPropertyMapping.find(cn)!=columnPropertyMapping.end())
	{
		return columnPropertyMapping[cn];
	}
	return "";
}

const strMap& DataSourceEntityMapping::getPropertyColumnMapping() const {
	return propertyColumnMapping;
}

vector<DataSourceInternalRelation> DataSourceEntityMapping::getRelations() {
	return relations;
}

DataSourceEntityMapping Mapping::getDataSourceEntityMapping(const string& clas) {
	if(dseMap.find(clas)!=dseMap.end())
	{
		return dseMap[clas];
	}
	return __c;
}

const string& DataSourceEntityMapping::getIdgencolumnName() const {
	return idgencolumnName;
}

const string& DataSourceEntityMapping::getIdgendbEntityName() const {
	return idgendbEntityName;
}

const string& DataSourceEntityMapping::getIdgendbEntityType() const {
	return idgendbEntityType;
}

const string& DataSourceEntityMapping::getIdgenentityColumn() const {
	return idgenentityColumn;
}

const string& DataSourceEntityMapping::getIdgenhiValueColumn() const {
	return idgenhiValueColumn;
}

const string& DataSourceEntityMapping::getIdgentype() const {
	return idgentype;
}

bool DataSourceEntityMapping::isIdGenerate() const {
	return idGenerate;
}

int DataSourceEntityMapping::getIdgenlowValue() const {
	return idgenlowValue;
}

void DataSourceEntityMapping::setEmbedded(const bool& embedded) {
	this->embedded = embedded;
}

void DataSourceEntityMapping::setIdgencolumnName(
		const string& idgencolumnName) {
	this->idgencolumnName = idgencolumnName;
}

void DataSourceEntityMapping::setIdgendbEntityName(
		const string& idgendbEntityName) {
	this->idgendbEntityName = idgendbEntityName;
}

void DataSourceEntityMapping::setIdgendbEntityType(
		const string& idgendbEntityType) {
	this->idgendbEntityType = idgendbEntityType;
}

void DataSourceEntityMapping::setIdgenentityColumn(
		const string& idgenentityColumn) {
	this->idgenentityColumn = idgenentityColumn;
}

void DataSourceEntityMapping::setIdGenerate(const bool& idGenerate) {
	this->idGenerate = idGenerate;
}

void DataSourceEntityMapping::setIdgenhiValueColumn(
		const string& idgenhiValueColumn) {
	this->idgenhiValueColumn = idgenhiValueColumn;
}

void DataSourceEntityMapping::setIdgentype(const string& idgentype) {
	this->idgentype = idgentype;
}

void DataSourceEntityMapping::setIdgenlowValue(const int& idgenlowValue) {
	this->idgenlowValue = idgenlowValue;
}
