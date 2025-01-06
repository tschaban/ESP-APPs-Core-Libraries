#include "ESPAPP_API_Flash.h"

  ESPAPP_API_Flash::ESPAPP_API_Flash(){}
  bool ESPAPP_API_Flash::getJSON(const char *fileName, JsonDocument &doc) {

    const char* input = "{\"sensor\":\"gps\",\"time\":1351824120,\"data\":[48.756080,2.302038]}";
    deserializeJson(doc, input);
  };