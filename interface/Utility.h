// Utility Classes/functions
//
// Useful tools that are heavily used in the analysis
//
// Created by Samvel Khalatyan, Apr 22, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_UTILITY
#define BSM_UTILITY

#include <ostream>
#include <functional>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "bsm_input/interface/bsm_input_fwd.h"
#include "interface/bsm_fwd.h"

class TLorentzVector;

namespace bsm
{
    class LorentzVector;

    class Summary
    {
        public:
            Summary(const uint32_t &files_total);

            uint64_t eventsProcessed() const
            {
                return _events_processed;
            }
            
            uint32_t filesTotal() const
            {
                return _files_total;
            }

            uint32_t filesProcessed() const
            {
                return _files_processed;
            }

            uint32_t averageEventSize() const
            {
                return eventsProcessed()
                    ? (_total_events_size / eventsProcessed())
                    : 0;
            }

            void addEventsProcessed(const uint32_t &events)
            {
                _events_processed += events;
            }

            void addFilesProcessed();

            void addEventsSize(const uint32_t &size)
            {
                _total_events_size += size;
            }

        private:
            uint64_t _events_processed;
            const uint32_t _files_total;
            uint32_t _files_processed;
            uint64_t _total_events_size;
            uint32_t _percent_done;
    };

    std::ostream &operator <<(std::ostream &, const Summary &);

    namespace utility
    {
        class SupressTHistAddDirectory
        {
            public:
                SupressTHistAddDirectory();
                ~SupressTHistAddDirectory();

            private:
                bool _flag;
        };

        void set(TLorentzVector *root_p4, const LorentzVector *bsm_p4);
    }

    template<typename T>
        std::ostream &operator <<(std::ostream &, const std::equal_to<T> &);

    template<typename T>
        std::ostream &operator <<(std::ostream &, const std::greater<T> &);

    template<typename T>
        std::ostream &operator <<(std::ostream &, const std::greater_equal<T> &);

    template<typename T>
        std::ostream &operator <<(std::ostream &, const std::less<T> &);

    template<typename T>
        std::ostream &operator <<(std::ostream &, const std::less_equal<T> &);

    template<typename T>
        std::ostream &operator <<(std::ostream &, const std::logical_and<T> &);

    template<typename T>
        std::ostream &operator <<(std::ostream &, const std::logical_or<T> &);

    struct PtLess
    {
        public:
            typedef boost::shared_ptr<LorentzVector> P4Ptr;

            virtual bool operator()(const P4Ptr &v1, const P4Ptr &v2);
    };

    struct PtGreater
    {
        public:
            typedef boost::shared_ptr<LorentzVector> P4Ptr;

            bool operator()(const P4Ptr &v1, const P4Ptr &v2);
    };

    struct CorrectedPtLess
    {
        public:
            virtual bool operator()(const CorrectedJet &v1, const CorrectedJet &v2);

        private:
            PtLess _pt_less;
    };

    struct CorrectedPtGreater
    {
        public:
            virtual bool operator()(const CorrectedJet &v1, const CorrectedJet &v2);

        private:
            PtGreater _pt_greater;
    };

    struct PtGreaterSort
    {
        public:
            typedef std::vector<CorrectedJet>::const_iterator Iterator;

            bool operator()(const Iterator &v1, const Iterator &v2)
            {
                return _pt_greater(*v1, *v2);
            }

        private:
            CorrectedPtGreater _pt_greater;
    };
}

template<typename T>
    std::ostream &bsm::operator <<(std::ostream &out, const std::equal_to<T> &)
{
    return out << "==";
}

template<typename T>
    std::ostream &bsm::operator <<(std::ostream &out, const std::greater<T> &)
{
    return out << " >";
}

template<typename T>
    std::ostream &bsm::operator <<(std::ostream &out, const std::greater_equal<T> &)
{
    return out << ">=";
}

template<typename T>
    std::ostream &bsm::operator <<(std::ostream &out, const std::less<T> &)
{
    return out << " <";
}

template<typename T>
    std::ostream &bsm::operator <<(std::ostream &out, const std::less_equal<T> &)
{
    return out << "<=";
}

template<typename T>
    std::ostream &bsm::operator <<(std::ostream &out, const std::logical_and<T> &)
{
    return out << "&&";
}

template<typename T>
    std::ostream &bsm::operator <<(std::ostream &out, const std::logical_or<T> &)
{
    return out << "||";
}

#endif
