#include "Device/include/CoLa/CoLaDevice.h"
#include "Logger/include/Logger.h"
//===========================================================================
//===========================================================================
CoLaDevice::CoLaDevice()
    : Device(){ 
    
  userLvls_.push_back(new CoLaUserLevel("Run", "0"));
  userLvls_.push_back(new CoLaUserLevel("Operator", "0"));
  userLvls_.push_back(new CoLaUserLevel("Maintenance", "0"));
  userLvls_.push_back(new CoLaUserLevel("AuthorizedClient", "0"));
  userLvls_.push_back(new CoLaUserLevel("Service", "0"));
  userLvls_.push_back(new CoLaUserLevel("SICKService", "0"));
  userLvls_.push_back(new CoLaUserLevel("Production", "0"));
  userLvls_.push_back(new CoLaUserLevel("Developer", "0"));

};

//===========================================================================
//===========================================================================
void CoLaDevice::AddUserLevel(CoLaUserLevel* lvl) {
  for (vector<CoLaUserLevel*>::iterator it = userLvls_.begin();
    it != userLvls_.end();  ++it) {
    if (0 == lvl->getUserLvlName().compare((*it)->getUserLvlName()))
    {
      (*it)-> setUserLvlPassword(lvl->getUserLvlPassword());
      return;
    }
  }
  userLvls_.push_back(lvl);
}

//===========================================================================
//===========================================================================
void CoLaDevice::AddType(GenericCoLaTypeContainer* pType) {
  bool exists = false;
  for (vector<GenericCoLaTypeContainer*>::iterator it =
           deviceDataTypes_.begin();
       it != deviceDataTypes_.end(); ++it) {
    if (0 == ((*it)->GetLocalName()).compare(pType->GetLocalName())) {
      exists = true;
    }
  }

  if (!exists) {
    LOG_DEBUG("Insert %s %s", pType->GetLocalName().c_str(),
              pType->GetBaseTypeName().c_str());
    deviceDataTypes_.push_back(pType);
  } else {
    LOG_DEBUG("Cant insert type: %s" , pType->GetLocalName().c_str());
  }
}

//===========================================================================
//===========================================================================
void CoLaDevice::AddVariable(CoLaVariable* pVar) {
  CoLaVariables_.push_back(pVar);
}

//===========================================================================
//===========================================================================
void CoLaDevice::AddMethod(CoLaMethod* pMethod) {
  CoLaMethods_.push_back(pMethod);
}
