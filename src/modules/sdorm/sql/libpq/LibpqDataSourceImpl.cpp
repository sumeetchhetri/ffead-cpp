/*
	Copyright 2009-2020, Sumeet Chhetri

    Licensed under the Apache License, Version 2.0 (const the& "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/
#include "LibpqDataSourceImpl.h"

std::atomic<bool> LibpqDataSourceImpl::done = false;

DSType LibpqDataSourceImpl::getType() {
	return SD_RAW_SQLPG;
}

LibpqDataSourceImpl::LibpqDataSourceImpl(const std::string& url, bool isAsync) {
	this->url = url;
#ifdef HAVE_LIBPQ
	conn = NULL;
#endif
	this->isAsync = isAsync;
	fd = -1;
}

LibpqDataSourceImpl::~LibpqDataSourceImpl() {
#ifdef HAVE_LIBPQ
	done = true;
	Thread::sSleep(3);
	if(conn!=NULL) {
		PQfinish(conn);
	}
#endif
}

bool LibpqDataSourceImpl::init() {
#ifdef HAVE_LIBPQ
	if(conn!=NULL) return true;
	conn = PQconnectdb(url.c_str());
	if (PQstatus(conn) == CONNECTION_BAD) {
		fprintf(stderr, "Connection to database failed: %s\n", PQerrorMessage(conn));
		PQfinish(conn);
		conn = NULL;
		return false;
	}
	fd = PQsocket(conn);
	if(isAsync) {
		bool rHandler = RequestReaderHandler::getInstance()!=NULL || RequestHandler2::getInstance()!=NULL;
		if(rHandler) {
#ifdef USE_IO_URING
			Thread* pthread = new Thread(&handle, this);
			pthread->execute();
#else
#ifdef HAVE_LIBPQ_BATCH
			if (PQenterBatchMode(conn) == 0)
			{
				fprintf(stderr, "PQenterBatchMode error: %s\n?", PQerrorMessage(conn));
				PQfinish(conn);
			}
			else
			{
				fprintf(stdout, "PQenterBatchMode successful\n");
			}
			rdTsk = new PgBatchReadTask(this);
			//logger = LoggerFactory::getLogger("LibpqDataSourceImpl");
#elif defined(HAVE_LIBPQ_PIPELINE)
			if (PQenterPipelineMode(conn) == 0)
			{
				fprintf(stderr, "PQenterPipelineMode error: %s\n?", PQerrorMessage(conn));
				PQfinish(conn);
			}
			else
			{
				fprintf(stdout, "PQenterPipelineMode successful\n");
			}
			rdTsk = new PgBatchReadTask(this);
			//logger = LoggerFactory::getLogger("LibpqDataSourceImpl");
#else
			fprintf(stdout, "PQconnectdb successful\n");
			rdTsk = new PgReadTask(this);
#endif
			PQsetnonblocking(conn, 1);
#if defined(OS_LINUX) && !defined(DISABLE_BPF)
			Server::set_cbpf(fd, get_nprocs());
#endif
			if(RequestReaderHandler::getInstance()!=NULL) {
				RequestReaderHandler::getInstance()->selector.registerRead(this, false, false, true);
			} else {
				RequestHandler2::getInstance()->selector.registerRead(this, false, false, true);
			}
#endif
		} else {
			PQsetnonblocking(conn, 1);
			Thread* pthread = new Thread(&handle, this);
			pthread->execute();
		}
	}
#endif
	return true;
}

void LibpqDataSourceImpl::handle() {
	rdTsk->run();
}

void LibpqDataSourceImpl::beginAsync(LibpqAsyncReq* areq) {
	if(isAsync) {
#ifdef HAVE_LIBPQ
		LibpqQuery* q = areq->getQuery();
		q->query = "BEGIN";
#endif
	}
}

void LibpqDataSourceImpl::commitAsync(LibpqAsyncReq* areq) {
	if(isAsync) {
#ifdef HAVE_LIBPQ
		LibpqQuery* q = areq->getQuery();
		q->query = "COMMIT";
#endif
	}
}

void LibpqDataSourceImpl::rollbackAsync(LibpqAsyncReq* areq) {
	if(isAsync) {
#ifdef HAVE_LIBPQ
		LibpqQuery* q = areq->getQuery();
		q->query = "ROLLBACK";
#endif
	}
}

bool LibpqDataSourceImpl::begin() {
#ifdef HAVE_LIBPQ
	PGresult *res = PQexec(conn, "BEGIN");
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		printf("BEGIN command failed\n");
		PQclear(res);
		//PQfinish(conn);
		return false;
	}
	PQclear(res);
#endif
	return true;
}

bool LibpqDataSourceImpl::commit() {
#ifdef HAVE_LIBPQ
	PGresult *res = PQexec(conn, "COMMIT");
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		printf("COMMIT command failed\n");
		PQclear(res);
		//PQfinish(conn);
		return false;
	}
	PQclear(res);
#endif
	return true;
}

bool LibpqDataSourceImpl::rollback() {
#ifdef HAVE_LIBPQ
	PGresult *res = PQexec(conn, "ROLLBACK");
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		printf("ROLLBACK command failed\n");
		PQclear(res);
		//PQfinish(conn);
		return false;
	}
	PQclear(res);
#endif
	return true;
}

LibpqAsyncReq* LibpqDataSourceImpl::peek() {
	LibpqAsyncReq* ar = NULL;
	if(rdTsk==NULL)c_mutex.lock();
	if(Q.size()>0) {
		ar = &(Q.front());
	}
	if(rdTsk==NULL)c_mutex.unlock();
	return ar;
}

void LibpqDataSourceImpl::pop() {
	if(rdTsk==NULL)c_mutex.lock();
	if(Q.size()>0) {
		Q.pop_front();
	}
	if(rdTsk==NULL)c_mutex.unlock();
}

void* LibpqDataSourceImpl::handle(void* inp) {
#ifdef HAVE_LIBPQ
	LibpqDataSourceImpl* ths = (LibpqDataSourceImpl*)inp;
	struct timeval tv;
	tv.tv_sec = 2;
	tv.tv_usec = 0;

	while(!done) {
		ths->c_mutex.lock();
		while (!ths->cvar)
		ths->c_mutex.conditionalWait();
		ths->c_mutex.unlock();

		ths->cvar = false;

		LibpqAsyncReq* item = NULL;
		while((item = ths->peek())!=NULL && !done) {
			int counter = -1;
			while(item->q.size()>0) {
				counter ++;
				LibpqQuery* qu = item->peek();

				int psize = (int)qu->pvals.size();

				std::string stmtName;
				if(qu->isPrepared) {
					if(ths->prepStmtMap.find(qu->query)==ths->prepStmtMap.end()) {
						stmtName = CastUtil::fromNumber(ths->prepStmtMap.size()+1);
						ths->prepStmtMap[qu->query] = stmtName;
						PGresult* res = PQprepare(ths->conn, stmtName.c_str(), qu->query.c_str(), psize, NULL);
						if (PQresultStatus(res) != PGRES_COMMAND_OK) {
							fprintf(stderr, "PREPARE failed: %s\n", PQerrorMessage(ths->conn));
							PQclear(res);
							if(item->fcb!=NULL) {
								item->fcb(item->ctx, false, NULL, qu->query, counter);
							} else if(qu->fcb!=NULL) {
								qu->fcb(qu->ctx, false, NULL, qu->query, counter);
							}
							item = NULL;
							break;
						}
					} else {
						stmtName = ths->prepStmtMap[qu->query];
					}
				}

				int qs = -1;
				if(qu->isMulti) {
					qs = PQsendQuery(ths->conn, qu->query.c_str());
				} else if(qu->isPrepared) {
					if(psize==0) {
						qs = PQsendQueryPrepared(ths->conn, stmtName.c_str(), psize, NULL, NULL, NULL, 1);
					} else {
						const char *paramValues[psize];
						int paramLengths[psize];
						int paramBinary[psize];
						int var = 0;
						for(std::list<LibpqParam>::iterator it=qu->pvals.begin(); it != qu->pvals.end(); ++it, var++) {
							if(it->t==1) {//short
								paramValues[var] = (char *)&it->s;
								paramLengths[var] = 2;
							} else if(it->t==2) {//int
								paramValues[var] = (char *)&it->i;
								paramLengths[var] = 4;
							} else if(it->t==3) {//long
								paramValues[var] = (char *)&it->l;
								paramLengths[var] = 8;
							} else {
								paramValues[var] = it->sv.p;
								paramLengths[var] = it->sv.l;
							}
							paramBinary[var] = 1;
						}
						qs = PQsendQueryPrepared(ths->conn, stmtName.c_str(), psize, paramValues, paramLengths, paramBinary, 1);
					}
				} else {
					if(psize==0) {
						qs = PQsendQueryParams(ths->conn, qu->query.c_str(), psize, NULL, NULL, NULL, NULL, 1);
					} else {
						const char *paramValues[psize];
						int paramLengths[psize];
						int paramBinary[psize];
						int var = 0;
						for(std::list<LibpqParam>::iterator it=qu->pvals.begin(); it != qu->pvals.end(); ++it, var++) {
							if(it->t==1) {//short
								paramValues[var] = (char *)&it->s;
								paramLengths[var] = 2;
							} else if(it->t==2) {//int
								paramValues[var] = (char *)&it->i;
								paramLengths[var] = 4;
							} else if(it->t==3) {//long
								paramValues[var] = (char *)&it->l;
								paramLengths[var] = 8;
							} else {
								paramValues[var] = it->sv.p;
								paramLengths[var] = it->sv.l;
							}
							paramBinary[var] = 1;
						}
						qs = PQsendQueryParams(ths->conn, qu->query.c_str(), psize, NULL, paramValues, paramLengths, paramBinary, 1);
					}
				}

				if (!qs) {
					fprintf(stderr, "Failed to send query %s\n", PQerrorMessage(ths->conn));
					if(item->fcb!=NULL) {
						item->fcb(item->ctx, false, NULL, qu->query, counter);
					} else if(qu->fcb!=NULL) {
						qu->fcb(qu->ctx, false, NULL, qu->query, counter);
					}

					item = NULL;
					break;
				}

				bool resDone = false;
				while(!resDone) {
					fd_set read_fds;
					FD_ZERO(&read_fds);
					FD_SET(ths->fd, &read_fds);

					int retval = select(ths->fd + 1, &read_fds, NULL, NULL, &tv);
					switch (retval) {
						case -1:
							perror("select() failed");
							resDone = true;
							break;
						case 0:
							break;
						default:
							if (FD_ISSET(ths->fd, &read_fds)) {
								if (!PQconsumeInput(ths->conn)) {
									fprintf(stderr, "Failed to consume pg input: %s\n", PQerrorMessage(ths->conn));
									throw std::runtime_error("Invalid connection state");
								}
								if(PQisBusy(ths->conn)==1) {
									continue;
								}

								PGresult* res = NULL;
								bool itemDone = false;
								while ((res = PQgetResult(ths->conn))!=NULL && !done) {
									if(itemDone) {
										PQclear(res);
										continue;
									}
									if(qu->isSelect) {
										if (PQresultStatus(res) != PGRES_TUPLES_OK) {
											if(item->fcb!=NULL) {
												item->fcb(item->ctx, false, NULL, qu->query, counter);
											} else if(qu->fcb!=NULL) {
												qu->fcb(qu->ctx, false, NULL, qu->query, counter);
											}
											fprintf(stderr, "SELECT failed: %s\n", PQerrorMessage(ths->conn));
											item->q.clear();
											item->cnt = 0;
										} else {
											switch(qu->cbType) {
												case 0: {
													qu->cb0(qu->ctx, res);
													break;
												}
												case 1: {
													int cols = PQnfields(res);
													int rows = PQntuples(res);
													for(int i=0; i<rows; i++) {
														for (int j = 0; j < cols; ++j) {
															qu->cb1(qu->ctx, (i==rows-1 && j==cols-1), i, j, PQfname(res, j), PQgetvalue(res, i, j), PQgetlength(res, i, j));
														}
													}
													break;
												}
												case 2: {
													int cols = PQnfields(res);
													int rows = PQntuples(res);
													for(int i=0; i<rows; i++) {
														for (int j = 0; j < cols; ++j) {
															qu->cb2(qu->ctx, (i==rows-1 && j==cols-1), i, j, PQgetvalue(res, i, j), PQgetlength(res, i, j));
														}
													}
													break;
												}
												case 3: {
													int cols = PQnfields(res);
													int rows = PQntuples(res);
													for(int i=0; i<rows; i++) {
														for (int j = 0; j < cols; ++j) {
															qu->cb3(qu->ctx, (i==rows-1 && j==cols-1), i, j, PQgetvalue(res, i, j));
														}
													}
													break;
												}
												default: {
													if(qu->fcb!=NULL) {
														qu->fcb(qu->ctx, false, NULL, qu->query, counter);
													}
													break;
												}
											}

											if(item->cnt--==0) {
												if(item->fcb!=NULL) {
													item->fcb(item->ctx, true, &item->results, qu->query, counter);
												}
												itemDone = true;
											} else {
												item->pop();
											}
										}
									} else {
										if (PQresultStatus(res) != PGRES_COMMAND_OK) {
											if(item->fcb!=NULL) {
												item->fcb(item->ctx, false, NULL, qu->query, counter);
											} else if(qu->fcb!=NULL) {
												qu->fcb(qu->ctx, false, NULL, qu->query, counter);
											}
											fprintf(stderr, "UPDATE failed: %s\n", PQerrorMessage(ths->conn));
											PQclear(res);
											item->q.clear();
											item->cnt = 0;
										} else {
											if(item->cnt--==0) {
												if(item->fcb!=NULL) {
													item->fcb(item->ctx, true, &item->results, qu->query, counter);
												}
												itemDone = true;
											} else {
												item->pop();
											}
										}
									}
									PQclear(res);
								}
								resDone = true;
							}
					}
				}
			}
			ths->pop();
		}
	}
#endif
	return NULL;
}

void PgReadTask::run() {
#ifdef HAVE_LIBPQ
	LibpqDataSourceImpl* ths = (LibpqDataSourceImpl*)sif;

	bool ritemDone = false;
	if (!PQconsumeInput(ths->conn)) {
		fprintf(stderr, "Failed to consume pg input: %s\n", PQerrorMessage(ths->conn));
		throw std::runtime_error("Invalid connection state");
	}

	if(PQisBusy(ths->conn)==1) {
		return;
	}

	PGresult* res = NULL;
	while((res = PQgetResult(ths->conn))!=NULL) {
		if(ritemDone) {
			PQclear(res);
			continue;
		}
		if(q->isPrepared && !q->prepared) {
			if (PQresultStatus(res) != PGRES_COMMAND_OK) {
				fprintf(stderr, "PREPARE failed: %s\n", PQerrorMessage(ths->conn));
				if(ritem->fcb!=NULL) {
					ritem->fcb(ritem->ctx, false, NULL, q->query, counter);
				} else if(q->fcb!=NULL) {
					q->fcb(q->ctx, false, NULL, q->query, counter);
				}
				ritemDone = true;
				q = NULL;
				ritem = NULL;
				ths->pop();
				counter = -1;
			} else {
				ritem->cnt--;
			}
			PQclear(res);
		} else if(q->isSelect) {
			if (PQresultStatus(res) != PGRES_TUPLES_OK) {
				if(ritem->fcb!=NULL) {
					ritem->fcb(ritem->ctx, false, NULL, q->query, counter);
				} else if(q->fcb!=NULL) {
					q->fcb(q->ctx, false, NULL, q->query, counter);
				}
				fprintf(stderr, "SELECT failed: %s\n", PQerrorMessage(ths->conn));
				ritemDone = true;
				ritem = NULL;
				ths->pop();
				counter = -1;
				PQclear(res);
			} else {
				switch(q->cbType) {
					case -1: {
						ritem->results.push_back(res);
						break;
					}
					case 0: {
						q->cb0(q->ctx, res);
						PQclear(res);
						break;
					}
					case 1: {
						int cols = PQnfields(res);
						int rows = PQntuples(res);
						for(int i=0; i<rows; i++) {
							for (int j = 0; j < cols; ++j) {
								q->cb1(q->ctx, (i==rows-1 && j==cols-1), i, j, PQfname(res, j), PQgetvalue(res, i, j), PQgetlength(res, i, j));
							}
						}
						PQclear(res);
						break;
					}
					case 2: {
						int cols = PQnfields(res);
						int rows = PQntuples(res);
						for(int i=0; i<rows; i++) {
							for (int j = 0; j < cols; ++j) {
								q->cb2(q->ctx, (i==rows-1 && j==cols-1), i, j, PQgetvalue(res, i, j), PQgetlength(res, i, j));
							}
						}
						PQclear(res);
						break;
					}
					case 3: {
						int cols = PQnfields(res);
						int rows = PQntuples(res);
						for(int i=0; i<rows; i++) {
							for (int j = 0; j < cols; ++j) {
								q->cb3(q->ctx, (i==rows-1 && j==cols-1), i, j, PQgetvalue(res, i, j));
							}
						}
						PQclear(res);
						break;
					}
					case 4: {
						int cols = PQnfields(res);
						int rows = PQntuples(res);
						for(int i=0; i<rows; i++) {
							for (int j = 0; j < cols; ++j) {
								q->cb4(q->ctx, i, j, PQfname(res, j), PQgetvalue(res, i, j), PQgetlength(res, i, j));
							}
						}
						PQclear(res);
						break;
					}
					case 5: {
						int cols = PQnfields(res);
						int rows = PQntuples(res);
						for(int i=0; i<rows; i++) {
							for (int j = 0; j < cols; ++j) {
								q->cb5(q->ctx, i, j, PQgetvalue(res, i, j), PQgetlength(res, i, j));
							}
						}
						PQclear(res);
						break;
					}
					case 6: {
						int cols = PQnfields(res);
						int rows = PQntuples(res);
						for(int i=0; i<rows; i++) {
							for (int j = 0; j < cols; ++j) {
								q->cb6(q->ctx, i, j, PQgetvalue(res, i, j));
							}
						}
						PQclear(res);
						break;
					}
					default: {
						break;
						PQclear(res);
					}
				}

				if(ritem->cnt--==0) {
					if(ritem->fcb!=NULL) {
						ritem->fcb(ritem->ctx, true, &ritem->results, q->query, counter);
					}
					ritemDone = true;
					if(q->isMulti) {
						ritem->pop();
					}
					q = NULL;
					ritem = NULL;
					ths->pop();
					counter = -1;
				} else {
					if(!q->isMulti) {
						ritem->pop();
						q = NULL;
					}
				}
			}
		} else {
			if (PQresultStatus(res) != PGRES_COMMAND_OK) {
				if(ritem->fcb!=NULL) {
					ritem->fcb(ritem->ctx, false, NULL, q->query, counter);
				} else if(q->fcb!=NULL) {
					q->fcb(q->ctx, false, NULL, q->query, counter);
				}
				fprintf(stderr, "UPDATE failed: %s\n", PQerrorMessage(ths->conn));
				ritemDone = true;
				ritem = NULL;
				ths->pop();
				counter = -1;
				q = NULL;
			} else {
				if(ritem->cnt--==0) {
					if(ritem->fcb!=NULL) {
						ritem->fcb(ritem->ctx, true, NULL, q->query, counter);
					}
					ritemDone = true;
					if(q->isMulti) {
						ritem->pop();
					}
					q = NULL;
					ritem = NULL;
					ths->pop();
					counter = -1;
				} else {
					if(!q->isMulti) {
						ritem->pop();
						q = NULL;
					}
				}
			}
			PQclear(res);
		}
	}
	flux = false;
	submit(NULL);
#endif
}

LibpqAsyncReq* PgReadTask::get() {
#ifdef HAVE_LIBPQ
	LibpqDataSourceImpl* ths = (LibpqDataSourceImpl*)sif;
	ths->Q.emplace_back();
	return &(ths->Q.back());
#endif
	return NULL;
}

void PgReadTask::submit(LibpqAsyncReq* nitem) {
#ifdef HAVE_LIBPQ
	LibpqDataSourceImpl* ths = (LibpqDataSourceImpl*)sif;

	if(flux && nitem!=NULL) {
		return;
	}

	if(ritem==NULL) {
		ritem = ths->peek();
	}

	if(ritem!=NULL) {
		if(ritem->q.size()>0) {
			if(q==NULL) {
				counter ++;
				q = ritem->peek();
			}

			int psize = (int)q->pvals.size();

			std::map<std::string, std::string>::iterator it;
			if(q->isPrepared) {
				if((it = ths->prepStmtMap.find(q->query))==ths->prepStmtMap.end()) {
					//fprintf(stdout, "Prepare query....\n");fflush(stdout);
					ritem->cnt++;
					ths->prepStmtMap[q->query] = CastUtil::fromNumber(ths->prepStmtMap.size()+1);
					int qs = PQsendPrepare(ths->conn,ths->prepStmtMap[q->query].c_str(), q->query.c_str(), psize, NULL);

					if (!qs) {
						fprintf(stderr, "Failed to prepare query %s\n", PQerrorMessage(ths->conn));
						if(ritem->fcb!=NULL) {
							ritem->fcb(ritem->ctx, false, NULL, q->query, counter);
						} else if(q->fcb!=NULL) {
							q->fcb(q->ctx, false, NULL, q->query, counter);
						}
						q = NULL;
						ritem = NULL;
						ths->pop();
					} else {
						flux = true;
						PQflush(ths->conn);
					}
					return;
				} else {
					q->prepared = true;
				}
			}


			int qs = -1;
			if(q->isMulti) {
				qs = PQsendQuery(ths->conn, q->query.c_str());
			} else if(q->prepared) {
				if(psize==0) {
					qs = PQsendQueryPrepared(ths->conn, it->second.c_str(), psize, NULL, NULL, NULL, 1);
				} else {
					const char *paramValues[psize];
					int paramLengths[psize];
					int paramBinary[psize];
					int var = 0;
					for(std::list<LibpqParam>::iterator it=q->pvals.begin(); it != q->pvals.end(); ++it, var++) {
						if(it->t==1) {//short
							paramValues[var] = (char *)&it->s;
							paramLengths[var] = 2;
						} else if(it->t==2) {//int
							paramValues[var] = (char *)&it->i;
							paramLengths[var] = 4;
						} else if(it->t==3) {//long
							paramValues[var] = (char *)&it->l;
							paramLengths[var] = 8;
						} else {
							paramValues[var] = it->sv.p;
							paramLengths[var] = it->sv.l;
						}
						paramBinary[var] = 1;
					}
					qs = PQsendQueryPrepared(ths->conn, it->second.c_str(), psize, paramValues, paramLengths, paramBinary, 1);
				}
			} else{
				if(psize==0) {
					qs = PQsendQueryParams(ths->conn, q->query.c_str(), psize, NULL, NULL, NULL, NULL, 1);
				} else {
					const char *paramValues[psize];
					int paramLengths[psize];
					int paramBinary[psize];
					int var = 0;
					for(std::list<LibpqParam>::iterator it=q->pvals.begin(); it != q->pvals.end(); ++it, var++) {
						if(it->t==1) {//short
							paramValues[var] = (char *)&it->s;
							paramLengths[var] = 2;
						} else if(it->t==2) {//int
							paramValues[var] = (char *)&it->i;
							paramLengths[var] = 4;
						} else if(it->t==3) {//long
							paramValues[var] = (char *)&it->l;
							paramLengths[var] = 8;
						} else {
							paramValues[var] = it->sv.p;
							paramLengths[var] = it->sv.l;
						}
						paramBinary[var] = 1;
					}
					qs = PQsendQueryParams(ths->conn, q->query.c_str(), psize, NULL, paramValues, paramLengths, paramBinary, 1);
				}
			}
			//fprintf(stdout, "Send query....\n");fflush(stdout);

			if (!qs) {
				fprintf(stderr, "Failed to send query %s\n", PQerrorMessage(ths->conn));
				if(ritem->fcb!=NULL) {
					ritem->fcb(ritem->ctx, false, NULL, q->query, counter);
				} else if(q->fcb!=NULL) {
					q->fcb(q->ctx, false, NULL, q->query, counter);
				}
				q = NULL;
				ritem = NULL;
				ths->pop();
			} else {
				flux = true;
				PQflush(ths->conn);
			}
		} else {
			ths->pop();
		}
	}
#endif
}

void LibpqDataSourceImpl::postAsync(LibpqAsyncReq* item) {
	if(item!=NULL) {
		if(rdTsk!=NULL) {
#if defined(HAVE_LIBPQ_BATCH) || defined(HAVE_LIBPQ_PIPELINE)
			((PgBatchReadTask*)rdTsk)->submit(item);
#else
			((PgReadTask*)rdTsk)->submit(item);
#endif
			return;
		} else {
			c_mutex.lock();
			cvar = true;
			c_mutex.conditionalNotifyOne();
			c_mutex.unlock();
		}
	}
}

void LibpqDataSourceImpl::postAsync(LibpqAsyncReq* item, int numQ) {
	if(item!=NULL) {
		item->cnt += numQ-1;

		if(rdTsk!=NULL) {
#if defined(HAVE_LIBPQ_BATCH) || defined(HAVE_LIBPQ_PIPELINE)
			((PgBatchReadTask*)rdTsk)->submit(item);
#else
			((PgReadTask*)rdTsk)->submit(item);
#endif
			return;
		} else {
			c_mutex.lock();
			cvar = true;
			c_mutex.conditionalNotifyOne();
			c_mutex.unlock();
		}
	}
}

#ifdef HAVE_LIBPQ
PGresult* LibpqDataSourceImpl::executeSync(LibpqQuery* q) {
	if(q->isMulti) {
		PQsendQuery(conn, q->query.c_str());
		return NULL;
	}

	int paramSize = q->pvals.size();
	if(q->isPrepared) {
		std::map<std::string, std::string>::iterator it;
		if((it=prepStmtMap.find(q->query))==prepStmtMap.end()) {
			prepStmtMap[q->query] = CastUtil::fromNumber(prepStmtMap.size()+1);
			PGresult* res = PQprepare(conn, prepStmtMap[q->query].c_str(), q->query.c_str(), paramSize, NULL);
			if (PQresultStatus(res) != PGRES_COMMAND_OK) {
				fprintf(stderr, "PREPARE failed: %s\n", PQerrorMessage(conn));
				PQclear(res);
				return NULL;
			}
		}

		if(paramSize==0) {
			PGresult* res = PQexecPrepared(conn, it->second.c_str(), 0, NULL, NULL, NULL, 1);
			return res;
		} else {
			const char *paramValues[paramSize];
			int paramLengths[paramSize];
			int paramBinary[paramSize];
			int var = 0;
			for(std::list<LibpqParam>::iterator it=q->pvals.begin(); it != q->pvals.end(); ++it, var++) {
				if(it->t==1) {//short
					paramValues[var] = (char *)&it->s;
					paramLengths[var] = 2;
				} else if(it->t==2) {//int
					paramValues[var] = (char *)&it->i;
					paramLengths[var] = 4;
				} else if(it->t==3) {//long
					paramValues[var] = (char *)&it->l;
					paramLengths[var] = 8;
				} else {
					paramValues[var] = it->sv.p;
					paramLengths[var] = it->sv.l;
				}
				paramBinary[var] = 1;
			}
			PGresult* res = PQexecPrepared(conn, it->second.c_str(), paramSize, paramValues, paramLengths, paramBinary, 1);
			return res;
		}
	} else {
		if(paramSize>0) {
			const char *paramValues[paramSize];
			int paramLengths[paramSize];
			int paramBinary[paramSize];
			int var = 0;
			for(std::list<LibpqParam>::iterator it=q->pvals.begin(); it != q->pvals.end(); ++it, var++) {
				if(it->t==1) {//short
					paramValues[var] = (char *)&it->s;
					paramLengths[var] = 2;
				} else if(it->t==2) {//int
					paramValues[var] = (char *)&it->i;
					paramLengths[var] = 4;
				} else if(it->t==3) {//long
					paramValues[var] = (char *)&it->l;
					paramLengths[var] = 8;
				} else {
					paramValues[var] = it->sv.p;
					paramLengths[var] = it->sv.l;
				}
				paramBinary[var] = 1;
			}
			PGresult* res = PQexecParams(conn, q->query.c_str(), paramSize, NULL, paramValues, paramLengths, paramBinary, 1);
			return res;
		} else {
			PGresult* res = PQexecParams(conn, q->query.c_str(), 0, NULL, NULL, NULL, NULL, 1);
			return res;
		}
	}
}
#endif

LibpqAsyncReq* LibpqDataSourceImpl::getAsyncRequest() {
#ifdef HAVE_LIBPQ
	LibpqAsyncReq* item = NULL;
	if(isAsync) {
		if(rdTsk!=NULL) {
#if defined(HAVE_LIBPQ_BATCH) || defined(HAVE_LIBPQ_PIPELINE)
			item = ((PgBatchReadTask*)rdTsk)->get();
#else
			item = ((PgReadTask*)rdTsk)->get();
#endif
		} else {
			Q.emplace_back();
			item = &(Q.back());
		}

		item->cnt = -1;
	}
	return item;
#endif
	return NULL;
}

void LibpqDataSourceImpl::executeMultiQuery(LibpqQuery* q) {
#ifdef HAVE_LIBPQ
	q->isSelect = true;
	q->isMulti = true;
	executeSync(q);
	PGresult* res = NULL;
	bool stat = true;
	int counter = -1;
	std::vector<PGresult*> results;
	while((res = PQgetResult(conn))!=NULL) {
		counter++;
		if (PQresultStatus(res) != PGRES_TUPLES_OK) {
			fprintf(stderr, "SELECT failed: %s\n", PQerrorMessage(conn));
			if(q->fcb!=NULL) {
				q->fcb(q->ctx, false, NULL, q->query, counter);
			}
			stat = false;
			PQclear(res);
			break;
		} else {
			switch(q->cbType) {
				case -1: {
					results.push_back(res);
					break;
				}
				case 0: {
					q->cb0(q->ctx, res);
					PQclear(res);
					break;
				}
				case 1: {
					int cols = PQnfields(res);
					int rows = PQntuples(res);
					for(int i=0; i<rows; i++) {
						for (int j = 0; j < cols; ++j) {
							q->cb1(q->ctx, (i==rows-1 && j==cols-1), i, j, PQfname(res, j), PQgetvalue(res, i, j), PQgetlength(res, i, j));
						}
					}
					PQclear(res);
					break;
				}
				case 2: {
					int cols = PQnfields(res);
					int rows = PQntuples(res);
					for(int i=0; i<rows; i++) {
						for (int j = 0; j < cols; ++j) {
							q->cb2(q->ctx, (i==rows-1 && j==cols-1), i, j, PQgetvalue(res, i, j), PQgetlength(res, i, j));
						}
					}
					PQclear(res);
					break;
				}
				case 3: {
					int cols = PQnfields(res);
					int rows = PQntuples(res);
					for(int i=0; i<rows; i++) {
						for (int j = 0; j < cols; ++j) {
							q->cb3(q->ctx, (i==rows-1 && j==cols-1), i, j, PQgetvalue(res, i, j));
						}
					}
					PQclear(res);
					break;
				}
				case 4: {
					int cols = PQnfields(res);
					int rows = PQntuples(res);
					for(int i=0; i<rows; i++) {
						for (int j = 0; j < cols; ++j) {
							q->cb4(q->ctx, i, j, PQfname(res, j), PQgetvalue(res, i, j), PQgetlength(res, i, j));
						}
					}
					PQclear(res);
					break;
				}
				case 5: {
					int cols = PQnfields(res);
					int rows = PQntuples(res);
					for(int i=0; i<rows; i++) {
						for (int j = 0; j < cols; ++j) {
							q->cb5(q->ctx, i, j, PQgetvalue(res, i, j), PQgetlength(res, i, j));
						}
					}
					PQclear(res);
					break;
				}
				case 6: {
					int cols = PQnfields(res);
					int rows = PQntuples(res);
					for(int i=0; i<rows; i++) {
						for (int j = 0; j < cols; ++j) {
							q->cb6(q->ctx, i, j, PQgetvalue(res, i, j));
						}
					}
					PQclear(res);
					break;
				}
				default: {
					break;
					PQclear(res);
				}
			}
		}
	}
	if(stat) {
		if(q->fcb!=NULL) {
			q->fcb(q->ctx, true, &results, q->query, counter);
		}
	}
#endif
}

void LibpqDataSourceImpl::executeUpdateMultiQuery(LibpqQuery* q) {
#ifdef HAVE_LIBPQ
	q->isMulti = true;
	executeSync(q);
	PGresult* res = NULL;
	int counter = -1;
	bool stat = true;
	while((res = PQgetResult(conn))!=NULL) {
		counter++;
		if (PQresultStatus(res) != PGRES_COMMAND_OK) {
			fprintf(stderr, "UPDATE failed: %s\n", PQerrorMessage(conn));
			PQclear(res);
			stat = false;
			if(q->fcb!=NULL) {
				q->fcb(q->ctx, false, NULL, q->query, counter);
			}
			break;
		}
		PQclear(res);
	}
	if(stat) {
		if(q->fcb!=NULL) {
			q->fcb(q->ctx, true, NULL, q->query, counter);
		}
	}
#endif
}

void LibpqDataSourceImpl::executeQuery(LibpqQuery* q) {
#ifdef HAVE_LIBPQ
	q->isSelect = true;
	PGresult *res = executeSync(q);
	if (PQresultStatus(res) != PGRES_TUPLES_OK) {
		fprintf(stderr, "SELECT failed: %s\n", PQerrorMessage(conn));
		PQclear(res);
		if(q->fcb!=NULL) {
			q->fcb(q->ctx, false, NULL, q->query, 0);
		}
		return;
	}
	switch(q->cbType) {
		case 0: {
			q->cb0(q->ctx, res);
			break;
		}
		case 1: {
			int cols = PQnfields(res);
			int rows = PQntuples(res);
			for(int i=0; i<rows; i++) {
				for (int j = 0; j < cols; ++j) {
					q->cb1(q->ctx, (i==rows-1 && j==cols-1), i, j, PQfname(res, j), PQgetvalue(res, i, j), PQgetlength(res, i, j));
				}
			}
			break;
		}
		case 2: {
			int cols = PQnfields(res);
			int rows = PQntuples(res);
			for(int i=0; i<rows; i++) {
				for (int j = 0; j < cols; ++j) {
					q->cb2(q->ctx, (i==rows-1 && j==cols-1), i, j, PQgetvalue(res, i, j), PQgetlength(res, i, j));
				}
			}
			break;
		}
		case 3: {
			int cols = PQnfields(res);
			int rows = PQntuples(res);
			for(int i=0; i<rows; i++) {
				for (int j = 0; j < cols; ++j) {
					q->cb3(q->ctx, (i==rows-1 && j==cols-1), i, j, PQgetvalue(res, i, j));
				}
			}
			break;
		}
		case 4: {
			int cols = PQnfields(res);
			int rows = PQntuples(res);
			for(int i=0; i<rows; i++) {
				for (int j = 0; j < cols; ++j) {
					q->cb4(q->ctx, i, j, PQfname(res, j), PQgetvalue(res, i, j), PQgetlength(res, i, j));
				}
			}
			break;
		}
		case 5: {
			int cols = PQnfields(res);
			int rows = PQntuples(res);
			for(int i=0; i<rows; i++) {
				for (int j = 0; j < cols; ++j) {
					q->cb5(q->ctx, i, j, PQgetvalue(res, i, j), PQgetlength(res, i, j));
				}
			}
			break;
		}
		case 6: {
			int cols = PQnfields(res);
			int rows = PQntuples(res);
			for(int i=0; i<rows; i++) {
				for (int j = 0; j < cols; ++j) {
					q->cb6(q->ctx, i, j, PQgetvalue(res, i, j));
				}
			}
			break;
		}
		default: {
			break;
		}
	}

	if(q->fcb!=NULL) {
		q->fcb(q->ctx, true, NULL, q->query, 0);
	}

	PQclear(res);
#endif
}

bool LibpqDataSourceImpl::executeUpdateQuery(LibpqQuery* q) {
#ifdef HAVE_LIBPQ
	PGresult *res = executeSync(q);
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		fprintf(stderr, "UPDATE failed: %s\n", PQerrorMessage(conn));
		PQclear(res);
		if(q->fcb!=NULL) {
			q->fcb(q->ctx, false, NULL, q->query, 0);
		}
		return false;
	}
	PQclear(res);

	if(q->fcb!=NULL) {
		q->fcb(q->ctx, true, NULL, q->query, 0);
	}
#endif
	return true;
}

PgReadTask::~PgReadTask() {
}

PgReadTask::PgReadTask(SocketInterface* sif) {
	this->sif = sif;
	ritem = NULL;
	counter = -1;
	q = NULL;
	flux = false;
}

#if defined(HAVE_LIBPQ_BATCH) || defined(HAVE_LIBPQ_PIPELINE)
PgBatchReadTask::~PgBatchReadTask() {
}

PgBatchReadTask::PgBatchReadTask(SocketInterface* sif) {
	this->sif = sif;
	q = NULL;
	ritem = NULL;
	queueEntries = false;
	sendBatch = true;
}

LibpqAsyncReq* PgBatchReadTask::peek() {
	LibpqAsyncReq* ar = NULL;
	if(lQ.size()>0) {
		ar = &(lQ.front());
	}
	return ar;
}

void PgBatchReadTask::pop() {
	if(lQ.size()>0) {
		lQ.pop_front();
	}
}

void PgBatchReadTask::run() {
#ifdef HAVE_LIBPQ
	LibpqDataSourceImpl* ths = (LibpqDataSourceImpl*)sif;
	//ths->logger.write("run:Resultset ready...Q size %d\n", ths->Q.size());

	int readQueries = 0;
	while(true) {
		//ths->logger << ("run:Read Data waiting...\n");
		if (!PQconsumeInput(ths->conn)) {
			fprintf(stderr, "Failed to consume pg input: %s\n", PQerrorMessage(ths->conn));
			throw std::runtime_error("Invalid connection state");
		}

		if(PQisBusy(ths->conn)==1) {
			/*if (!PQconsumeInput(ths->conn)) {
				fprintf(stderr, "Failed to consume pg input: %s\n", PQerrorMessage(ths->conn));
				throw std::runtime_error("Invalid connection state");
			}
			if(PQisBusy(ths->conn)==1) {
				return;
			}*/
			return;
		}


		PGresult* res = PQgetResult(ths->conn);
		if(res==NULL) {
			//ths->logger << ("run:Null Resultset...\n");
			continue;
		}

		//ths->logger << ("run:Reading Resultset...\n");
#ifdef HAVE_LIBPQ_PIPELINE
		if(PQresultStatus(res) == PGRES_PIPELINE_SYNC) {
#else
		if(PQresultStatus(res) == PGRES_BATCH_END) {
#endif
			//ths->logger.write("run:End batch...%d\n", readQueries);
			sendBatch = true;
			PQclear(res);
			break;
		}

		readQueries++;

		if(ritem==NULL) {
			ritem = ths->peek();
		}

		if(ritem==NULL) {
			PQclear(res);
			//ths->logger.write("run:Resultset without query oops...Q size %q\n", ths->Q.size());
			continue;
		}

		counter++;
		q = ritem->peek();

		if(q->isPrepared && !q->prepared) {
			if (PQresultStatus(res) != PGRES_COMMAND_OK) {
				fprintf(stderr, "PREPARE failed: %s\n", PQerrorMessage(ths->conn));
				if(ritem->fcb!=NULL) {
					ritem->fcb(ritem->ctx, false, NULL, q->query, counter);
				} else if(q->fcb!=NULL) {
					q->fcb(q->ctx, false, NULL, q->query, counter);
				}
				q = NULL;
				ritem = NULL;
				ths->pop();
				counter = -1;
			} else {
				q->prepared = true;
				ritem->cnt--;
			}
			PQclear(res);
		} else if(q->isSelect) {
			if (PQresultStatus(res) != PGRES_TUPLES_OK) {
				if(ritem->fcb!=NULL) {
					ritem->fcb(ritem->ctx, false, NULL, q->query, counter);
				} else if(q->fcb!=NULL) {
					q->fcb(q->ctx, false, NULL, q->query, counter);
				}
				fprintf(stderr, "SELECT failed: %s\n", PQerrorMessage(ths->conn));
				ritem = NULL;
				ths->pop();
				counter = -1;
				PQclear(res);
			} else {
				switch(q->cbType) {
					case -1: {
						ritem->results.push_back(res);
						break;
					}
					case 0: {
						q->cb0(q->ctx, res);
						PQclear(res);
						break;
					}
					case 1: {
						int cols = PQnfields(res);
						int rows = PQntuples(res);
						for(int i=0; i<rows; i++) {
							for (int j = 0; j < cols; ++j) {
								q->cb1(q->ctx, (i==rows-1 && j==cols-1), i, j, PQfname(res, j), PQgetvalue(res, i, j), PQgetlength(res, i, j));
							}
						}
						PQclear(res);
						break;
					}
					case 2: {
						int cols = PQnfields(res);
						int rows = PQntuples(res);
						for(int i=0; i<rows; i++) {
							for (int j = 0; j < cols; ++j) {
								q->cb2(q->ctx, (i==rows-1 && j==cols-1), i, j, PQgetvalue(res, i, j), PQgetlength(res, i, j));
							}
						}
						PQclear(res);
						break;
					}
					case 3: {
						int cols = PQnfields(res);
						int rows = PQntuples(res);
						for(int i=0; i<rows; i++) {
							for (int j = 0; j < cols; ++j) {
								q->cb3(q->ctx, (i==rows-1 && j==cols-1), i, j, PQgetvalue(res, i, j));
							}
						}
						PQclear(res);
						break;
					}
					case 4: {
						int cols = PQnfields(res);
						int rows = PQntuples(res);
						for(int i=0; i<rows; i++) {
							for (int j = 0; j < cols; ++j) {
								q->cb4(q->ctx, i, j, PQfname(res, j), PQgetvalue(res, i, j), PQgetlength(res, i, j));
							}
						}
						PQclear(res);
						break;
					}
					case 5: {
						int cols = PQnfields(res);
						int rows = PQntuples(res);
						for(int i=0; i<rows; i++) {
							for (int j = 0; j < cols; ++j) {
								q->cb5(q->ctx, i, j, PQgetvalue(res, i, j), PQgetlength(res, i, j));
							}
						}
						PQclear(res);
						break;
					}
					case 6: {
						int cols = PQnfields(res);
						int rows = PQntuples(res);
						for(int i=0; i<rows; i++) {
							for (int j = 0; j < cols; ++j) {
								q->cb6(q->ctx, i, j, PQgetvalue(res, i, j));
							}
						}
						PQclear(res);
						break;
					}
					default: {
						PQclear(res);
						break;
					}
				}

				if(ritem->cnt--==0) {
					if(ritem->fcb!=NULL) {
						ritem->fcb(ritem->ctx, true, &ritem->results, q->query, counter);
					}
					if(q->isMulti) {
						ritem->pop();
					}
					q = NULL;
					ritem = NULL;
					ths->pop();
					counter = -1;
				} else {
					if(!q->isMulti) {
						ritem->pop();
						q = NULL;
					}
				}
			}
		} else {
			if (PQresultStatus(res) != PGRES_COMMAND_OK) {
				if(ritem->fcb!=NULL) {
					ritem->fcb(ritem->ctx, false, NULL, q->query, counter);
				} else if(q->fcb!=NULL) {
					q->fcb(q->ctx, false, NULL, q->query, counter);
				}
				fprintf(stderr, "UPDATE failed: %s\n", PQerrorMessage(ths->conn));
				ths->pop();
				ritem = NULL;
				counter = -1;
			} else {
				if(ritem->cnt--==0) {
					if(ritem->fcb!=NULL) {
						ritem->fcb(ritem->ctx, true, NULL, q->query, counter);
					}
					if(q->isMulti) {
						ritem->pop();
					}
					q = NULL;
					ritem = NULL;
					ths->pop();
					counter = -1;
				} else {
					if(!q->isMulti) {
						ritem->pop();
						q = NULL;
					}
				}
			}
			PQclear(res);
		}
		//ths->logger << ("run:Done Resultset...\n");
	}

	processPending();
#endif
}

void PgBatchReadTask::processPending() {
#ifdef HAVE_LIBPQ
	LibpqDataSourceImpl* ths = (LibpqDataSourceImpl*)sif;
	int numQueriesInBatch = 0;
	LibpqAsyncReq* nitem = NULL;

	if(sendBatch && lQ.size()>0) {
		while((nitem = peek())!=NULL) {
			ths->Q.push_back(std::move(*nitem));
			pop();
			nitem = &(ths->Q.back());
			batchQueries(nitem, numQueriesInBatch);
			//if(numQueriesInBatch>=30) break;
		}

		if(numQueriesInBatch>0) {
#ifdef HAVE_LIBPQ_PIPELINE
			if (PQpipelineSync(ths->conn) == 0) {
				fprintf(stderr, "processPending:PQpipelineSync error: %s\n", PQerrorMessage(ths->conn));
#else
			if (PQbatchSendQueue(ths->conn) == 0) {
				fprintf(stderr, "processPending:PQbatchSendQueue error: %s\n", PQerrorMessage(ths->conn));
#endif
			} else {
				//fprintf(stdout, "processPending:PQbatchSendQueue from submit %d, Batch Query Q size %d\n", numQueriesInBatch, ths->Q.size());
				//PQfinish(ths->conn);
				//return;
			}
			PQflush(ths->conn);
			sendBatch = false;
		}
	}
	if(lQ.size()==0) {
		queueEntries = false;
	} else {
		queueEntries = true;
	}
#endif
}

LibpqAsyncReq* PgBatchReadTask::get() {
#ifdef HAVE_LIBPQ
	LibpqDataSourceImpl* ths = (LibpqDataSourceImpl*)sif;
	if(!queueEntries && sendBatch) {
		ths->Q.emplace_back();
		return &(ths->Q.back());
	} else {
		lQ.emplace_back();
		return &(lQ.back());
	}
#endif
	return NULL;
}

void PgBatchReadTask::submit(LibpqAsyncReq* nitem) {
#ifdef HAVE_LIBPQ
	LibpqDataSourceImpl* ths = (LibpqDataSourceImpl*)sif;
	if(nitem!=NULL) {
		if(!queueEntries && sendBatch) {
			//ths->Q.push(nitem);
			int numQueriesInBatch = 0;
			batchQueries(nitem, numQueriesInBatch);
#ifdef HAVE_LIBPQ_PIPELINE
			if (PQpipelineSync(ths->conn) == 0) {
				fprintf(stderr, "submit:PQpipelineSync error: %s\n", PQerrorMessage(ths->conn));
#else
			if (PQbatchSendQueue(ths->conn) == 0) {
				fprintf(stderr, "submit:PQbatchSendQueue error: %s\n", PQerrorMessage(ths->conn));
#endif
				//PQfinish(ths->conn);
			} else {
				//fprintf(stdout, "submit:PQbatchSendQueue from submit %d, Batch Query Q size %d\n", numQueriesInBatch, ths->Q.size());
			}
			PQflush(ths->conn);
			queueEntries = true;
			sendBatch = false;
		} else {
			//lQ.push(nitem);
			//ths->logger.write("submit:Add Query to Pending Queue...%d\n", lQ.size());
		}
	}
#endif
}

void PgBatchReadTask::batchQueries(LibpqAsyncReq* nitem, int& numQueriesInBatch) {
#ifdef HAVE_LIBPQ
	LibpqDataSourceImpl* ths = (LibpqDataSourceImpl*)sif;
	for (int i = 0; i < nitem->q.size(); ++i) {
		LibpqQuery* q = &(nitem->q.at(i));
		int psize = (int)q->pvals.size();

		if(q->isMulti) {
			PQsendQuery(ths->conn, q->query.c_str());
			numQueriesInBatch++;
			return;
		}

		std::map<std::string, std::string>::iterator it;
		if(q->isPrepared) {
			if((it = ths->prepStmtMap.find(q->query))==ths->prepStmtMap.end()) {
				//ths->logger << ("batchQueries:PQsendPrepare\n");
				nitem->cnt++;
				ths->prepStmtMap[q->query] = CastUtil::fromNumber(ths->prepStmtMap.size()+1);
				int qs = PQsendPrepare(ths->conn,ths->prepStmtMap[q->query].c_str(), q->query.c_str(), psize, NULL);
				//printf("ADD PQsendPrepare to batch\n");
				numQueriesInBatch++;

				if (!qs) {
					fprintf(stderr, "Failed to prepare query %s\n", PQerrorMessage(ths->conn));
					if(nitem->fcb!=NULL) {
						nitem->fcb(nitem->ctx, false, NULL, q->query, i+1);
					} else if(q->fcb!=NULL) {
						q->fcb(q->ctx, false, NULL, q->query, i+1);
					}
					return;
				}
			} else {
				q->prepared = true;
			}
		}

		int qs = -1;
		if(q->isMulti) {
			qs = PQsendQueryParams(ths->conn, q->query.c_str(), 0, NULL, NULL, NULL, NULL, 1);
			//ths->logger << ("batchQueries:ADD PQsendQueryParams to batch\n");
		} else if(q->prepared) {
			if(psize==0) {
				qs = PQsendQueryPrepared(ths->conn, it->second.c_str(), psize, NULL, NULL, NULL, 1);
			} else {
				const char *paramValues[psize];
				int paramLengths[psize];
				int paramBinary[psize];
				int var = 0;
				for(std::list<LibpqParam>::iterator it=q->pvals.begin(); it != q->pvals.end(); ++it, var++) {
					if(it->t==1) {//short
						paramValues[var] = (char *)&it->s;
						paramLengths[var] = 2;
					} else if(it->t==2) {//int
						paramValues[var] = (char *)&it->i;
						paramLengths[var] = 4;
					} else if(it->t==3) {//long
						paramValues[var] = (char *)&it->l;
						paramLengths[var] = 8;
					} else {
						paramValues[var] = it->sv.p;
						paramLengths[var] = it->sv.l;
					}
					paramBinary[var] = 1;
				}
				qs = PQsendQueryPrepared(ths->conn, it->second.c_str(), psize, paramValues, paramLengths, paramBinary, 1);
			}
		} else{
			if(psize==0) {
				qs = PQsendQueryParams(ths->conn, q->query.c_str(), psize, NULL, NULL, NULL, NULL, 1);
			} else {
				const char *paramValues[psize];
				int paramLengths[psize];
				int paramBinary[psize];
				int var = 0;
				for(std::list<LibpqParam>::iterator it=q->pvals.begin(); it != q->pvals.end(); ++it, var++) {
					if(it->t==1) {//short
						paramValues[var] = (char *)&it->s;
						paramLengths[var] = 2;
					} else if(it->t==2) {//int
						paramValues[var] = (char *)&it->i;
						paramLengths[var] = 4;
					} else if(it->t==3) {//long
						paramValues[var] = (char *)&it->l;
						paramLengths[var] = 8;
					} else {
						paramValues[var] = it->sv.p;
						paramLengths[var] = it->sv.l;
					}
					paramBinary[var] = 1;
				}
				qs = PQsendQueryParams(ths->conn, q->query.c_str(), psize, NULL, paramValues, paramLengths, paramBinary, 1);
			}
		}
		numQueriesInBatch++;
		//fprintf(stdout, "Send query....\n");fflush(stdout);

		if (!qs) {
			fprintf(stderr, "Failed to send query %s\n", PQerrorMessage(ths->conn));
			if(nitem->fcb!=NULL) {
				nitem->fcb(nitem->ctx, false, NULL, q->query, i+1);
			} else if(q->fcb!=NULL) {
				q->fcb(q->ctx, false, NULL, q->query, i+1);
			}
			return;
		} else {
			PQflush(ths->conn);
		}
	}
#endif
}
#endif

void LibpqQuery::withParamInt2(unsigned short i) {
	pvals.emplace_back();
	LibpqParam& par = pvals.back();
	par.s = htons(i);
	par.t = 1;
}

void LibpqQuery::withParamInt4(unsigned int i) {
	pvals.emplace_back();
	LibpqParam& par = pvals.back();
	par.i = htonl(i);
	par.t = 2;
}

void LibpqQuery::withParamInt8(long long i) {
	pvals.emplace_back();
	LibpqParam& par = pvals.back();
	par.l = i;
	par.t = 3;
}

void LibpqQuery::withParamStr(const char *i) {
	pvals.emplace_back();
	LibpqParam& par = pvals.back();
	par.sv.p = i;
	par.sv.l = strlen(i);
	par.t = 4;
}

void LibpqQuery::withParamBin(const char *i, size_t len) {
	pvals.emplace_back();
	LibpqParam& par = pvals.back();
	par.sv.p = i;
	par.sv.l = len;
	par.t = 5;
}

LibpqQuery& LibpqQuery::withSelectQuery(const std::string &query, bool isPrepared) {
	this->query = query;
	this->isSelect = true;
	this->isPrepared = this->isMulti?false:isPrepared;
	return *this;
}

LibpqQuery& LibpqQuery::withUpdateQuery(const std::string &query, bool isPrepared) {
	this->query = query;
	this->isSelect = false;
	this->isPrepared = this->isMulti?false:isPrepared;
	return *this;
}

LibpqQuery& LibpqQuery::withContext(void* ctx) {
	this->ctx = ctx;
	return *this;
}

LibpqQuery& LibpqQuery::withMulti() {
	this->isMulti = true;
	this->isPrepared = false;
	return *this;
}

LibpqQuery& LibpqQuery::withPrepared() {
	this->isPrepared = this->isMulti?false:true;
	return *this;
}

LibpqQuery* LibpqAsyncReq::getQuery() {
	q.emplace_back();
	this->cnt++;
	LibpqQuery& query = q.back();
	return &query;
}

LibpqQuery* LibpqAsyncReq::peek() {
	if(q.size()>0) {
		return &(q.front());
	}
	return NULL;
}

LibpqAsyncReq::~LibpqAsyncReq() {
	if(results.size()>0) {
		for (auto res: results) {
#ifdef HAVE_LIBPQ
			PQclear(res);
#endif
		}
	}
}

LibpqAsyncReq::LibpqAsyncReq() {
	ctx = NULL;
	fcb = NULL;
	cnt = 0;
}

void LibpqAsyncReq::pop() {
	q.pop_front();
}

void LibpqQuery::reset() {
	cb0 = NULL;
	cb1 = NULL;
	cb2 = NULL;
	cb3 = NULL;
	cb4 = NULL;
	cb5 = NULL;
	cb6 = NULL;
	fcb = NULL;
	cbType = 0;
	isSelect = false;
	isPrepared = false;
	prepared = false;
	isMulti = false;
	pvals.clear();
	ctx = NULL;
}

LibpqQuery::LibpqQuery() {
	cb0 = NULL;
	cb1 = NULL;
	cb2 = NULL;
	cb3 = NULL;
	cb4 = NULL;
	cb5 = NULL;
	cb6 = NULL;
	fcb = NULL;
	cbType = -1;
	isSelect = false;
	isPrepared = false;
	prepared = false;
	isMulti = false;
	ctx = NULL;
}

/*
LibpqParamsBase* LibpqDataSourceImpl::getParams(int size) {
	if(size>2000 || size<0) {
		return NULL;
	}
	switch(size) {
	case 0: { return new LibpqParams<0>(); }
	case 1: { return new LibpqParams<1>(); }
	case 2: { return new LibpqParams<2>(); }
	case 3: { return new LibpqParams<3>(); }
	case 4: { return new LibpqParams<4>(); }
	case 5: { return new LibpqParams<5>(); }
	case 6: { return new LibpqParams<6>(); }
	case 7: { return new LibpqParams<7>(); }
	case 8: { return new LibpqParams<8>(); }
	case 9: { return new LibpqParams<9>(); }
	case 10: { return new LibpqParams<10>(); }
	}
	return NULL;
}*/
