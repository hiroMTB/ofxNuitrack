#ifndef NUITRACKMODULE_H_
#define NUITRACKMODULE_H_

#include <memory>
#include "Module.h"

struct NuitrackModule
{
	std::shared_ptr<tdv::nuitrack::Module> impl;
};

#endif /* NUITRACKMODULE_H_ */
