// Construct different jet permutations for the BSM ttbar-like analysis.
//
// Created by Samvel Khalatyan, Aug 11, 2011
// Copyright 2011, All rights reserved

#include <stdexcept>

#include "bsm_stat/interface/Number.h"
#include "bsm_stat/interface/Permutation.h"
#include "interface/JetPermutation.h"

using namespace std;

using bsm::JetPermutation;

JetPermutation::JetPermutation():
    _flip_combination(false)
{
    _prime_generator.reset(new stat::PrimeNumber());
    _jets = 0;
}

void JetPermutation::init(const Jets &jets)
{
    _jets = &jets;

    _flip_combination = false;

    // Test if permutations exist
    //
    PermutationTables::const_iterator current_table =
        _permutation_tables.find(jets.size());
    if (_permutation_tables.end() == current_table)
    {
        // permutations do not exist: generate these
        //
        generatePermutations(jets.size());
    }
    else
    {
        _current_permutation = current_table->second.begin();
        _end_permutation = current_table->second.end();
    }
}

bool JetPermutation::next()
{
    if (_end_permutation == _current_permutation)
        return false;

    _flip_combination = !_flip_combination;
    if (_flip_combination)
        return true;

    return _end_permutation != ++_current_permutation;
}

JetPermutation::Jets JetPermutation::leptonicJets()
{
    if (!_jets)
        throw runtime_error("jet permutations are not initialized");

    if (_end_permutation == _current_permutation)
        return Jets();

    return jets(_flip_combination
            ? _current_permutation->right
            : _current_permutation->left);
}

JetPermutation::Jets JetPermutation::hadronicJets()
{
    if (!_jets)
        throw runtime_error("jet permutations are not initialized");

    if (_end_permutation == _current_permutation)
        return Jets();

    return jets(_flip_combination
            ? _current_permutation->left
            : _current_permutation->right);
}

// Private
//
JetPermutation::Jets JetPermutation::jets(uint64_t product)
{
    Jets jets;

    while(1 != product)
    {
        for(PrimeIDs::const_iterator prime = _prime_id.begin();
                _prime_id.end() != prime;
                ++prime)
        {
            if (0 == product % prime->first)
            {
                product /= prime->first;

                jets.push_back((*_jets)[prime->second]);

                break;
            }
        }
    }

    return jets;
}

void JetPermutation::generatePermutations(const uint32_t &size)
{
    expandPrimeIDs(size);

    // Prepare vector of primes for permutator
    //
    typedef vector<uint32_t> Primes;
    Primes primes;
    {
        uint32_t i = 0;
        for(PrimeIDs::const_iterator prime = _prime_id.begin();
                size > i;
                ++prime, ++i)
        {
            primes.push_back(prime->first);
        }
    }

    Permutations &permutations = _permutation_tables[size];

    // Generate all possible permutations
    //
    typedef stat::Permutation<uint32_t> PermutationUInt;
    typedef PermutationUInt::ObjectIterators ObjectIterators;

    for(uint32_t sub_size = 2; size >= sub_size; ++sub_size)
    {
        PermutationUInt permutator(primes, sub_size);
        do
        {
            // Get current permutation, move the saparator from left to right,
            // calculate the product of left and right prime numbers, store
            // in table if unique (combination is assumed to be the same if
            // left and right products are swapped: this swap should be taken
            // into account later when pair is used
            //
            const ObjectIterators &iterators = permutator.objectIterators();
            for(uint32_t separation = 1; sub_size > separation; ++separation)
            {
                // Calculate products
                //
                Combination combo;
                uint32_t i = 0;
                for(ObjectIterators::const_iterator iter = iterators.begin();
                        iterators.end() != iter;
                        ++iter, ++i)
                {
                    if (separation > i)
                        combo.left *= *(*iter);
                    else
                        combo.right *= *(*iter);
                }

                if (permutations.end() != find(permutations.begin(),
                            permutations.end(),
                            combo))
                    continue;

                permutations.push_back(combo);
            }
        }
        while(permutator.next());
    }

    _current_permutation = permutations.begin();
    _end_permutation = permutations.end();
}

void JetPermutation::expandPrimeIDs(const uint32_t &size)
{
    // Expand list of primary numbers
    //
    for(uint32_t i = _prime_id.size(); size > i; ++i)
    {
        _prime_id[*_prime_generator] = i;
        _prime_generator->next();
    }
}
