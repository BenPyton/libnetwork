/*
* @author PELLETIER Benoit
*
* @file Serializer.inl
*
* @date 25/10/2018
*
* @brief Serialize some object into char arrays
*
*/

template<typename T>
void Serializer::serialize(T& _v)
{
	if (is_base_of<ISerializable, T>::value)
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
			size_t byteSize = sizeof(T);

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

template<>
void Serializer::serialize(string & _v)
{
	if (m_write)
	{
		size_t size = _v.size();
		size_t byteSize = size * sizeof(char);

		// add string size before data
		serialize(size);

		// Reallocate if necessary
		if (m_bufferSize < m_index + byteSize)
		{
			resize(m_index + byteSize);
		}

		// add string data
		memcpy(m_buffer + m_index, _v.c_str(), byteSize);
		m_index += byteSize;
		if (m_index > m_endIndex)
			m_endIndex = m_index;
	}
	else // read
	{
		size_t size;
		// get string size before data
		serialize(size);
		// get string data (char by char)
		_v.resize(size);
		for (size_t i = 0; i < size; i++)
		{
			_v[i] = (m_buffer + m_index)[i];
		}

		m_index += size * sizeof(char);
	}
}

template<typename T>
void Serializer::serialize(vector<T>& _v)
{
	if (m_write)
	{
		size_t size = _v.size();
		// add vector size before data
		serialize(size);

		size_t byteSize = sizeof(T);
		// Reallocate if necessary
		if (m_bufferSize < m_index + size * byteSize)
		{
			resize(m_index + size * byteSize);
		}

		for (typename vector<T>::iterator it = _v.begin(); it != _v.end(); ++it)
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
		size_t size;
		// get vector size before data
		serialize(size);
		_v.resize(size); // resize the vector
		// Get vector elements one by one
		for (size_t i = 0; i < size; i++)
		{
			serialize(_v[i]);
		}
	}
}
