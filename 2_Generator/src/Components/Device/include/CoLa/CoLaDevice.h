#pragma once
#include <string>
#include <vector>
#include <algorithm>

#include "Device/include/Common/Device.h"
#include "Device/include/CoLa/CoLaBaseTypes.h"
#include "Device/include/CoLa/CoLaMethod.h"
#include "Device/include/CoLa/CoLaVariable.h"

using namespace std;

class CoLaUserLevel {
 public:
  CoLaUserLevel(const string& lvlName, const string& lvlPassword)
      : lvlName_(lvlName), lvlPassword_(lvlPassword){};
  ~CoLaUserLevel(){};

  string getUserLvlName(void) {
    if (0 == lvlName_.compare("Run")) {
      return "LEVEL_RUN";
    }
    if (0 == lvlName_.compare("Operator")) {
      return "LEVEL_OPERATOR";
    }
    if (0 == lvlName_.compare("Maintenance")) {
      return "LEVEL_MAINTENANCE";
    }
    if (0 == lvlName_.compare("AuthorizedClient")) {
      return "LEVEL_AUTHORIZED_CLIENT";
    }
    if (0 == lvlName_.compare("Service")) {
      return "LEVEL_SERVICE";
    }
    if (0 == lvlName_.compare("SICKService")) {
      return "LEVEL_SICKSERVICE";
    }
    if (0 == lvlName_.compare("Production")) {
      return "LEVEL_PRODUCTION";
    }
    if (0 == lvlName_.compare("Developer")) {
      return "LEVEL_DEVELOPER";
    }
    return "LEVEL_INVALID";
  };

  string getUserLvlPassword(void) { return lvlPassword_; };

  void setUserLvlPassword(const string& lvlPassword)
  {
    lvlPassword_ = lvlPassword;
  }

  void getUserLvlPassword(uint32_t* pWordInt) {
    (*pWordInt) = strtoul(lvlPassword_.c_str(), 0, 16);
  };

 private:
  string lvlName_;
  string lvlPassword_;
};



class CoLaDevice : public Device {
 public:
  CoLaDevice();
  ~CoLaDevice(){};

  void AddUserLevel(CoLaUserLevel* lvl);

  vector<CoLaUserLevel*> GetUserLevels() { return userLvls_; }

  void AddType(GenericCoLaTypeContainer* pType);

  vector<GenericCoLaTypeContainer*> GetTypes() { return deviceDataTypes_; }

  GenericCoLaTypeContainer* GetTypeByBaseName(const std::string& typeName) {
    for (auto x : deviceDataTypes_) {
      if (0 == typeName.compare(x->GetBaseTypeName())) {
        return x;
      }
    }
    return NULL;
  }

  GenericCoLaTypeContainer* GetTypeByLocalName(const std::string& typeName) {
    for (auto x : deviceDataTypes_) {
      if (0 == typeName.compare(x->GetLocalName())) {
        return x;
      }
    }

    return NULL;
  }

  void AddVariable(CoLaVariable* pVar);
  void AddMethod(CoLaMethod* pMeth);
  vector<CoLaVariable*> GetVariables() { return CoLaVariables_; }
  vector<CoLaMethod*> GetMethods() { return CoLaMethods_; }

  vector<SensorInterfaceProtocol*> GetProtocols() {
    vector<SensorInterfaceProtocol*> protos;

    for (vector<SensorInterface*>::iterator it = sensorInterfaces_.begin();
         it != sensorInterfaces_.end(); ++it) {
      std::vector<SensorInterfaceProtocol*> Protos = (*it)->GetProtocols();
      for (vector<SensorInterfaceProtocol*>::iterator iit = Protos.begin();
           iit != Protos.end(); ++iit) {
        if (protos.size() == 0) {
          protos.push_back((*iit));
        } else {
          bool found = false;
          for (vector<SensorInterfaceProtocol*>::iterator iiit = protos.begin();
               iiit != protos.end(); ++iiit) {
            found = found | ((*iit) == (*iiit));
          }
          if (!found) {
            protos.push_back((*iit));
          }
        }
      }
    }
    return protos;
  }

 private:
  vector<GenericCoLaTypeContainer*> deviceDataTypes_;
  vector<CoLaUserLevel*>            userLvls_;
  vector<CoLaMethod*>               CoLaMethods_;
  vector<CoLaVariable*>             CoLaVariables_;
};


