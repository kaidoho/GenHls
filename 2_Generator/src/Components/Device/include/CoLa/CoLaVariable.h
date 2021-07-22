#pragma once

#include <string>

#include "Device/include/CoLa/CoLaBaseTypes.h"

class CoLaVariable {
 public:
  CoLaVariable(std::string name, std::string rwDir, std::string viewAccessLvl,
                std::string writeAccessLvl, int32_t idx);
  ~CoLaVariable(){};

  bool providesEvent(void) {
    if (seIdx_ >= 0) {
      return true;
    } else {
      return false;
    }
  };
  bool isReadOnly(void) { return readOnly_; };

  void setComName(std::string comName) { comName_ = comName; };
  void setEventIndex(int32_t eidx) { seIdx_ = eidx; };

  std::string getName(void) { return name_; };

  std::string getViewAccessLvl(void) {
    return getFinalAccessLevel(viewAccessLvl_);
  };
  std::string getWriteAccessLvl(void) {
    return getFinalAccessLevel(writeAccessLvl_);
  };
  std::string getComName(void) { return comName_; };
  int32_t getVariableIndex(void) { return svIdx_; };
  int32_t getEventIndex(void) { return seIdx_; };

  void setType(GenericCoLaTypeContainer* pType) { pType_ = pType; };
  GenericCoLaTypeContainer* getType(void) { return pType_; };

 private:
  std::string getFinalAccessLevel(std::string& level);

  std::string name_;
  std::string rwDir_;
  std::string viewAccessLvl_;
  std::string writeAccessLvl_;
  std::string comName_;
  bool readOnly_;
  int32_t svIdx_;
  int32_t seIdx_;

  GenericCoLaTypeContainer* pType_;
};