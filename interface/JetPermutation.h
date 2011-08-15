// Construct different jet permutations for the BSM ttbar-like analysis.
//
// Created by Samvel Khalatyan, Aug 11, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_JET_PERMUTATION
#define BSM_JET_PERMUTATION

#include <stdint.h>

#include <vector>
#include <map>

#include <boost/shared_ptr.hpp>

#include "bsm_input/interface/bsm_input_fwd.h"
#include "bsm_stat/interface/bsm_stat_fwd.h"

namespace bsm
{
    class JetPermutation
    {
        public:
            typedef std::vector<const Jet *> Jets;

            JetPermutation();

            // Initialize with array of jets
            //
            void init(const Jets &jets);

            // get permutted jets
            //
            bool next();

            Jets leptonicJets();
            Jets hadronicJets();

        private:
            struct Combination
            {
                Combination(): left(1), right(1)
                {
                }

                bool operator ==(const Combination &object)
                {
                    return (object.left == left
                            && object.right == right)

                        || (object.left == right
                            && object.right == left);
                }

                uint64_t left;  // leptonic
                uint64_t right; // hadronic
            };

            // map<PrimeNumber, vector ID>
            //
            typedef std::map<uint32_t, uint32_t> PrimeIDs;

            // collection of all permutations: prime number products
            //
            typedef std::vector<Combination> Permutations;

            // all possible permutations for different multiplicities
            //
            typedef std::map<uint32_t, Permutations> PermutationTables;



            // Convert product of prime numbers to jets
            //
            Jets jets(uint64_t product);

            void generatePermutations(const uint32_t &size);
            void expandPrimeIDs(const uint32_t &size);



            boost::shared_ptr<stat::PrimeNumber> _prime_generator;
            
            const Jets *_jets;

            PrimeIDs _prime_id;
            PermutationTables _permutation_tables;

            Permutations::const_iterator _current_permutation;
            Permutations::const_iterator _end_permutation;
            bool _flip_combination;
    };
}

#endif
