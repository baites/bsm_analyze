// Input Channel
//
// Created by Samvel Khalatyan, Dec 2, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_CHANNEL
#define BSM_CHANNEL

#include <string>
#include <iosfwd>

class Channel
{
    public:
        enum Type
        {
            DATA = 0,
            QCD,
            STOP,
            ZJETS,
            WJETS,
            TTBAR,
            ZPRIME1000,
            ZPRIME1500,
            ZPRIME2000
        };

        Channel(const Type &type): _type(type)
        {
        }

        inline Type type() const
        {
            return _type;
        }

        operator std::string() const;

        std::string repr() const;

        Channel &operator ++();
        Channel &operator --();

    private:
        Type _type;
};

bool operator <(const Channel &, const Channel &);
bool operator >(const Channel &, const Channel &);

bool operator <=(const Channel &, const Channel &);
bool operator >=(const Channel &, const Channel &);

bool operator ==(const Channel &, const Channel &);
bool operator !=(const Channel &, const Channel &);

std::ostream &operator <<(std::ostream &out, const Channel &c);

#endif
