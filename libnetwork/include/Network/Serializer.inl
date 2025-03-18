// Copyright (c) 2018 Benoit Pelletier
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

template<typename T>
void Serializer::serialize(T& _v)
{
	if (std::is_base_of<ISerializable, T>::value)
	{
		// On fait du gros gros forcing pour caster en ISerializable (dynamic_cast ne fonctionne pas sur T*)
		// On est certain que ca fonctionnera car on teste juste avant si la classe T derive de ISerializable
		// (Par contre on est d'accord que c'est tres sale de faire comme ca)
		// Pas sur que ca fonctionne en Release (si c'est bon ca fonctionne)
		((ISerializable*)((void*)(&_v)))->serialize(*this);
	}
	else // standard serialization
	{
		if (m_write)
		{
			std::size_t byteSize = sizeof(T);

			// Reallocate if necessary
			if (m_bufferSize < m_index + byteSize)
			{
				resize(m_index + byteSize);
			}

			memcpy(m_buffer + m_index, (char*)&_v, byteSize);
			m_index += byteSize;
			if (m_index > m_endIndex)
				m_endIndex = m_index;
		}
		else // read
		{
			memcpy((char*)&_v, m_buffer + m_index, sizeof(T));
			m_index += sizeof(T);
		}
	}
}

template<typename T>
void Serializer::serialize(std::vector<T>& _v)
{
	if (m_write)
	{
		std::size_t size = _v.size();
		// add vector size before data
		serialize(size);

		std::size_t byteSize = sizeof(T);
		// Reallocate if necessary
		if (m_bufferSize < m_index + size * byteSize)
		{
			resize(m_index + size * byteSize);
		}

		for (typename std::vector<T>::iterator it = _v.begin(); it != _v.end(); ++it)
		{
			serialize(*it);
		}

		//// Check space left from cursor
		//if (m_index + _v.size() * sizeof(T) < m_bufferSize)
		//{
		//	// add vector data
		//	memcpy(m_buffer + m_index, _v.data(), _v.size() * sizeof(T));
		//	m_index += _v.size() * sizeof(T);
		//}
	}
	else // read
	{
		std::size_t size;
		// get vector size before data
		serialize(size);
		_v.resize(size); // resize the vector
		// Get vector elements one by one
		for (std::size_t i = 0; i < size; i++)
		{
			serialize(_v[i]);
		}
	}
}
