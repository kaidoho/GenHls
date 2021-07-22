#pragma once
#include <string>
#include <vector>
#include <algorithm>



using namespace std;

class SensorInterfaceProtocol {
 public:
  SensorInterfaceProtocol(const std::string& ProtocolType,
                          const std::string& Addressing)
      : ProtocolType_(ProtocolType),
        Addressing_(Addressing),
        SwitchVar_(""),
        SwitchVarValue_(""),
        switchPossible(false){};
  ~SensorInterfaceProtocol(){};
  std::string GetProtocolType(void) { return ProtocolType_; }
  std::string GetAddressing(void) { return Addressing_; }
  void SetDefaultInterface() { defaultInterface_ = true; };

  bool IsDefaultInterface() { return defaultInterface_; };
  bool operator==(const SensorInterfaceProtocol& rhs) const {
    if (0 == ProtocolType_.compare(rhs.ProtocolType_)) {
      return true;
    } else {
      return false;
    }
  }

  bool IsProtocolSwitchPossible() { return switchPossible; }

  void SetSwitchVar(std::string x) {
    SwitchVar_ = x;
    switchPossible = true;
  }
  void SetSwitchVarValue(std::string x) {
    SwitchVarValue_ = x;
    switchPossible = true;
  }

  std::string GetSwitchVar(void) { return SwitchVar_; }
  std::string GetSwitchVarValue(void) { return SwitchVarValue_; }

 private:
  std::string ProtocolType_;
  std::string Addressing_;
  std::string SwitchVar_;
  std::string SwitchVarValue_;
  bool switchPossible;

  bool defaultInterface_;
};

class SensorInterface {
 public:
  SensorInterface(const std::string& name, const std::string& type,
                  int32_t portOrDefaultBaudrate)
      : InterfaceName_(name),
        InterfaceType_(type),
        PortOrDefaultBaudRate_(portOrDefaultBaudrate),
        defaultInterface_(false){};

  ~SensorInterface(){};

  std::string GetInterfaceType(void) {
    if (0 == InterfaceType_.compare("Eth")) {
      return "ETHERNET_INTERFACE";
    }
    if (0 == InterfaceType_.compare("Serial")) {
      return "SERIAL_INTERFACE";
    }
    return "NOT_SUPPORTED";
  }

  int32_t GetPort(void) { return PortOrDefaultBaudRate_; };
  int32_t GetDefaultBaudrate(void) { return PortOrDefaultBaudRate_; };

  void SetDefaultInterface() { defaultInterface_ = true; };

  bool IsDefaultInterface() { return defaultInterface_; };
  std::string GetName(void) { return InterfaceName_; };

  void AddProtocol(SensorInterfaceProtocol* pProto) {
    protocols.push_back(pProto);
  };
  std::vector<SensorInterfaceProtocol*> GetProtocols() { return protocols; };

 private:
  std::string InterfaceName_;
  std::string InterfaceType_;

  std::vector<SensorInterfaceProtocol*> protocols;
  int32_t PortOrDefaultBaudRate_;
  bool defaultInterface_;
};