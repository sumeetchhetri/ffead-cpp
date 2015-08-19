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

const vector<string>& GroupClause::getColumns() const {
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

LogicalGroup& JoinClause::on(const string& lhs, const QueryOperator& oper, const GenericObject& rhs) {
	return condition;
}

JoinClause::JoinClause() {
	qb = NULL;
	type = JoinType::CROSS;
}

const string& JoinClause::getAlias() const {
	return alias;
}

const string& JoinClause::getClassName() const {
	return className;
}

const LogicalGroup& JoinClause::getCondition() const {
	return condition;
}

const string& JoinClause::getTableName() const {
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

LogicalGroup& LogicalGroup::where(const string& lhs, const QueryOperator& oper, const GenericObject& rhs) {
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

LogicalGroup& LogicalGroup::where(const string& lhs, const QueryOperator& oper, const GenericObject& rhs, const GenericObject& rhs1) {
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

LogicalGroup& LogicalGroup::where(const string& lhs, const QueryOperator& oper, const vector<GenericObject>& rhsList) {
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

const vector<Condition>& LogicalGroup::getConds() const {
	return conds;
}

QueryBuilder& LogicalGroup::end() {
	return *qb;
}

QueryBuilder& QueryBuilder::allColumns() {
	allCols = true;
	return *this;
}

QueryBuilder& QueryBuilder::columnName(string name, string calias) {
	StringUtil::trim(name);
	StringUtil::trim(calias);
	if(className=="")
		columns[alias + "." + name] = calias;
	else
		//Ignore alias if classname is provided, these are properties and not column names
		columns[name] = "";
	return *this;
}

QueryBuilder& QueryBuilder::columnNames(const map<string, string>& columns) {
	this->columns = columns;
	return *this;
}

QueryBuilder& QueryBuilder::delimitedColumnNames(const string& names) {
	vector<string> cols = StringUtil::splitAndReturn<vector<string> >(names, ",");
	for (int var = 0; var < (int)cols.size(); ++var) {
		string col = cols.at(var);
		StringUtil::trim(col);
		if(className!="") {
			columns[col] = "";
		}
		else if(StringUtil::toLowerCopy(col).find(" as ")!=string::npos) {
			vector<string> colparts = StringUtil::splitAndReturn<vector<string> >(col, " as ");
			string coln = StringUtil::trimCopy(colparts.at(0));
			string colal = StringUtil::trimCopy(colparts.at(1));
			if(coln.find(".")==string::npos) {
				coln = (alias + "." + coln);
			}
			columns[coln] = colal;
		} else if(StringUtil::toLowerCopy(col).find(" ")!=string::npos) {
			vector<string> colparts = StringUtil::splitAndReturn<vector<string> >(col, " ");
			string coln = StringUtil::trimCopy(colparts.at(0));
			string colal = StringUtil::trimCopy(colparts.at(1));
			if(coln.find(".")==string::npos) {
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

QueryBuilder& QueryBuilder::fromTable(const string& name, const string& alias) {
	tableName = name;
	this->alias = alias;
	return *this;
}

QueryBuilder& QueryBuilder::fromClass(const string& name, const string& alias) {
	className = name;
	this->alias = alias;
	return *this;
}

LogicalGroup& QueryBuilder::condition() {
	return conditions;
}

GroupClause& QueryBuilder::groupBy(const string& column) {
	group.columns.push_back(column);
	return group;
}

GroupClause& QueryBuilder::groupBy(const vector<string>& columns) {
	group.columns = columns;
	return group;
}

GroupClause& QueryBuilder::delimitedGroupByColumns(const string& names) {
	vector<string> cols = StringUtil::splitAndReturn<vector<string> >(names, ",");
	for (int var = 0; var < (int)cols.size(); ++var) {
		string col = cols.at(var);
		StringUtil::trim(col);
		group.columns.push_back(col);
	}
	return group;
}

QueryBuilder& QueryBuilder::orderByAsc(const string& column) {
	columnsAsc.push_back(column);
	return *this;
}

QueryBuilder& QueryBuilder::orderByDesc(const string& column) {
	columnsAsc.push_back(column);
	return *this;
}

QueryBuilder& QueryBuilder::paginate(const int& start, const int& count) {
	this->start = start;
	this->count = count;
	return *this;
}

JoinClause& QueryBuilder::joinTable(const JoinType& jt, const string& name, const string& alias) {
	JoinClause* jc = new JoinClause(this);
	jc->tableName = name;
	jc->alias = alias;
	jc->type = jt;
	joinClauses.push_back(jc);
	return *jc;
}

JoinClause& QueryBuilder::joinClass(const JoinType& jt, const string& name, const string& alias) {
	JoinClause* jc = new JoinClause(this);
	jc->alias = alias;
	jc->type = jt;
	jc->className = name;
	joinClauses.push_back(jc);
	return *jc;
}

QueryBuilder& QueryBuilder::unionQuery(const QueryBuilder& qb) {
	unions.push_back(qb);
	return *this;
}

const string& QueryBuilder::getAlias() const {
	return alias;
}

bool QueryBuilder::isAllCols() const {
	return allCols;
}

const string& QueryBuilder::getClassName() const {
	return className;
}

const map<string, string>& QueryBuilder::getColumns() const {
	return columns;
}

const vector<string>& QueryBuilder::getColumnsAsc() const {
	return columnsAsc;
}

const vector<string>& QueryBuilder::getColumnsDesc() const {
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

const vector<JoinClause*>& QueryBuilder::getJoinClauses() const {
	return joinClauses;
}

int QueryBuilder::getStart() const {
	return start;
}

const string& QueryBuilder::getTableName() const {
	return tableName;
}

const vector<QueryBuilder>& QueryBuilder::getUnionAlls() const {
	return unionAlls;
}

const vector<QueryBuilder>& QueryBuilder::getUnions() const {
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

const string& Condition::getLhs() const {
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
