/*****************************************************************************
BSD 3-Clause License

Copyright (c) 2021, 🍀☀🌕🌥 🌊
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*****************************************************************************/

#include "thread_pool.h"

#include "logging.h"
#include "job_pool.h"

#include "fmt/xchar.h"
#include "fmt/format.h"

namespace threads
{
	using namespace logging;

	thread_pool::thread_pool(const wstring& title, const vector<shared_ptr<thread_worker>>& workers)
		: _workers(workers), _job_pool(make_shared<job_pool>(title)), _promise_status({}), _title(title)
	{
	}

	thread_pool::~thread_pool(void)
	{
		stop();
	}

	void thread_pool::start(void)
	{
		scoped_lock<mutex> guard(_mutex);

		for (auto& worker : _workers)
		{
			if (worker == nullptr)
			{
				continue;
			}

			worker->start();
		}
	}

	void thread_pool::append(shared_ptr<thread_worker> worker, const bool& start)
	{
		unique_lock<mutex> unique(_mutex);
		
		worker->set_worker_notification(bind(&thread_pool::worker_notification, this, placeholders::_1, placeholders::_2));
		worker->set_job_pool(_job_pool);
		_workers.push_back(worker);
		_worker_conditions.insert({ worker->guid(), false });

		logger::handle().write(logging_level::parameter, fmt::format(L"appended new worker on {}: priority - {}", _title, (int)worker->priority()));

		unique.unlock();

		if (start)
		{
			worker->start();
		}
	}

	void thread_pool::stop(const bool& stop_immediately, const bool& jop_pool_lock)
	{
		scoped_lock<mutex> guard(_mutex);
		
		if (stop_immediately && _job_pool != nullptr)
		{
			_job_pool.reset();
		}

		if (_job_pool != nullptr)
		{
			_job_pool->set_push_lock(jop_pool_lock);

			if (!_promise_status.has_value())
			{
				_job_pool->append_notification(L"thread_pool", bind(&thread_pool::empty_pool_notification, this, placeholders::_1));

				_promise_status = { promise<bool>() };
				_future_status = _promise_status.value().get_future();
				_future_status.wait();
				_promise_status.reset();

				_job_pool->remove_notification(L"thread_pool");
			}

			_job_pool.reset();
		}

		for (auto& worker : _workers)
		{
			if (worker == nullptr)
			{
				continue;
			}

			worker->stop();
		}

		_workers.clear();
	}

	void thread_pool::push(shared_ptr<job> job)
	{
		if (_job_pool == nullptr)
		{
			return;
		}

		_job_pool->push(job);
	}

	void thread_pool::empty_pool_notification(const priorities& priority)
	{
		if (priority != priorities::none)
		{
			return;
		}

		for (auto& worker_condition : _worker_conditions)
		{
			if (worker_condition.second)
			{
				return;
			}
		}

		if (_promise_status.has_value())
		{
			_promise_status.value().set_value(true);
		}
	}

	void thread_pool::worker_notification(const wstring& id, const bool& working_condition)
	{
		auto target = _worker_conditions.find(id);
		if (target == _worker_conditions.end())
		{
			return;
		}

		target->second = working_condition;

		if (_job_pool == nullptr)
		{
			return;
		}

		_job_pool->check_empty();
	}
}