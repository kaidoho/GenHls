#include "Parser/include/Common/Parser.h"
#include "Parser/include/CoLa/CoLaDeviceParser.h"

#include "Logger/include/Logger.h"

Parser* createTypeParser(DeviceFamily* pDevFamily, const string& parseFile) {
  Parser* pRet = nullptr;
  DeviceType type = pDevFamily->GetType();

  switch (type) {
    case COLA:
      pRet = reinterpret_cast<Parser*>( new CoLaDeviceParser(parseFile) ); 
        break;
    default:
      LOG_ERROR("Can't create parser for this type");
      break;
  }
  return pRet;
}