/*
 * QueryBuilder.cpp
 *
 *  Created on: 11-May-2014
 *      Author: sumeetc
 */

#include "QueryBuilder.h"

QueryClause QueryClause::NONE,
	QueryClause::LOGICAL_GRP_OPEN,
	QueryClause::LOGICAL_GRP_CLOSE,
	QueryClause::AND,
	QueryClause::OR;

QueryOperator QueryOperator::NONE,
	QueryOperator::EQUALS,
	QueryOperator::NOT_EQUALS,
	QueryOperator::IN,
	QueryOperator::NOT_IN,
	QueryOperator::BETWEEN,
	QueryOperator::GREATER_THAN,
	QueryOperator::LESS_THAN,
	QueryOperator::GREATER_THAN_EQUALS,
	QueryOperator::LESS_THAN_EQUALS;

JoinType JoinType::INNER,
	JoinType::LEFT,
	JoinType::RIGHT,
	JoinType::FULL,
	JoinType::NATURAL,
	JoinType::CROSS,
	JoinType::LEFT_OUTER,
	JoinType::RIGHT_OUTER,
	JoinType::FULL_OUTER;

QueryBuilder::QueryBuilder() {
	unique = false;
	allCols =  false;
	start = 0;
	count = 0;
	group = GroupClause(this);
	conditions = LogicalGroup(this);
}

QueryBuilder::~QueryBuilder() {
	// TODO Auto-generated destructor stub
}

GroupClause::GroupClause(QueryBuilder* qb) {
	this->qb = qb;
}

GroupClause::~GroupClause() {
}

LogicalGroup& GroupClause::having() {
	return conditions;
}

GroupClause::GroupClause() {
	qb = NULL;
}

const std::vector<std::string>& GroupClause::getColumns() const {
	return columns;
}

const LogicalGroup& GroupClause::getConditions() const {
	return conditions;
}

QueryBuilder& GroupClause::end() {
	return *qb;
}

JoinClause::JoinClause(QueryBuilder* qb) {
	this->qb = qb;
	type = JoinType::CROSS;
}

JoinClause::~JoinClause() {
}

LogicalGroup& JoinClause::on(const std::string& lhs, const QueryOperator& oper, const GenericObject& rhs) {
	return condition;
}

JoinClause::JoinClause() {
	qb = NULL;
	type = JoinType::CROSS;
}

const std::string& JoinClause::getAlias() const {
	return alias;
}

const std::string& JoinClause::getClassName() const {
	return className;
}

const LogicalGroup& JoinClause::getCondition() const {
	return condition;
}

const std::string& JoinClause::getTableName() const {
	return tableName;
}

JoinType JoinClause::getType() const {
	return type;
}

QueryBuilder& JoinClause::end() {
	return *qb;
}

LogicalGroup::LogicalGroup(QueryBuilder* qb) {
	this->qb = qb;
	lastClause = QueryClause::NONE;
	condStarted = false;
}

LogicalGroup::~LogicalGroup() {
}

LogicalGroup& LogicalGroup::where(const std::string& lhs, const QueryOperator& oper, const GenericObject& rhs) {
	Condition cond;
	cond.lhs = lhs;
	cond.oper = oper;
	cond.rhsVec.push_back(rhs);
	cond.clause = QueryClause::NONE;
	if(condStarted && (lastClause!=QueryClause::AND && lastClause!=QueryClause::OR)) {
		logicalAnd();
	}
	condStarted = true;
	conds.push_back(cond);
	return *this;
}

LogicalGroup& LogicalGroup::logicalAnd() {
	Condition cond;
	cond.oper = QueryOperator::NONE;
	cond.clause = QueryClause::AND;
	conds.push_back(cond);
	lastClause = QueryClause::AND;
	return *this;
}

LogicalGroup& LogicalGroup::logicalOr() {
	Condition cond;
	cond.oper = QueryOperator::NONE;
	cond.clause = QueryClause::OR;
	conds.push_back(cond);
	lastClause = QueryClause::OR;
	return *this;
}

LogicalGroup& LogicalGroup::open() {
	Condition cond;
	cond.oper = QueryOperator::NONE;
	cond.clause = QueryClause::LOGICAL_GRP_OPEN;
	conds.push_back(cond);
	return *this;
}

LogicalGroup& LogicalGroup::close() {
	Condition cond;
	cond.oper = QueryOperator::NONE;
	cond.clause = QueryClause::LOGICAL_GRP_CLOSE;
	conds.push_back(cond);
	return *this;
}

LogicalGroup::LogicalGroup() {
	qb = NULL;
	lastClause = QueryClause::NONE;
	condStarted = false;
}

LogicalGroup& LogicalGroup::where(const std::string& lhs, const QueryOperator& oper, const GenericObject& rhs, const GenericObject& rhs1) {
	Condition cond;
	cond.lhs = lhs;
	cond.oper = oper;
	cond.rhsVec.push_back(rhs);
	cond.rhsVec.push_back(rhs1);
	cond.clause = QueryClause::NONE;
	if(condStarted && (lastClause!=QueryClause::AND && lastClause!=QueryClause::OR)) {
		logicalAnd();
	}
	condStarted = true;
	conds.push_back(cond);
	return *this;
}

LogicalGroup& LogicalGroup::where(const std::string& lhs, const QueryOperator& oper, const std::vector<GenericObject>& rhsList) {
	Condition cond;
	cond.lhs = lhs;
	cond.oper = oper;
	cond.rhsVec = rhsList;
	cond.clause = QueryClause::NONE;
	if(condStarted && (lastClause!=QueryClause::AND && lastClause!=QueryClause::OR)) {
		logicalAnd();
	}
	condStarted = true;
	conds.push_back(cond);
	return *this;
}

const std::vector<Condition>& LogicalGroup::getConds() const {
	return conds;
}

QueryBuilder& LogicalGroup::end() {
	return *qb;
}

QueryBuilder& QueryBuilder::allColumns() {
	allCols = true;
	return *this;
}

QueryBuilder& QueryBuilder::columnName(std::string name, std::string calias) {
	StringUtil::trim(name);
	StringUtil::trim(calias);
	if(className=="")
		columns[alias + "." + name] = calias;
	else
		//Ignore alias if classname is provided, these are properties and not column names
		columns[name] = "";
	return *this;
}

QueryBuilder& QueryBuilder::columnNames(const std::map<std::string, std::string>& columns) {
	this->columns = columns;
	return *this;
}

QueryBuilder& QueryBuilder::delimitedColumnNames(const std::string& names) {
	std::vector<std::string> cols = StringUtil::splitAndReturn<std::vector<std::string> >(names, ",");
	for (int var = 0; var < (int)cols.size(); ++var) {
		std::string col = cols.at(var);
		StringUtil::trim(col);
		if(className!="") {
			columns[col] = "";
		}
		else if(StringUtil::toLowerCopy(col).find(" as ")!=std::string::npos) {
			std::vector<std::string> colparts = StringUtil::splitAndReturn<std::vector<std::string> >(col, " as ");
			std::string coln = StringUtil::trimCopy(colparts.at(0));
			std::string colal = StringUtil::trimCopy(colparts.at(1));
			if(coln.find(".")==std::string::npos) {
				coln = (alias + "." + coln);
			}
			columns[coln] = colal;
		} else if(StringUtil::toLowerCopy(col).find(" ")!=std::string::npos) {
			std::vector<std::string> colparts = StringUtil::splitAndReturn<std::vector<std::string> >(col, " ");
			std::string coln = StringUtil::trimCopy(colparts.at(0));
			std::string colal = StringUtil::trimCopy(colparts.at(1));
			if(coln.find(".")==std::string::npos) {
				coln = (alias + "." + coln);
			}
			columns[coln] = colal;
		}
	}
	return *this;
}

QueryBuilder& QueryBuilder::distinct() {
	unique = true;
	return *this;
}

QueryBuilder& QueryBuilder::fromTable(const std::string& name, const std::string& alias) {
	tableName = name;
	this->alias = alias;
	return *this;
}

QueryBuilder& QueryBuilder::fromClass(const std::string& name, const std::string& alias) {
	className = name;
	this->alias = alias;
	return *this;
}

LogicalGroup& QueryBuilder::condition() {
	return conditions;
}

GroupClause& QueryBuilder::groupBy(const std::string& column) {
	group.columns.push_back(column);
	return group;
}

GroupClause& QueryBuilder::groupBy(const std::vector<std::string>& columns) {
	group.columns = columns;
	return group;
}

GroupClause& QueryBuilder::delimitedGroupByColumns(const std::string& names) {
	std::vector<std::string> cols = StringUtil::splitAndReturn<std::vector<std::string> >(names, ",");
	for (int var = 0; var < (int)cols.size(); ++var) {
		std::string col = cols.at(var);
		StringUtil::trim(col);
		group.columns.push_back(col);
	}
	return group;
}

QueryBuilder& QueryBuilder::orderByAsc(const std::string& column) {
	columnsAsc.push_back(column);
	return *this;
}

QueryBuilder& QueryBuilder::orderByDesc(const std::string& column) {
	columnsAsc.push_back(column);
	return *this;
}

QueryBuilder& QueryBuilder::paginate(const int& start, const int& count) {
	this->start = start;
	this->count = count;
	return *this;
}

JoinClause& QueryBuilder::joinTable(const JoinType& jt, const std::string& name, const std::string& alias) {
	JoinClause jc(this);
	jc.tableName = name;
	jc.alias = alias;
	jc.type = jt;
	joinClauses.push_back(jc);
	return joinClauses.back();
}

JoinClause& QueryBuilder::joinClass(const JoinType& jt, const std::string& name, const std::string& alias) {
	JoinClause jc(this);
	jc.alias = alias;
	jc.type = jt;
	jc.className = name;
	joinClauses.push_back(jc);
	return joinClauses.back();
}

QueryBuilder& QueryBuilder::unionQuery(const QueryBuilder& qb) {
	unions.push_back(qb);
	return *this;
}

const std::string& QueryBuilder::getAlias() const {
	return alias;
}

bool QueryBuilder::isAllCols() const {
	return allCols;
}

const std::string& QueryBuilder::getClassName() const {
	return className;
}

const std::map<std::string, std::string>& QueryBuilder::getColumns() const {
	return columns;
}

const std::vector<std::string>& QueryBuilder::getColumnsAsc() const {
	return columnsAsc;
}

const std::vector<std::string>& QueryBuilder::getColumnsDesc() const {
	return columnsDesc;
}

const LogicalGroup& QueryBuilder::getConditions() const {
	return conditions;
}

int QueryBuilder::getCount() const {
	return count;
}

const GroupClause& QueryBuilder::getGroup() const {
	return group;
}

const std::vector<JoinClause>& QueryBuilder::getJoinClauses() const {
	return joinClauses;
}

int QueryBuilder::getStart() const {
	return start;
}

const std::string& QueryBuilder::getTableName() const {
	return tableName;
}

const std::vector<QueryBuilder>& QueryBuilder::getUnionAlls() const {
	return unionAlls;
}

const std::vector<QueryBuilder>& QueryBuilder::getUnions() const {
	return unions;
}

bool QueryBuilder::isUnique() const {
	return unique;
}

QueryBuilder& QueryBuilder::unionAllQuery(const QueryBuilder& qb) {
	unionAlls.push_back(qb);
	return *this;
}

QueryClause Condition::getClause() const {
	return clause;
}

const std::string& Condition::getLhs() const {
	return lhs;
}

QueryOperator Condition::getOper() const {
	return oper;
}

Condition::Condition() {
	oper = QueryOperator::NONE;
	clause = QueryClause::NONE;
	managed = false;
}

Condition::~Condition() {
}

GenericObject& Condition::getRhs() {
	return rhsVec.at(0);
}

GenericObject& Condition::getRhs(const int& index) {
	return rhsVec.at(index);
}

bool Condition::isManaged() const {
	return managed;
}

int Condition::getRhsSize() {
	return rhsVec.size();
}
