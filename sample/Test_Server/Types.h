#ifndef _TYPES_H
#define _TYPES_H

#include "Network/Serializer.h"

struct Data : public ISerializable
{
	bool success;
	vector<char> pos;

	virtual void serialize(Serializer& _bs) override
	{
		_bs.serialize(success);
		_bs.serialize(pos);
	}
};

#endif // _TYPES_H
