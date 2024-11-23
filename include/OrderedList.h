#pragma once

#include <stdint.h>

namespace uazips
{

    template<typename T>
    class OrderedList
    {
    private:
        T* array;
        size_t arr_capacity;
        size_t arr_size;

    public:
        inline OrderedList() : OrderedList(1)
        {
        }

        inline OrderedList(size_t cap) : arr_capacity(cap), arr_size(0)
        {
            array = new T[cap];
            if (!array)
                THROW("Out of memory.");
        }

        inline ~OrderedList()
        {
            delete[] array;
        }

        inline void Push(T data)
        {
            if (arr_capacity == arr_size)
            {
                T* temp = new T[arr_capacity++];
                if (!temp)
                    THROW("Out of memory.");
                for (int i = 0; i <= arr_capacity; i++)
                    temp[i] = array[i];
                delete[] array;
                array = temp;
            }
            array[arr_size] = data;
            arr_size++;
        }

        inline void Set(T data, size_t index)
        {
            if (arr_capacity <= index)
                Push(data);
            else
                array[index] = data;
        }

        inline T Get(size_t index) const
        {
            if (index < arr_size)
                return array[index];
            return reinterpret_cast<T>(-1);
        }

        inline T operator[](size_t index)
        {
            return Get(index);
        }

        inline const T& Pop()
        {
            const T& ret = array[arr_size];
            arr_size--;
            return ret;
        }

        inline void Remove(T remove)
        {
            T* temp = new T[arr_capacity--];
            if (!temp)
                THROW("Out of memory.");
            size_t rewind = 0;
            for (int i = 0; i <= arr_capacity; i++)
            {
                if (array[i] == remove)
                {
                    rewind++;
                    continue;
                }
                temp[i - rewind] = array[i];
            }
            delete[] array;
            array = temp;
            arr_size -= rewind;
        }

        inline size_t GetCapacity() const
        {
            return arr_capacity;
        }

        inline size_t GetSize() const
        {
            return arr_size;
        }

    };

}