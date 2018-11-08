/*
 * SolrSearch.h
 *
 *  Created on: 03-Nov-2018
 *      Author: sumeet
 */

#ifndef ELASTICSEARCH_H_
#define ELASTICSEARCH_H_
#include "SearchEngineInterface.h"

class ElasticSearch: public SearchEngineInterface {
	static std::string COLL_URL;
public:
	ElasticSearch();
	virtual ~ElasticSearch();

	void createIndex(IndexQuery& iq);
	void updateIndex(IndexQuery& iq);
	void removeIndex(IndexQuery& iq);

	void add(DocumentQuery& iq);
	void update(DocumentQuery& iq);
	void remove(DocumentQuery& iq);

	std::string query(SearchQuery& q);
};

#endif /* ELASTICSEARCH_H_ */
