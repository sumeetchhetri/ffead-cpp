/*
 * QueryBuilder.h
 *
 *  Created on: 11-May-2014
 *      Author: sumeetc
 */

#ifndef QUERYBUILDER_H_
#define QUERYBUILDER_H_
#include "Query.h"

class QueryBuilder;

class QueryClause;

class QueryClause {
	int val;
	static void init()
	{
		if(NONE.val!=1)
		{
			NONE.val = 1;
			LOGICAL_GRP_OPEN.val = 2;
			LOGICAL_GRP_CLOSE.val = 3;
			AND.val = 4;
			OR.val = 5;
		}
	}
	QueryClause()
	{
		val = 0;
	}
	friend class Condition;
	friend class LogicalGroup;
	friend class QueryBuilder;
	friend class GroupClause;
	friend class JoinClause;
	friend class ConfigurationHandler;
public:
	int get() const
	{
		return val;
	}
	static QueryClause NONE,
		LOGICAL_GRP_OPEN,
		LOGICAL_GRP_CLOSE,
		AND,
		OR;
	bool operator==(const QueryClause& other) const
	{
		if(this->val == other.val)
		{
			return true;
		}
		return false;
	}
	bool operator!=(const QueryClause& other) const
	{
		if(this->val != other.val)
		{
			return true;
		}
		return false;
	}
};

class QueryOperator;

class QueryOperator {
	int val;
	static void init()
	{
		if(NONE.val!=1)
		{
			NONE.val = 1;
			EQUALS.val = 2;
			NOT_EQUALS.val = 3;
			IN.val = 4;
			NOT_IN.val = 5;
			BETWEEN.val = 6;
			GREATER_THAN.val = 7;
			LESS_THAN.val = 8;
			GREATER_THAN_EQUALS.val = 9;
			LESS_THAN_EQUALS.val = 10;
		}
	}
	QueryOperator()
	{
		val = 0;
	}
	friend class Condition;
	friend class LogicalGroup;
	friend class QueryBuilder;
	friend class GroupClause;
	friend class JoinClause;
	friend class ConfigurationHandler;
public:
	int get() const
	{
		return val;
	}
	static QueryOperator NONE,
		EQUALS,
		NOT_EQUALS,
		IN,
		NOT_IN,
		BETWEEN,
		GREATER_THAN,
		LESS_THAN,
		GREATER_THAN_EQUALS,
		LESS_THAN_EQUALS;
	bool operator==(const QueryOperator& other) const
	{
		if(this->val == other.val)
		{
			return true;
		}
		return false;
	}
	bool operator!=(const QueryOperator& other) const
	{
		if(this->val != other.val)
		{
			return true;
		}
		return false;
	}
};


class JoinType;

class JoinType {
	int val;
	static void init()
	{
		if(INNER.val!=1)
		{
			INNER.val = 1;
			LEFT.val = 2;
			RIGHT.val = 3;
			FULL.val = 4;
			NATURAL.val = 5;
			CROSS.val = 6;
			LEFT_OUTER.val = 7;
			RIGHT_OUTER.val = 8;
			FULL_OUTER.val = 9;
		}
	}
	JoinType()
	{
		val = 0;
	}
	friend class Condition;
	friend class LogicalGroup;
	friend class QueryBuilder;
	friend class GroupClause;
	friend class JoinClause;
	friend class ConfigurationHandler;
public:
	int get() const
	{
		return val;
	}
	static JoinType INNER,
		LEFT,
		RIGHT,
		FULL,
		NATURAL,
		CROSS,
		LEFT_OUTER,
		RIGHT_OUTER,
		FULL_OUTER;
	bool operator==(const JoinType& other) const
	{
		if(this->val == other.val)
		{
			return true;
		}
		return false;
	}
	bool operator!=(const JoinType& other) const
	{
		if(this->val != other.val)
		{
			return true;
		}
		return false;
	}
};

class Condition {
	std::string lhs;
	QueryOperator oper;
	std::vector<GenericObject> rhsVec;
	QueryClause clause;
	bool managed;
	friend class LogicalGroup;
public:
	Condition();
	virtual ~Condition();
	QueryClause getClause() const;
	const std::string& getLhs() const;
	QueryOperator getOper() const;
	GenericObject& getRhs();
	GenericObject& getRhs(const int& index);
	int getRhsSize();
	bool isManaged() const;
};

class LogicalGroup {
	QueryBuilder* qb;
	std::vector<Condition> conds;
	QueryClause lastClause;
	bool condStarted;
	friend class QueryBuilder;
	LogicalGroup(QueryBuilder* qb);
public:
	LogicalGroup();
	virtual ~LogicalGroup();
	LogicalGroup& where(const std::string& lhs, const QueryOperator& oper, const GenericObject& rhs);
	LogicalGroup& where(const std::string& lhs, const QueryOperator& oper, const GenericObject& rhs, const GenericObject& rhs1);
	LogicalGroup& where(const std::string& lhs, const QueryOperator& oper, const std::vector<GenericObject>& rhsList);
	template <class T> LogicalGroup& where(const std::string& lhs, const QueryOperator& oper, const T& rhs);
	template <class T> LogicalGroup& where(const std::string& lhs, const QueryOperator& oper, const T& rhs, const T& rhs1);
	template <class T> LogicalGroup& where(const std::string& lhs, const QueryOperator& oper, const std::vector<T>& rhsList);
	LogicalGroup& logicalAnd();
	LogicalGroup& logicalOr();
	LogicalGroup& open();
	LogicalGroup& close();
	QueryBuilder& end();
	const std::vector<Condition>& getConds() const;
};

class JoinClause {
	QueryBuilder* qb;
	std::string tableName;
	std::string className;
	std::string alias;
	JoinType type;
	LogicalGroup condition;
	friend class QueryBuilder;
	JoinClause(QueryBuilder* qb);
public:
	JoinClause();
	virtual ~JoinClause();
	LogicalGroup& on(const std::string& lhs, const QueryOperator& oper, const GenericObject& rhs);
	QueryBuilder& end();
	const std::string& getAlias() const;
	const std::string& getClassName() const;
	const LogicalGroup& getCondition() const;
	const std::string& getTableName() const;
	JoinType getType() const;
};

class GroupClause {
	QueryBuilder* qb;
	std::vector<std::string> columns;
	LogicalGroup conditions;
	friend class QueryBuilder;
	GroupClause(QueryBuilder* qb);
public:
	GroupClause();
	virtual ~GroupClause();
	LogicalGroup& having();
	QueryBuilder& end();
	const std::vector<std::string>& getColumns() const;
	const LogicalGroup& getConditions() const;
};

class QueryBuilder {
	friend class Query;
	std::string tableName;
	std::string className;
	std::string alias;
	std::map<std::string, std::string> columns;
	bool allCols;
	bool unique;
	int start;
	int count;
	std::vector<JoinClause> joinClauses;
	LogicalGroup conditions;
	std::vector<std::string> columnsAsc;
	std::vector<std::string> columnsDesc;
	std::vector<QueryBuilder> unions;
	std::vector<QueryBuilder> unionAlls;
	GroupClause group;
public:
	QueryBuilder();
	virtual ~QueryBuilder();

	QueryBuilder& allColumns();
	QueryBuilder& columnName(std::string name, std::string calias= "");
	QueryBuilder& columnNames(const std::map<std::string, std::string>& columns);
	QueryBuilder& delimitedColumnNames(const std::string& names);
	QueryBuilder& distinct();

	QueryBuilder& fromTable(const std::string& name, const std::string& alias);
	QueryBuilder& fromClass(const std::string& name, const std::string& alias);

	LogicalGroup& condition();

	GroupClause& groupBy(const std::string& column);
	GroupClause& groupBy(const std::vector<std::string>& columns);
	GroupClause& delimitedGroupByColumns(const std::string& name);

	QueryBuilder& orderByAsc(const std::string& column);
	QueryBuilder& orderByDesc(const std::string& column);

	QueryBuilder& paginate(const int&, const int&);

	JoinClause& joinTable(const JoinType& jt, const std::string& name, const std::string& alias);
	JoinClause& joinClass(const JoinType& jt, const std::string& name, const std::string& alias);

	QueryBuilder& unionQuery(const QueryBuilder& qb);
	QueryBuilder& unionAllQuery(const QueryBuilder& qb);
	const std::string& getAlias() const;
	bool isAllCols() const;
	const std::string& getClassName() const;
	const std::map<std::string, std::string>& getColumns() const;
	const std::vector<std::string>& getColumnsAsc() const;
	const std::vector<std::string>& getColumnsDesc() const;
	const LogicalGroup& getConditions() const;
	int getCount() const;
	const GroupClause& getGroup() const;
	const std::vector<JoinClause>& getJoinClauses() const;
	int getStart() const;
	const std::string& getTableName() const;
	const std::vector<QueryBuilder>& getUnionAlls() const;
	const std::vector<QueryBuilder>& getUnions() const;
	bool isUnique() const;
};

template<class T>
inline LogicalGroup& LogicalGroup::where(const std::string& lhs, const QueryOperator& oper, const T& rhs) {
	Condition cond;
	cond.lhs = lhs;
	cond.oper = oper;
	cond.managed = true;
	GenericObject o;
	o << rhs;
	cond.rhsVec.push_back(o);
	cond.clause = QueryClause::NONE;
	if(condStarted && (lastClause!=QueryClause::AND && lastClause!=QueryClause::OR)) {
		logicalAnd();
	}
	condStarted = true;
	conds.push_back(cond);
	return *this;
}

template<class T>
inline LogicalGroup& LogicalGroup::where(const std::string& lhs, const QueryOperator& oper, const T& rhs, const T& rhs1) {
	Condition cond;
	cond.lhs = lhs;
	cond.oper = oper;
	cond.managed = true;
	GenericObject o;
	o << rhs;
	cond.rhsVec.push_back(o);
	GenericObject o1;
	o1 << rhs1;
	cond.rhsVec.push_back(o1);
	cond.clause = QueryClause::NONE;
	if(condStarted && (lastClause!=QueryClause::AND && lastClause!=QueryClause::OR)) {
		logicalAnd();
	}
	condStarted = true;
	conds.push_back(cond);
	return *this;
}

template<class T>
inline LogicalGroup& LogicalGroup::where(const std::string& lhs, const QueryOperator& oper, const std::vector<T>& rhsList) {
	Condition cond;
	cond.lhs = lhs;
	cond.oper = oper;
	cond.managed = true;
	for (int var = 0; var < (int)rhsList.size(); ++var) {
		GenericObject o;
		o << rhsList.at(var);
		cond.rhsVec.push_back(o);
	}
	cond.clause = QueryClause::NONE;
	if(condStarted && (lastClause!=QueryClause::AND && lastClause!=QueryClause::OR)) {
		logicalAnd();
	}
	condStarted = true;
	conds.push_back(cond);
	return *this;
}

#endif /* QUERYBUILDER_H_ */
