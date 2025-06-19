#ifndef I_CLOSER_GENERATOR_H
#define I_CLOSER_GENERATOR_H

#include <functional>

class ICloser;

typedef std::function<ICloser*()> ICloserGenerator;

#endif 