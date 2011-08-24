// Create Several Range Cuts and apply them to numbers 1..10
//
// Created by Samvel Khalatyan, Aug 24, 2011
// Copyright 2011, All rights reserved

#include <functional>
#include <iomanip>
#include <iostream>

#include <boost/shared_ptr.hpp>

#include "interface/Cut.h"
#include "interface/Utility.h"

using namespace boost;
using namespace bsm;
using namespace std;

int main(int argc, char *argv[])
{
    shared_ptr<Cut> range1(new RangeComparator<>(2, 6, "Exclusive Range"));
    shared_ptr<Cut> range2(new RangeComparator<less<float>,
            greater<float> >(2, 6, "Outside Range"));
    shared_ptr<Cut> range3(new RangeComparator<less<float>,
            greater<float> >(6, 2, "Inverse Exclusive"));
    shared_ptr<Cut> range4(new RangeComparator<greater_equal<float>,
            less_equal<float> >(2, 6, "Inclusive"));

    shared_ptr<Cut> range5(new RangeComparator<less<float>,
            greater<float>,
            logical_or<bool> >(2, 6, "Exclusive Outside (OR)"));

    shared_ptr<Cut> range6(new RangeComparator<less_equal<float>,
            greater_equal<float>,
            logical_or<bool> >(2, 6, "Inclusive Outside (OR)"));

    for(uint32_t i = 0; 11 > i; ++i)
    {
        cout << setw(2) << i << ": "
            << (range1->apply(i) ? "o" : "x")
            << " " 
            << (range2->apply(i) ? "o" : "x")
            << " " 
            << (range3->apply(i) ? "o" : "x")
            << " " 
            << (range4->apply(i) ? "o" : "x")
            << " " 
            << (range5->apply(i) ? "o" : "x")
            << " " 
            << (range6->apply(i) ? "o" : "x")
            << endl;
    }

    cout << *range1 << endl;
    cout << *range2 << endl;
    cout << *range3 << endl;
    cout << *range4 << endl;
    cout << *range5 << endl;
    cout << *range6 << endl;

    return 1;
}
