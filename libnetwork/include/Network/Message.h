// Copyright (c) 2019 Benoit Pelletier
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef _MESSAGE_H
#define _MESSAGE_H

#include "Exports.h"
#include <iostream>
#include <vector>
#include <array>
#include <set>
#include <cstdint>
#include "Utils.h"

namespace net
{
	struct LIBNETWORK_API Fragment
	{
		typedef uint16_t ID;
		typedef uint16_t Size;
		struct Header
		{
			ID msg_id;
			ID frag_id;
			ID nb_frag;
			Size data_size;
		};

		static const Size max_size = 512;
		static const Size data_max_size = max_size - sizeof(Header);

		Header header;
		std::array<char, data_max_size> data;

		std::size_t size() const;
		friend bool operator<(const Fragment& _a, const Fragment& _b);
	};

	class LIBNETWORK_API Message
	{
	private:
		uint16_t m_id;
		uint16_t m_nbFrag;
		std::set<Fragment> m_fragments;
		bool m_completed;

	public:
		Message(uint64_t _id, uint16_t _nbFrag);
		Message(const Message& _m) = delete;
		Message(Message&& _m);

		inline uint64_t id() { return m_id; }
		inline uint16_t totalFrag() { return m_nbFrag; }
		inline std::size_t size() { return m_fragments.size(); }
		inline bool isComplete() { return m_completed; }

		void addFragment(const Fragment& _f);
		std::string getString();

		Message& operator=(const Message& _m) = delete;
		Message& operator=(Message&& _m);

		static std::vector<Fragment> FragmentString(uint16_t _id, const std::string& _str);
		static std::vector<Fragment> FragmentBuffer(uint16_t _id, const char* _buf, std::size_t _size);

	private:
		void _swap(Message& _m);
	};
} //namespace net

#endif // _MESSAGE_H
