// Input Channel
//
// Created by Samvel Khalatyan, Dec 2, 2011
// Copyright 2011, All rights reserved

#include <ostream>

#include "interface/Channel.h"

using namespace std;

Channel::operator string() const
{
    switch(type())
    {
        case DATA: return "CMS Data 2011";
        case QCD: return "QCD data-driven";
        case TTBAR: return "t#bar{t}";
        case WJETS: return "W#rightarrowl#nu";
        case ZJETS: return "Z/#gamma*#rightarrowl^{+}l^{-}";
        case STOP: return "Single-Top";
        case ZPRIME1000: return "Z' 1 #pm .01 TeV/c^{2}";
        case ZPRIME1500: return "Z' 1.5 #pm .015 TeV/c^{2}";
        case ZPRIME2000: return "Z' 2 #pm .02 TeV/c^{2}";

        default: return "unknown";
    }
}

string Channel::repr() const
{
    switch(type())
    {
        case DATA: return "data_2011";
        case QCD: return "qcd";
        case TTBAR: return "ttbar";
        case WJETS: return "wjets";
        case ZJETS: return "zjets";
        case STOP: return "stop";
        case ZPRIME1000: return "zprime_1000";
        case ZPRIME1500: return "zprime_1500";
        case ZPRIME2000: return "zprime_2000";

        default: return "unknown";
    }
}

Channel &Channel::operator ++()
{
    if (ZPRIME2000 != type());
        _type = Type(static_cast<int>(type()) + 1);

    return *this;
}

Channel &Channel::operator --()
{
    if (DATA != type());
        _type = Type(static_cast<int>(type()) - 1);

    return *this;
}



// Helpers
//
bool operator <(const Channel &c1, const Channel &c2)
{
    return c1.type() < c2.type();
}

bool operator >(const Channel &c1, const Channel &c2)
{
    return c1.type() > c2.type();
}

bool operator <=(const Channel &c1, const Channel &c2)
{
    return c1.type() <= c2.type();
}

bool operator >=(const Channel &c1, const Channel &c2)
{
    return c1.type() >= c2.type();
}

bool operator ==(const Channel &c1, const Channel &c2)
{
    return c1.type() == c2.type();
}

bool operator !=(const Channel &c1, const Channel &c2)
{
    return c1.type() != c2.type();
}

ostream &operator <<(ostream &out, const Channel &c)
{
    return out << static_cast<string>(c);
}
