#ifndef _TYPES_H
#define _TYPES_H

#include "BinSerializer.h"

//#ifdef _DEBUG
//	//#include "stdio.h"
//	#define DebugLog(fmt, ...) printf(fmt, ##__VA_ARGS__)
//#else
//	#define DebugLog(fmt, ...)
//#endif


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