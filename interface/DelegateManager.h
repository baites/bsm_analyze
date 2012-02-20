// Common delegate manager
//
// Created by Samvel Khalatyan, Feb 17, 2012
// Copyright 2012, All rights reserved

#ifndef BSM_DELEGATE_MANAGER
#define BSM_DELEGATE_MANAGER

namespace bsm
{
    template<typename T>
    class DelegateManager
    {
        public:
            DelegateManager();
            virtual ~DelegateManager();

            template<typename Y>
                void setDelegate(Y *);

            void setDelegate(T *);

            template<typename Y>
                Y *delegate() const;

            T *delegate() const;

        private:
            T *_delegate;
    };
}

template<typename T>
bsm::DelegateManager<T>::DelegateManager()
{
    _delegate = 0;
}

template<typename T>
bsm::DelegateManager<T>::~DelegateManager()
{
}

template<typename T>
    template<typename Y>
    void bsm::DelegateManager<T>::setDelegate(Y *delegate)
{
    if (_delegate != delegate)
        _delegate = delegate;
}

template<typename T>
    void bsm::DelegateManager<T>::setDelegate(T *delegate)
{
    setDelegate<T>(delegate);
}

template<typename T>
    template<typename Y>
    Y *bsm::DelegateManager<T>::delegate() const
{
    return dynamic_cast<Y *>(_delegate);
}

template<typename T>
    T *bsm::DelegateManager<T>::delegate() const
{
    return delegate<T>();
}

#endif
