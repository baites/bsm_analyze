// Generic object cache
//
// Created by Samvel Khalatyan, Mar 25, 2011
// Copyright 2011, All rights reserved

#ifndef CACHE_H
#define CACHE_H

#include <stdexcept>

namespace bsm
{
    template<typename T>
    class Cache
    {
        public:
            Cache();

            bool is_valid() const;
            void invalidate();

            void set(const T &);

            // make sure cache is valid before extract its value otherwise
            // runtime_error is thrown
            //
            const T &get() const;

        private:
            T _value;
            bool _is_valid;
    };
}

template<typename T>
bsm::Cache<T>::Cache():
    _is_valid(false)
{
}

template<typename T>
bool bsm::Cache<T>::is_valid() const
{
    return _is_valid;
}

template<typename T>
void bsm::Cache<T>::invalidate()
{
    _is_valid = false;
}

template<typename T>
void bsm::Cache<T>::set(const T &new_value)
{
    _value = new_value;
    _is_valid = true;
}

template<typename T>
const T &bsm::Cache<T>::get() const
{
    if (is_valid())
        return _value;

    throw std::runtime_error("invalid cache");
}

#endif
