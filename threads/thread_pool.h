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

#pragma once

#include "thread_worker.h"

#include <map>
#include <mutex>
#include <memory>
#include <vector>
#include <future>
#include <optional>

using namespace std;

namespace threads
{
	class thread_pool : public enable_shared_from_this<thread_pool>
	{
	public:
		thread_pool(const wstring& title = L"thread_pool", const vector<shared_ptr<thread_worker>>& workers = {});
		~thread_pool(void);

	public:
		void start(void);
		void append(shared_ptr<thread_worker> worker, const bool& start = false);
		void stop(const bool& stop_immediately = true, const bool& jop_pool_lock = false);

	public:
		void push(shared_ptr<job> job);

	protected:
		void empty_pool_notification(const priorities& priority);
		void worker_notification(const wstring& id, const bool& working_condition);

	private:
		optional<promise<bool>> _promise_status;
		future<bool> _future_status;

	private:
		mutex _mutex;
		wstring _title;
		shared_ptr<job_pool> _job_pool;
		map<wstring, bool> _worker_conditions;
		vector<shared_ptr<thread_worker>> _workers;
	};
}

