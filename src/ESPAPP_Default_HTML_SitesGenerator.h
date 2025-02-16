#ifndef _ESPAPP_Default_HTML_SitesGenerator_h
#define _ESPAPP_Default_HTML_SitesGenerator_h

#ifndef ESPAPP_CUSTOM_HTML_SITES_GENERATORA

#include <ESPAPP_Core.h>
#include <ESPAPP_Parameters_HTTP_Server.h>
#include <ESPAPP_HTML_UI.h>

class ESPAPP_HTML_SitesGenerator
{
private:
  ESPAPP_Core *System;
  ESPAPP_HTML_UI *UI;
  String *HTMLResponse = new String();
  ESPAPP_HTTP_REQUEST *HTTPRequest;
  void setHTTPResponseCode(int responseCode = ESPAPP_HTTP_RESPONSE_CODE_OK);
public:
  ESPAPP_HTML_SitesGenerator(ESPAPP_Core *_System, String *_HTMLResponse, ESPAPP_HTTP_REQUEST *_HTTPRequest);
  ~ESPAPP_HTML_SitesGenerator();
  bool processHTTPRequest(void);
};

#endif // ESPAPP_CUSTOM_HTML_SITES_GENERATOR
#endif // _ESPAPP_HTML_SitesGenerator_h