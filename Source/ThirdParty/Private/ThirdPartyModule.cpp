#include "ThirdParty.h"

#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"

class FThirdPartyModule : public IModuleInterface
{
public:
	/** IDK what this does, I stole it from VS Tools */
	virtual void StartupModule() override {}
	virtual void ShutdownModule() override {}
};

IMPLEMENT_MODULE(FThirdPartyModule, ThirdParty)
