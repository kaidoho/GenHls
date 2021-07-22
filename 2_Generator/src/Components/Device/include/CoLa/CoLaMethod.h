#pragma once

#include <string>

#include "Device/include/CoLa/CoLaBaseTypes.h"
#include "Device/include/CoLa/CoLaVariable.h"

class CoLaMethod {
 public:
  CoLaMethod(std::string name, std::string accessLvl, int32_t midx);
  ~CoLaMethod(){};

  void setComName(std::string comName) { comName_ = comName; };

  std::string getName(void) { return name_; };

  std::string getAccessLvl(void) { return accessLvl_; };
  std::string getComName(void) { return comName_; };
  int32_t getIndex(void) { return smIdx_; };

  std::string getFinalAccessLevel();

  void SetReturnType(GenericCoLaTypeContainer* pT) { returnType_ = pT; };
  void SetArgType(GenericCoLaTypeContainer* pT) { argType_ = pT; };

  GenericCoLaTypeContainer* GetReturnType() { return returnType_; };
  GenericCoLaTypeContainer* GetArgType() { return argType_; };

 private:
  std::string name_;
  std::string accessLvl_;
  std::string comName_;
  int32_t smIdx_;

  GenericCoLaTypeContainer* returnType_;
  GenericCoLaTypeContainer* argType_;
};