#include "Device/include/CoLa/CoLaVariable.h"


CoLaVariable::CoLaVariable(std::string name, std::string rwDir,
                             std::string viewAccessLvl,
                             std::string writeAccessLvl, int32_t idx)
    : name_(name),
      rwDir_(rwDir),
      viewAccessLvl_(viewAccessLvl),
      writeAccessLvl_(writeAccessLvl),
      svIdx_(idx),
      seIdx_(-1),
      pType_(NULL),
      readOnly_(true) {
  if (rwDir.compare("Bidirectional") == 0) {
    readOnly_ = false;
  }
}

std::string CoLaVariable::getFinalAccessLevel(std::string& level) {
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