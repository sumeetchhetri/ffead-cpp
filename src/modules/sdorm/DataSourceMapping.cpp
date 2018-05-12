/*
 * DataSourceMapping.cpp
 *
 *  Created on: 18-May-2014
 *      Author: sumeetc
 */

#include "DataSourceMapping.h"

const std::string& DataSourceInternalRelation::getClsName() const {
	return clsName;
}

void DataSourceInternalRelation::setClsName(const std::string& clsName) {
	this->clsName = clsName;
}

const std::string& DataSourceInternalRelation::getField() const {
	return field;
}

void DataSourceInternalRelation::setField(const std::string& field) {
	this->field = field;
}

const std::string& DataSourceInternalRelation::getDfk() const {
	return dfk;
}

void DataSourceInternalRelation::setDfk(const std::string& dfk) {
	this->dfk = dfk;
}

const std::string& DataSourceInternalRelation::getDmappedBy() const {
	return dmappedBy;
}

void DataSourceInternalRelation::setDmappedBy(const std::string& dmappedBy) {
	this->dmappedBy = dmappedBy;
}

int DataSourceInternalRelation::getType() const {
	return type;
}

void DataSourceInternalRelation::setType(const int& type) {
	this->type = type;
}


std::string Mapping::getClassForTable(const std::string& table) {
	if(tableClassMapping.find(table)!=tableClassMapping.end())
	{
		return tableClassMapping[table];
	}
	return "";
}

std::string Mapping::getTableForClass(const std::string& clas) {
	if(dseMap.find(clas)!=dseMap.end())
	{
		return dseMap[clas].tableName;
	}
	return "";
}


const std::string& Mapping::getAppName() const {
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

std::string Mapping::getPropertyForColumn(const std::string& tableName, const std::string& columnName) {
	if(tableClassMapping.find(tableName)!=tableClassMapping.end())
	{
		DataSourceEntityMapping dsemp = dseMap[tableClassMapping[tableName]];
		if(dsemp.columnPropertyMappingCS.find(columnName)!=dsemp.columnPropertyMappingCS.end())
		{
			return dsemp.columnPropertyMappingCS[columnName];
		}
		std::string cn = StringUtil::toLowerCopy(columnName);
		if(dsemp.columnPropertyMapping.find(cn)!=dsemp.columnPropertyMapping.end())
		{
			return dsemp.columnPropertyMapping[cn];
		}
	}
	return "";
}

strMap& Mapping::getMappingForTable(const std::string& tableName) {
	if(tableClassMapping.find(tableName)!=tableClassMapping.end())
	{
		DataSourceEntityMapping& dsemp = dseMap[tableClassMapping[tableName]];
		return dsemp.columnPropertyMapping;
	}
	return __s;
}

std::map<std::string, DataSourceEntityMapping>& Mapping::getDseMap() {
	return dseMap;
}

void Mapping::setAppName(const std::string& appName) {
	this->appName = appName;
}

const std::string& DataSourceEntityMapping::getClassName() const {
	return className;
}

void DataSourceEntityMapping::setClassName(const std::string& className) {
	this->className = className;
}

const std::string& DataSourceEntityMapping::getIdPropertyName() const {
	return idPropertyName;
}

void DataSourceEntityMapping::setIdPropertyName(const std::string& idPropertyName) {
	this->idPropertyName = idPropertyName;
}

bool DataSourceEntityMapping::isEmbedded() const {
	return embedded;
}

void DataSourceEntityMapping::setIsEmbedded(const bool& embedded) {
	this->embedded = embedded;
}

const std::string& DataSourceEntityMapping::getTableName() const {
	return tableName;
}

DataSourceEntityMapping::DataSourceEntityMapping() {
	embedded = false;
	idGenerate = false;
	idgenlowValue = 32767;
}

DataSourceEntityMapping::~DataSourceEntityMapping() {
}

void DataSourceEntityMapping::setTableName(const std::string& tableName) {
	this->tableName = tableName;
}

void DataSourceEntityMapping::addPropertyColumnMapping(const std::string& property, const std::string& column) {
	if(property!="" && column!="")
	{
		propertyColumnMappingCS[property] = column;
		columnPropertyMappingCS[column] = property;
		std::string cn = StringUtil::toLowerCopy(column);
		propertyColumnMapping[property] = cn;
		columnPropertyMapping[cn] = property;
	}
}

void DataSourceEntityMapping::addRelation(const DataSourceInternalRelation& relation) {
	relations.push_back(relation);
}

std::string DataSourceEntityMapping::getColumnForProperty(const std::string& property) {
	if(propertyColumnMapping.find(property)!=propertyColumnMapping.end())
	{
		return propertyColumnMapping[property];
	}
	return "";
}

const strMap& DataSourceEntityMapping::getColumnPropertyMapping() const {
	return columnPropertyMapping;
}

std::string DataSourceEntityMapping::getPropertyForColumn(const std::string& column) {
	if(columnPropertyMappingCS.find(column)!=columnPropertyMappingCS.end())
	{
		return columnPropertyMappingCS[column];
	}
	std::string cn = StringUtil::toLowerCopy(column);
	if(columnPropertyMapping.find(cn)!=columnPropertyMapping.end())
	{
		return columnPropertyMapping[cn];
	}
	return "";
}

const strMap& DataSourceEntityMapping::getPropertyColumnMapping() const {
	return propertyColumnMapping;
}

const strMap& DataSourceEntityMapping::getPropertyColumnMappingCS() const {
	return propertyColumnMappingCS;
}

std::vector<DataSourceInternalRelation> DataSourceEntityMapping::getRelations() {
	return relations;
}

DataSourceEntityMapping& Mapping::getDataSourceEntityMapping(const std::string& clas) {
	if(dseMap.find(clas)!=dseMap.end())
	{
		return dseMap[clas];
	}
	return __c;
}

const std::string& DataSourceEntityMapping::getIdgencolumnName() const {
	return idgencolumnName;
}

const std::string& DataSourceEntityMapping::getIdgendbEntityName() const {
	return idgendbEntityName;
}

const std::string& DataSourceEntityMapping::getIdgendbEntityType() const {
	return idgendbEntityType;
}

const std::string& DataSourceEntityMapping::getIdgenentityColumn() const {
	return idgenentityColumn;
}

const std::string& DataSourceEntityMapping::getIdgenhiValueColumn() const {
	return idgenhiValueColumn;
}

const std::string& DataSourceEntityMapping::getIdgentype() const {
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
		const std::string& idgencolumnName) {
	this->idgencolumnName = idgencolumnName;
}

void DataSourceEntityMapping::setIdgendbEntityName(
		const std::string& idgendbEntityName) {
	this->idgendbEntityName = idgendbEntityName;
}

void DataSourceEntityMapping::setIdgendbEntityType(
		const std::string& idgendbEntityType) {
	this->idgendbEntityType = idgendbEntityType;
}

void DataSourceEntityMapping::setIdgenentityColumn(
		const std::string& idgenentityColumn) {
	this->idgenentityColumn = idgenentityColumn;
}

void DataSourceEntityMapping::setIdGenerate(const bool& idGenerate) {
	this->idGenerate = idGenerate;
}

void DataSourceEntityMapping::setIdgenhiValueColumn(
		const std::string& idgenhiValueColumn) {
	this->idgenhiValueColumn = idgenhiValueColumn;
}

void DataSourceEntityMapping::setIdgentype(const std::string& idgentype) {
	this->idgentype = idgentype;
}

void DataSourceEntityMapping::setIdgenlowValue(const int& idgenlowValue) {
	this->idgenlowValue = idgenlowValue;
}
