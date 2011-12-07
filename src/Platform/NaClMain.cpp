#include <cassert>
#include "../NxnaConfig.h"

#ifdef NXNA_PLATFORM_NACL

#include <ppapi/cpp/instance.h>
#include <ppapi/cpp/module.h>


int NxnaMain(int argc, char* argv[]);

pp::Instance* g_Instance = nullptr;

class NxnaNaClInstance : public pp::Instance {
 public:
  explicit NxnaNaClInstance(PP_Instance instance) : pp::Instance(instance)
  {
	  NxnaMain(0, NULL);
  }
  virtual ~NxnaNaClInstance() {}

};

class NxnaNaClModule : public pp::Module {

 public:
  NxnaNaClModule() : pp::Module() {}
  virtual ~NxnaNaClModule() {}

  virtual pp::Instance* CreateInstance(PP_Instance instance) {
	assert(g_Instance == nullptr);
	g_Instance = new NxnaNaClInstance(instance);
	return g_Instance;
  }
};


namespace pp
{
	Module* CreateModule() {
		return new NxnaNaClModule();
	}
}

#endif
