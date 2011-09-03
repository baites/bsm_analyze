// Test comparator copy, clone, cutflow
//
// Created by Samvel Khalatyan, Sep 03, 2011
// Copyright 2011, All rights reserved

#include <iostream>

#include <boost/pointer_cast.hpp>
#include <boost/shared_ptr.hpp>

#include "interface/Cut.h"
#include "interface/Utility.h"

using namespace std;
using namespace boost;
using namespace bsm;

int main(int argc, char *argv[])
{
    shared_ptr<Cut> cut1(new Comparator<>(10, "Original Comparator"));
    shared_ptr<Cut> cut1_copy1
        = dynamic_pointer_cast<Cut>(cut1->clone());
    cut1_copy1->setName("Cut1 Copy1");

    for(int i = 0; 50 > i; ++i)
    {
        cut1->apply(i);
        cut1_copy1->apply(i);
    }

    cout << *cut1 << endl;
    cout << *cut1_copy1 << endl;

    cout << "merge" << endl;
    cut1->merge(cut1_copy1);
    cout << *cut1 << endl;
}
