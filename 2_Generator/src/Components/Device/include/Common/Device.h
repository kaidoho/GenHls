#pragma once
#include <algorithm>
#include <string>
#include <vector>
#include <regex>
#include "Device/include/Common/SensorInterface.h"

using namespace std;



class Device {
 public:
  Device();
  ~Device(){};

  void SetDeviceName(const string& name);
  string GetDeviceName(void) { return name_; };
  void SetVersion(string version) {
    version_ = version;
    std::replace(version_.begin(), version_.end(), '.', '_');
  };

  string GetNameVersionCombined(void) { 
    string tmp = name_ + "_" + version_;
    char buf[200];
    regex re("[^(a-zA-Z0-9_)]"); 
    const char* start = tmp.c_str();


    string replacement("_");
    *std::regex_replace(&buf[0], start, &start[tmp.length()], re, replacement) =
        '\0';

    return string(buf); 
  }


  string GetSkeletonName(void) { 
    return GetNameVersionCombined() + "_Skeleton";
  }

  void AddInterface(SensorInterface* interface);
  vector<SensorInterface*> GetInterfaces(void) { return sensorInterfaces_; };
  bool SetDefaultInterface(std::string name);

  bool RebootGiven() { return (0 != rebootFunction_.size()); }
  bool StoreParamGiven() { return (0 != storeFunction_.size()); }
  bool FactoryResetGiven() { return (0 != factoryDefaultsFunction_.size()); }

  void SetRebootFuncName(string x, uint32_t time) {
    rebootFunction_ = x;
    rebootTime_ = time;
  }
  void SetStoreParamFuncName(string x, uint32_t time) {
    storeFunction_ = x;
    storeTime_ = time;
  }
  void SetFactoryDefaultFuncName(string x, uint32_t time) {
    factoryDefaultsFunction_ = x;
    factoryDefaultsTime_ = time;
  }
  string GetVersion() { return version_; };
  string GetRebootFuncName() { return rebootFunction_; }
  string GetStoreParamFuncName() { return storeFunction_; }
  string GetFactoryDefaultFuncName() { return factoryDefaultsFunction_; }

  uint32_t GetRebootFuncTime() { return rebootTime_; }
  uint32_t GetStoreParamFuncTime() { return storeTime_; }
  uint32_t GetFactoryDefaultFuncTime() { return factoryDefaultsTime_; }

 protected:
  string name_;
  string rebootFunction_;
  string storeFunction_;
  string factoryDefaultsFunction_;
  uint32_t rebootTime_;
  uint32_t storeTime_;
  uint32_t factoryDefaultsTime_;
  std::vector<SensorInterface*> sensorInterfaces_;
  string version_;
};