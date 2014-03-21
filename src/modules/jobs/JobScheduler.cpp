/*
	Copyright 2010, Sumeet Chhetri

    Licensed under the Apache License, Version 2.0 (the "License");
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
 * JobScheduler.cpp
 *
 *  Created on: 15-Jul-2013
 *      Author: sumeetc
 */

#include "JobScheduler.h"

JobScheduler* JobScheduler::instance = NULL;

JobScheduler::JobScheduler() {
	isStarted = false;
}

JobScheduler::~JobScheduler() {
}

void JobScheduler::init(ElementList tabs, string appName) {
	if(instance!=NULL)
		return;
	Logger logger = LoggerFactory::getLogger("JOB", "JobScheduler");
	instance = new JobScheduler();
	logger << "Initialized JobScheduler" << endl;
	for (unsigned int dn = 0; dn < tabs.size(); dn++)
	{
		if(tabs.at(dn).getTagName()!="job-proc")
		{
			logger << "Invalid Element Tag found inside job-procs, should be job-proc..." << endl;
			continue;
		}
		string clas = tabs.at(dn).getAttribute("class");
		string cron = tabs.at(dn).getAttribute("cron");
		string meth = tabs.at(dn).getAttribute("method");
		string name = tabs.at(dn).getAttribute("name");
		if(clas!="" && meth!="" && cron!="" && name!="")
		{
			JobConfig config;
			config.name = name;
			config.cron = cron;
			config.clas = clas;
			config.meth = meth;
			config.app = appName;
			instance->configs.push_back(config);
			logger << "Added JobConfig inside JobScheduler" << endl;
		}
		else
		{
			logger << "Cannot add Job Process as some mandatory elements are missing from the configuration" << endl;
		}
	}
}


void JobScheduler::start() {
	Reflector ref;
	if(instance!=NULL && instance->isStarted)
		return;
	Logger logger = LoggerFactory::getLogger("JOB", "JobScheduler");
	instance->isStarted = true;
	for (unsigned int dn = 0; dn < (int)instance->configs.size(); dn++)
	{
		string clas = instance->configs.at(dn).clas;
		string cron = instance->configs.at(dn).cron;
		string method = instance->configs.at(dn).meth;
		string name = instance->configs.at(dn).name;
		string appName = instance->configs.at(dn).app;
		if(clas!="" && method!="" && cron!="" && name!="")
		{
			ClassInfo claz = ref.getClassInfo(clas, appName);
			logger << "JobScheduler - Got class " + claz.getClassName() << endl;
			if(claz.getClassName()!="")
			{
				args argus;
				Method meth = claz.getMethod(method, argus);
				Constructor ctor = claz.getConstructor(argus);

				logger << "JobScheduler - Got method,class " + meth.getMethodName() + "," + ctor.getName() << endl;

				if(meth.getMethodName()!="" && ctor.getName()!="")
				{
					void* objIns = ref.newInstanceGVP(ctor, appName);
					JobFunction f = (JobFunction)ref.getMethodInstance(meth, appName);

					logger << "JobScheduler - Got objins,func " << objIns << "," << f << endl;

					if(objIns!=NULL && f!=NULL)
					{
						JobTask* task = new JobTask;
						task->objIns = objIns;
						task->cron = cron;
						task->name = name;
						task->meth = meth;
						task->appName = appName;
						task->doRun = true;

						Thread pthread(&JobScheduler::service, task);
						pthread.execute();

						instance->tasks.push_back(task);
						logger << "Added Job Process successfully" << endl;
					}
					else
					{
						logger << "Cannot add Job Process as could not initialize class instance and method" << endl;
					}
				}
				else
				{
					logger << "Cannot add Job Process as method was not found or public no argument constructor not defined for class" << endl;
				}
			}
			else
			{
				logger << "Cannot add Job Process as class information was not found" << endl;
			}
		}
		else
		{
			logger << "Cannot add Job Process as some mandatory elements are missing from the configuration" << endl;
		}
	}
}

void JobScheduler::stop() {
	if(instance==NULL)
	{
		return;
	}
	for (int var = 0; var < (int)instance->tasks.size(); ++var) {
		//instance->tasks.at(var)->mutex.lock();
		instance->tasks.at(var)->doRun = false;
		//instance->tasks.at(var)->mutex.unlock();
	}
	Logger logger = LoggerFactory::getLogger("JOB", "JobScheduler");
	logger << "Waiting 10 seconds for all Job Processes to shutdown....";
	sleep(10);
	delete instance;
}

void* JobScheduler::service(void* arg)
{
	JobTask *task = (JobTask*)arg;
	task->run();
	delete task;
	return NULL;
}


void JobScheduler::JobTask::run() {
	Logger logger = LoggerFactory::getLogger("JOB", "JobTask");
	try {
		CronTimer timer(cron);
		vals values;
		timer.nextRunDate = new Date;
		bool toRun;
		//task->mutex.lock();
		toRun = doRun;
		//task->mutex.unlock();

		Reflector ref;
		JobFunction f = (JobFunction)ref.getMethodInstance(meth, appName);

		while(toRun)
		{
			sleep(1);
			Date d2;
			if(timer.isValid(5, d2.getYearStr(), timer.nextRunDate->getYearStr()))
			{
				if(timer.isValid(3, d2.getMonthStr(), timer.nextRunDate->getMonthStr()))
				{
					if(timer.isValid(2, d2.getDayStr(), timer.nextRunDate->getDayStr()))
					{
						if(timer.isValid(1, d2.getHhStr(), timer.nextRunDate->getHhStr()))
						{
							if(timer.isValid(0, d2.getMmStr(), timer.nextRunDate->getMmStr()))
							{
								logger << "Running Job Process " + name << endl;

								f(objIns, values);

								bool incrementDone = false;

								incrementDone = timer.tryIncrement(0, timer.nextRunDate->getMmStr());
								if(!incrementDone) {
									incrementDone = timer.tryIncrement(1, timer.nextRunDate->getHhStr());
								}

								if(!incrementDone) {
									incrementDone = timer.tryIncrement(2, timer.nextRunDate->getDayStr());
								}

								if(!incrementDone) {
									incrementDone = timer.tryIncrement(3, timer.nextRunDate->getMonthStr());
								}
								if(!incrementDone) {
									incrementDone = timer.tryIncrement(5, timer.nextRunDate->getYearStr());
								}

								logger << "Running Job Process " + name + " complete" << endl;
							}
						}
					}
				}
			}
		}
	} catch(const char* ex) {
		logger << "Cannot run Job as the cron string is invalid" << endl;
	}
}

