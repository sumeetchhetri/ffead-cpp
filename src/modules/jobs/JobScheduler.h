/*
	Copyright 2010, Sumeet Chhetri

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
/*
 * JobScheduler.h
 *
 *  Created on: 15-Jul-2013
 *      Author: sumeetc
 */

#ifndef JOBSCHEDULER_H_
#define JOBSCHEDULER_H_
#include "CastUtil.h"
#include "CronTimer.h"
#include "XmlParser.h"
#include "Reflector.h"
#include "Thread.h"
#include "LoggerFactory.h"
#include "Task.h"
#include "GenericObject.h"
#include "ConfigurationData.h"

typedef void* (*JobFunction) (void*, const vals&);

class JobConfig
{
	std::string name;
	std::string cron;
	std::string clas;
	std::string meth;
	std::string app;
	friend class JobScheduler;
};

class JobScheduler {
	class JobTask : public Task
	{
		void* objIns;
		std::string cron, clas;
		std::string name, appName;
		Method meth;
		std::atomic<bool> doRun;
		void run();
		friend class JobScheduler;
		public:
			int getTid();
			void setTid(int tid);
	};
	bool isStarted;
	std::vector<JobConfig> configs;
	std::vector<JobTask*> tasks;
	static JobScheduler* instance;
	JobScheduler();
	virtual ~JobScheduler();
	static void* service(void* arg);
public:
	static void init(const ElementList& tabs, const std::string& appName= "default");
	static void init(const std::string& fileName, const std::string& appName= "default");
	static void start();
	static void stop();
};

#endif /* JOBSCHEDULER_H_ */
