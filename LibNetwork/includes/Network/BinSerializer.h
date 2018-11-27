/*
* @author PELLETIER Benoit
*
* @file BinSerializer.h
*
* @date 25/10/2018
*
* @brief Serialize some object into char arrays
*
*/

#ifndef _BINSERIALIZER_H
#define _BINSERIALIZER_H

//#include <iostream>
#include <string>
#include <vector>
#include "Exports.h"

using namespace std;

class BinSerializer;
class LIBNETWORK_API ISerializable
{
public:
	// Serialize the class or struct
	// Don't modify _bs.data() directly, only use _bs.serialize() function that already exists
	virtual void serialize(BinSerializer& _bs) = 0;
	virtual ~ISerializable() {}
};

class LIBNETWORK_API BinSerializer
{
private:

	char* m_buffer = nullptr;
	uint64_t m_index = 0;
	bool m_write = false;
	size_t m_bufferSize = 0;
	size_t m_endIndex = 0;

public:
	LIBNETWORK_API enum Mode {
		Read,
		Write
	};

	BinSerializer(size_t _bufsize, Mode _mode);
	BinSerializer(const BinSerializer& _bs);
	~BinSerializer();

	BinSerializer& operator=(const BinSerializer& _bs);

	// Accessors
	// for now, avoid mode change
	// inline void setMode(Mode _mode) { m_write = (_mode == Mode::Write); }
	// inline Mode getMode() { return (m_write == true) ? Mode::Write : Mode::Read; }
	inline size_t capacity() { return m_bufferSize; }
	inline size_t size() { return m_endIndex; }
	inline char* data() { return m_buffer; }

	// Methods
	void clearBuffer(char _v = 0); // set all buffer data to value _v
	inline void rewind() { m_index = 0; }
	void resize(size_t _size);

	// Templates
	template<class T> void serialize(T& _v);

	// Specializations
	template<> void serialize(string& _v);

	// Overload with vector type
	template<typename T> void serialize(vector<T>& _v);

private:
	void _reallocate(const size_t size);
};

#include "BinSerializer.inl"

#endif // _BINSERIALIZER_H
