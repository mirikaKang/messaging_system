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

#include "container.h"

#include "session_types.h"
#include "constexpr_string.h"

#include <memory>
#include <vector>
#include <string>
#include <thread>
#include <future>
#include <optional>

#include "asio.hpp"

using namespace std;

namespace network
{
	class messaging_session;
	class messaging_server : public enable_shared_from_this<messaging_server>
	{
	public:
		messaging_server(const wstring& source_id, const unsigned char& start_code_value = 231, const unsigned char& end_code_value = 67);
		~messaging_server(void);

	public:
		shared_ptr<messaging_server> get_ptr(void);

	public:
		void set_encrypt_mode(const bool& encrypt_mode);
		void set_compress_mode(const bool& compress_mode);
		void set_compress_block_size(const unsigned short& compress_block_size);
		void set_use_message_response(const bool& use_message_response);
		void set_drop_connection_time(const unsigned short& drop_connection_time);
		void set_connection_key(const wstring& connection_key);
		void set_acceptable_target_ids(const vector<wstring>& acceptable_target_ids);
		void set_ignore_target_ids(const vector<wstring>& ignore_target_ids);
		void set_ignore_snipping_targets(const vector<wstring>& ignore_snipping_targets);
		void set_possible_session_types(const vector<session_types>& possible_session_types);
		void set_session_limit_count(const size_t& session_limit_count);

	public:
		void set_connection_notification(const function<void(const wstring&, const wstring&, const bool&)>& notification);
		void set_message_notification(const function<void(shared_ptr<container::value_container>)>& notification);
		void set_file_notification(const function<void(const wstring&, const wstring&, const wstring&, const wstring&)>& notification);
		void set_binary_notification(const function<void(const wstring&, const wstring&, const wstring&, const wstring&, const vector<uint8_t>&)>& notification);
		void set_specific_compress_sequence(const function<vector<uint8_t>(const vector<uint8_t>&, const bool&)>& specific_compress_sequence);
		void set_specific_encrypt_sequence(const function<vector<uint8_t>(const vector<uint8_t>&, const bool&)>& specific_encrypt_sequence);

	public:
		void start(const unsigned short& port, const unsigned short& high_priority = 8, const unsigned short& normal_priority = 8, const unsigned short& low_priority = 8);
		void wait_stop(const unsigned int& seconds = 0);
		void stop(void);

	public:
		void disconnect(const wstring& target_id, const wstring& target_sub_id);
		
	public:
		void echo(void);
		bool send(const container::value_container& message, optional<session_types> type = session_types::message_line);
		bool send(shared_ptr<container::value_container> message, optional<session_types> type = session_types::message_line);
		void send_files(const container::value_container& message);
		void send_files(shared_ptr<container::value_container> message);
		void send_binary(const wstring& target_id, const wstring& target_sub_id, const vector<uint8_t>& data);
		void send_binary(const wstring& source_id, const wstring& source_sub_id, const wstring& target_id, const wstring& target_sub_id, const vector<uint8_t>& data);

	protected:
		void wait_connection(void);
		void connect_condition(shared_ptr<messaging_session> target, const bool& condition);

	private:
		vector<shared_ptr<messaging_session>> current_sessions(void);
		void received_message(shared_ptr<container::value_container> message);
		void received_binary(const wstring& source_id, const wstring& source_sub_id, const wstring& target_id, const wstring& target_sub_id, const vector<uint8_t>& data);

	private:
		bool _encrypt_mode;
		bool _compress_mode;
		bool _use_message_response;
		unsigned short _compress_block_size;
		unsigned short _drop_connection_time;
		wstring _source_id;
		wstring _connection_key;
		unsigned short _high_priority;
		unsigned short _normal_priority;
		unsigned short _low_priority;
		size_t _session_limit_count;
		vector<wstring> _acceptable_target_ids;
		vector<wstring> _ignore_target_ids;
		vector<wstring> _ignore_snipping_targets;
		vector<session_types> _possible_session_types;

	private:
		unsigned char _start_code_value;
		unsigned char _end_code_value;

	private:
		shared_ptr<thread> _thread;
		shared_ptr<asio::io_context> _io_context;
		shared_ptr<asio::ip::tcp::acceptor> _acceptor;

	private:
		optional<promise<bool>> _promise_status;
		future<bool> _future_status;
		vector<shared_ptr<messaging_session>> _sessions;

	private:
		function<void(const wstring&, const wstring&, const bool&)> _connection;
		function<void(shared_ptr<container::value_container>)> _received_message;

		function<void(const wstring&, const wstring&, const wstring&, const wstring&)> _received_file;
		function<void(const wstring&, const wstring&, const wstring&, const wstring&, const vector<uint8_t>&)> _received_data;

	private:
		function<vector<uint8_t>(const vector<uint8_t>&, const bool&)> _specific_compress_sequence;
		function<vector<uint8_t>(const vector<uint8_t>&, const bool&)> _specific_encrypt_sequence;
	};
}