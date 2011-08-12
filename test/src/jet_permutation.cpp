// Test Jet Permutation class
//
// Created by Samvel Khalatyan, Aug 12, 2011
// Copyright 2011, All rights reserved

#include <time.h>

#include <iostream>
#include <stdexcept>

#include <boost/lexical_cast.hpp>

#include "interface/JetPermutation.h"

using namespace std;
using namespace boost;

using bsm::JetPermutation;

typedef JetPermutation::Jets Jets;

Jets jets;

namespace bsm
{
    class Jet
    {
        public:
            explicit Jet(const int &id): _id(id)
            {
            }

            void setId(const int &id)
            {
                _id = id;
            }

            int id() const
            {
                return _id;
            }
        private:
            int _id;
    };
}

using bsm::Jet;

ostream &operator<<(ostream &out, const Jet &jet)
{
    return out << jet.id();
}

ostream &operator<<(ostream &out, const Jets &jets)
{
    for(Jets::const_iterator jet = jets.begin();
            jets.end() != jet;
            ++jet)
    {
        out << *(*jet) << " ";
    }

    return out;
}

// This method is used for debugging
//
void printJets()
{
    if (jets.empty())
        cout << "jets are not set" << endl;
    else
        cout << jets << endl;
}

int main(int argc, char *argv[])
{
    if (2 > argc)
    {
        cerr << "usage: " << argv[0] << " jets" << endl;

        return 0;
    }

    const uint32_t number_of_jets = lexical_cast<uint32_t>(argv[1]);

    for(uint32_t i = 0; number_of_jets > i; ++i)
    {
        ::jets.push_back(new Jet(i + 1));
    }

    printJets();

    JetPermutation permutation;

    clock_t start, end;
    start = clock();
    permutation.init(::jets);

    uint32_t total_permutations = 0;

    do
    {
        ++total_permutations;
        Jets leptonic = permutation.leptonicJets();
        Jets hadronic = permutation.hadronicJets();
        //cout << "Leptonic: " << leptonic << endl;
        //cout << "Hadronic: " << hadronic << endl;
        //cout << endl;
    }
    while(permutation.next());

    end = clock();

    cout << total_permutations << " total permutations were generated" << endl;
    cout << "it took " << double(end - start) / CLOCKS_PER_SEC << " seconds" << endl;

    cout << endl;
    cout << "reuse tables" << endl;
    start = clock();
    permutation.init(::jets);

    total_permutations = 0;
    do
    {
        ++total_permutations;
        Jets leptonic = permutation.leptonicJets();
        Jets hadronic = permutation.hadronicJets();
    }
    while(permutation.next());
    end = clock();

    cout << total_permutations << " total permutations were generated" << endl;
    cout << "it took " << double(end - start) / CLOCKS_PER_SEC << " seconds" << endl;

    for(Jets::const_iterator jet = ::jets.begin();
            ::jets.end() != jet;
            ++jet)
    {
        delete *jet;
    }

    return 1;
}
