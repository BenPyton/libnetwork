// Copyright (c) 2018 Benoit Pelletier
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef _BINSERIALIZER_H
#define _BINSERIALIZER_H

//#include <iostream>
#include <string>
#include <vector>
#include "Exports.h"

class Serializer;

class LIBNETWORK_API ISerializable
{
public:
	// Serialize the class or struct
	// Don't modify _bs.data() directly, only use _bs.serialize() function that already exists
	virtual void serialize(Serializer& _bs) = 0;
	virtual ~ISerializable() {}
};

class LIBNETWORK_API Serializer
{
private:
	std::size_t m_bufferSize = 0;
	std::size_t m_index = 0;
	std::size_t m_endIndex = 0;
	char* m_buffer = nullptr;
	bool m_write = false;

public:
	enum class Mode
	{
		Read,
		Write
	};

	Serializer(std::size_t _bufsize, Mode _mode);
	Serializer(const Serializer& _bs);
	~Serializer();

	Serializer& operator=(const Serializer& _bs);

	// Accessors
	// for now, avoid mode change
	// inline void setMode(Mode _mode) { m_write = (_mode == Mode::Write); }
	// inline Mode getMode() { return (m_write == true) ? Mode::Write : Mode::Read; }
	inline std::size_t capacity() { return m_bufferSize; }
	inline std::size_t size() { return m_endIndex; }
	inline char* data() { return m_buffer; }

	// Methods
	void clearBuffer(char _v = 0); // set all buffer data to value _v
	inline void rewind() { m_index = 0; }
	void resize(std::size_t _size);

	// Templates
	template<class T>
	void serialize(T& _v);

	// Overload with vector type
	template<typename T>
	void serialize(std::vector<T>& _v);

	// Conversion to string
	inline operator std::string() { return std::string(m_buffer, m_endIndex); }

private:
	void _reallocate(const std::size_t size);
};

template<>
void Serializer::serialize(std::string& _v);

#include "Serializer.inl"

#endif // _BINSERIALIZER_H
