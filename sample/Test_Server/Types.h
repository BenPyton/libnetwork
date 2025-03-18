// Copyright (c) 2019 Benoit Pelletier
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef _TYPES_H
#define _TYPES_H

#include "Network/Serializer.h"

struct Data : public ISerializable
{
	bool success;
	std::vector<char> pos;

	virtual void serialize(Serializer& _bs) override
	{
		_bs.serialize(success);
		_bs.serialize(pos);
	}
};

#endif // _TYPES_H
