#ifndef EXCEPTIONTRANSLATOR_H_
#define EXCEPTIONTRANSLATOR_H_

#include "nuitrack/types/Error.h"

namespace tdv
{
namespace nuitrack
{

class ExceptionTranslator
{
public:
	static void generateExceptionByErrorCode(ExceptionType errorCode)
	{
#ifndef NUITRACK_NO_EXCEPTIONS
		switch(errorCode)
		{
		case EXCEPTION:
			throw tdv::nuitrack::Exception();
		case TERMINATE_EXCEPTION:
			throw tdv::nuitrack::TerminateException();
		case BAD_CONFIG_VALUE_EXCEPTION:
			throw tdv::nuitrack::BadConfigValueException();
		case CONFIG_NOT_FOUND_EXCEPTION:
			throw tdv::nuitrack::ConfigNotFoundException();
		case MODUDLE_NOT_FOUND_EXCEPTION:
			throw tdv::nuitrack::ModuleNotFoundException();
		case LICENSE_NOT_ACQUIRED_EXCEPTION:
			throw tdv::nuitrack::LicenseNotAcquiredException();
		case MODULE_NOT_INITIALIZED_EXCEPTION:
			throw tdv::nuitrack::ModuleNotInitializedException();
		case MODULE_NOT_STARTED_EXCEPTION:
			throw tdv::nuitrack::ModuleNotStartedException();
		}
#else
		std::terminate();
#endif
	}
};

} /* namespace nuitrack */
} /* namespace tdv */

#endif /* EXCEPTIONTRANSLATOR_H_ */
