/*
* @author PELLETIER Benoit
*
* @file BinSerializer.cpp
*
* @date 25/10/2018
*
* @brief Serialize some object into char arrays
*
*/

#include "stdafx.h"
#include "BinSerializer.h"


BinSerializer::BinSerializer(size_t _bufsize, Mode _mode)
	:m_buffer(nullptr), m_bufferSize(1), m_endIndex(0), m_index(0)
{
	m_write = (_mode == Mode::Write);

	m_buffer = (char*)malloc(m_bufferSize * sizeof(char));
}

BinSerializer::BinSerializer(const BinSerializer & _bs)
	:m_buffer(nullptr), m_bufferSize(_bs.m_bufferSize), m_endIndex(_bs.m_endIndex), m_index(_bs.m_index)
{
	m_write = _bs.m_write;

	m_buffer = (char*)malloc(m_bufferSize);
	memcpy(m_buffer, _bs.m_buffer, m_bufferSize);
}

BinSerializer::~BinSerializer()
{
	if (nullptr != m_buffer)
	{
		free(m_buffer);
	}
}

BinSerializer & BinSerializer::operator=(const BinSerializer & _bs)
{
	char* localBuffer = (char*)malloc(_bs.m_bufferSize);
	// Stop here if malloc failed (localBuffer == nullptr)

	if (nullptr != localBuffer)
	{
		// free existing buffer
		if (nullptr != m_buffer)
		{
			free(m_buffer);
		}
		m_buffer = localBuffer;

		m_bufferSize = _bs.m_bufferSize;
		m_write = _bs.m_write;
		memcpy(m_buffer, _bs.m_buffer, m_bufferSize);
	}
	return *this;
}

void BinSerializer::clearBuffer(char _v)
{
	memset(m_buffer, _v, m_bufferSize);
}

void BinSerializer::resize(size_t _size)
{
	size_t newSize = m_bufferSize;
	while (newSize < _size)
	{
		newSize *= 2;
	}
	_reallocate(newSize);
	m_endIndex = _size;
}

void BinSerializer::_reallocate(const size_t _size)
{
	if (m_bufferSize < _size)
	{
		m_buffer = (char*)realloc(m_buffer, _size);
		m_bufferSize = _size;
	}
}
