#ifndef _TYPES_H
#define _TYPES_H

#include "Network/BinSerializer.h"

struct Data : public ISerializable
{
	bool success;
	vector<char> pos;

	virtual void serialize(BinSerializer & _bs) override
	{
		_bs.serialize(success);
		_bs.serialize(pos);
	}
};


#endif // _TYPES_H