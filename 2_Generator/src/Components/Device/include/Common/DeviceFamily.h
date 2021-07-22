#pragma once
#include <algorithm>
#include <string>
#include <vector>
#include "Device/include/Common/Device.h"
using namespace std;

enum DeviceType { COLA };

class DeviceFamily {
 public:
  DeviceFamily() { pDevBase_ = nullptr; };
  ~DeviceFamily() {
    if (nullptr != pDevBase_) {
      delete pDevBase_;
    }
  };

  void SetName(string name) { familyName_ = name; };
  void SetGroup(string name) { familyGroup_ = name; };
  void SetVersion(string version) {
    version_ = version;
    std::replace(version_.begin(), version_.end(), '.', '_');
  };
  void SetBehavior(string version) {
    behavior_ = version;

  };


  void SetFamilyType(const string& type) {
    if (0 == type.compare("CoLa")) {
      devType_ = COLA;
    }
  };

  DeviceType GetType(void) { return devType_;}

  string GetName() { return familyName_; };
  string GetGroup() { return familyGroup_; };
  string GetNameVersionCombined() {
    return pDevBase_->GetNameVersionCombined();
  }
  string GetVersion() { return version_; };
  string GetBehavior() { return behavior_; };
  void AddFamilyMember(string memberName) {
    familyMembers_.push_back(memberName);
  }
  vector<string> GetFamilyMembers() { return familyMembers_; }

  void SetParsedDevice(Device* pDev) {
    pDevBase_ = pDev;
    pDevBase_->SetVersion(version_);
  }

  Device* pDevBase_;
 private:
  string familyName_;
  string familyGroup_;
  string version_;
  string behavior_;
  vector<string> familyMembers_;
  DeviceType devType_;
  
};
