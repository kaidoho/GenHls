#include "Device/include/CoLa/CoLaMethod.h"


CoLaMethod::CoLaMethod(std::string name, std::string accessLvl, int32_t midx)
    : name_(name), accessLvl_(accessLvl), smIdx_(midx) {
  returnType_ = NULL;
  argType_ = NULL;
}

std::string CoLaMethod::getFinalAccessLevel() {
  std::string& level = accessLvl_;
  if (0 == level.compare("Run")) {
    return "LEVEL_RUN";
  }
  if (0 == level.compare("Operator")) {
    return "LEVEL_OPERATOR";
  }
  if (0 == level.compare("Maintenance")) {
    return "LEVEL_MAINTENANCE";
  }
  if (0 == level.compare("AuthorizedClient")) {
    return "LEVEL_AUTHORIZED_CLIENT";
  }
  if (0 == level.compare("Service")) {
    return "LEVEL_SERVICE";
  }
  if (0 == level.compare("SICKService")) {
      return "LEVEL_SICKSERVICE";
  }
  if (0 == level.compare("Production")) {
    return "LEVEL_PRODUCTION";
  }
  if (0 == level.compare("Developer")) {
    return "LEVEL_DEVELOPER";
  }
  return "LEVEL_INVALID";
}