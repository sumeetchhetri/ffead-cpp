/*
 * SolrSearch.h
 *
 *  Created on: 03-Nov-2018
 *      Author: sumeet
 */

#ifndef SOLRSEARCH_H_
#define SOLRSEARCH_H_
#include "SearchEngineInterface.h"

class SolrSearch: public SearchEngineInterface {
	static std::string COLL_C_PARAMS, COLL_M_PARAMS;
	bool isCloud;
public:
	SolrSearch();
	virtual ~SolrSearch();

	void createIndex(IndexQuery& iq);
	void updateIndex(IndexQuery& iq);
	void removeIndex(IndexQuery& iq);

	void add(DocumentQuery& iq);
	void update(DocumentQuery& iq);
	void remove(DocumentQuery& iq);

	std::string query(SearchQuery& q);
};

#endif /* SOLRSEARCH_H_ */
