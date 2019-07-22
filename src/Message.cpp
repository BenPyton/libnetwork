/*
* @author PELLETIER Benoit
*
* @file Message.cpp
*
* @date 08/02/2019
*
* @brief Manage fragmented message transiting throught network
*
*/

#include "Network/Message.h"
#include "Utils.h"

namespace net
{
	size_t Fragment::size() const
	{
		return sizeof(Header) + header.data_size;
	}

	bool operator<(const Fragment& _a, const Fragment& _b)
	{
		// ordering only by fragment id
		return _a.header.frag_id < _b.header.frag_id;
	}


	Message::Message(uint64_t _id, uint16_t _nbFrag)
		: m_id(_id), m_nbFrag(_nbFrag), m_completed(false)
	{
	}

	Message::Message(Message && _m)
	{
		_m._swap(*this);
	}

	Message& Message::operator=(Message&& _m)
	{
		_m._swap(*this);
		return *this;
	}

	void Message::addFragment(const Fragment & _f)
	{
		if (_f.header.msg_id == m_id)
		{
			m_fragments.insert(_f);
			m_completed = (m_fragments.size() == _f.header.nb_frag);
		}
	}

	string Message::getString()
	{
		string str = "";
		for (set<Fragment>::iterator it = m_fragments.begin(); it != m_fragments.end(); ++it)
		{
			str.append(it->data.data(), it->header.data_size);
		}

		return str;
	}

	vector<Fragment> Message::FragmentString(uint16_t _id, const string & _str)
	{
		vector<Fragment> fragments;

		uint16_t lastFragSize = _str.size() % Fragment::data_max_size;
		uint16_t nbFrag = _str.size() / Fragment::data_max_size + (lastFragSize != 0 ? 1 : 0);
		uint16_t fragSize = 0;
		for (int i = 0; i < nbFrag; i++)
		{
			fragSize = Fragment::data_max_size;
			if (i == nbFrag - 1) // last fragment
			{
				fragSize = lastFragSize;
			}

			Fragment frag;
			frag.header = { (Fragment::ID)_id, (Fragment::ID)i, (Fragment::ID)nbFrag, (Fragment::Size)fragSize };
			memcpy(frag.data.data(), _str.data() + i * Fragment::data_max_size, fragSize);

			fragments.push_back(frag);
		}

		return fragments;
	}

	vector<Fragment> Message::FragmentBuffer(uint16_t _id, const char * _buf, size_t _size)
	{
		vector<Fragment> fragments;

		uint16_t lastFragSize = _size % Fragment::data_max_size;
		uint16_t nbFrag = _size / Fragment::data_max_size + (lastFragSize != 0 ? 1 : 0);
		uint16_t fragSize = 0;
		for (int i = 0; i < nbFrag; i++)
		{
			fragSize = Fragment::data_max_size;
			if (i == nbFrag - 1) // last fragment
			{
				fragSize = lastFragSize;
			}

			Fragment frag;
			frag.header = { (Fragment::ID)_id, (Fragment::ID)i, (Fragment::ID)nbFrag, (Fragment::Size)fragSize };
			memcpy(frag.data.data(), _buf + i * Fragment::data_max_size, fragSize);

			fragments.push_back(frag);
		}

		return fragments;
	}


	void Message::_swap(Message& _m)
	{
		std::swap(m_id, _m.m_id);
		std::swap(m_nbFrag, _m.m_nbFrag);
		std::swap(m_fragments, _m.m_fragments);
		std::swap(m_completed, _m.m_completed);
	}
}
