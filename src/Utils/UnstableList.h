#ifndef NXNA_UTILS_UNSTABLELIST_H
#define NXNA_UTILS_UNSTABLELIST_H

namespace Nxna
{
namespace Utils
{
	template<typename T>
	class UnstableList
	{
		std::vector<T> m_data;

	public:
		void Add(const T& item)
		{
			m_data.push_back(item);
		}

		static void Add(const T& item, std::vector<T>& data)
		{
			data.push_back(item);
		}

		void Remove(const T& item)
		{
			for (size_t i = 0; i < m_data.size(); i++)
			{
				if (m_data[i] == item)
				{
					RemoveAt(i);
					break;
				}
			}
		}

		static void Remove(const T& item, std::vector<T>& data)
		{
			for (size_t i = 0; i < data.size(); i++)
			{
				if (data[i] == item)
				{
					RemoveAt(i, data);
					break;
				}
			}
		}

		void RemoveAt(size_t index)
		{
			if (index + 1 < m_data.size())
			{
				// swap the rear element to this one
				m_data[index] = m_data[m_data.size() - 1];
			}

			m_data.pop_back();
		}

		static void RemoveAt(size_t index, std::vector<T>& data)
		{
			if (index + 1 < data.size())
			{
				// swap the rear element to this one
				data[index] = data[data.size() - 1];
			}

			data.pop_back();
		}

		size_t GetSize()
		{
			return m_data.size();
		}

		T Get(size_t index)
		{
			return m_data[index];
		}

		T* GetData()
		{
			return m_data.data();
		}

		typename std::vector<T>::iterator Begin()
		{
			return m_data.begin();
		}

		typename std::vector<T>::iterator End()
		{
			return m_data.end();
		}
	};
}
}

#endif // NXNA_UTILS_UNSTABLELIST_H
