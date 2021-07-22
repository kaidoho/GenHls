#include "Device/include/Common/Device.h"

//===========================================================================
//===========================================================================
Device::Device()
    : name_("Empty"),
      rebootFunction_(""),
      storeFunction_(""),
      factoryDefaultsFunction_(""){

      };
//===========================================================================
//===========================================================================
void Device::SetDeviceName(const std::string& name) { name_ = name; }

//===========================================================================
//===========================================================================
void Device::AddInterface(SensorInterface* interface) {
  sensorInterfaces_.push_back(interface);
}

//===========================================================================
//===========================================================================
bool Device::SetDefaultInterface(std::string name) {
  for (std::vector<SensorInterface*>::iterator it = sensorInterfaces_.begin();
       it != sensorInterfaces_.end(); ++it) {
    if (0 == name.compare((*it)->GetName())) {
      (*it)->SetDefaultInterface();
      return true;
    }
  }
  return false;
}
