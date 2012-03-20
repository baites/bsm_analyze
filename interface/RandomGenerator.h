
#ifndef BSM_RANDOM_GENERATOR
#define BSM_RANDOM_GENERATOR

#include <boost/shared_ptr.hpp>
#include <boost/thread/locks.hpp>

#include "bsm_core/interface/ID.h"
#include "bsm_core/interface/Object.h"
#include "interface/bsm_fwd.h"

#include <boost/random/uniform_real.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/variate_generator.hpp>

namespace bsm
{

template <
    typename Generator = boost::mt11213b, 
    typename Distribution = boost::uniform_real<float>
>
class RandomGenerator : public core::Object
{
public:

    typedef typename Distribution::input_type Type;  
    typedef boost::shared_ptr<Generator> GeneratorPtr;
    typedef typename boost::shared_ptr<Distribution> DistributionPtr;
    typedef typename boost::variate_generator<Generator, Distribution> Variable;
    typedef typename boost::shared_ptr<Variable> VariablePtr;

    RandomGenerator(Type min = Type(0), Type max = Type(1)) 
    {
        _generator.reset(new Generator());
        _distribution.reset(new Distribution(min, max));
        _variable.reset(new Variable(*_generator, *_distribution));
    }; 
 
    RandomGenerator(
        RandomGenerator<Generator, Distribution> const & object
    )  
    {
        _generator = object._generator;
        _distribution = object._distribution;
        _variable.reset(
            new boost::variate_generator<Generator, Distribution>(
                *_generator, *_distribution
            )
        );        
    }

    Type value()
    {
        boost::mutex::scoped_lock lock(_mutex);
        return (*_variable)();
    }

    virtual uint32_t id() const
    {
        return core::ID<RandomGenerator>::get();
    }

    virtual ObjectPtr clone() const
    {
        return ObjectPtr(new RandomGenerator(*this));
    }

    using Object::merge;

    virtual void print(std::ostream &) const {};

private:

    boost::mutex _mutex;
    GeneratorPtr _generator;
    DistributionPtr _distribution;
    VariablePtr _variable;

};

}

#endif

