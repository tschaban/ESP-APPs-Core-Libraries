#ifndef _ESPAPP_Default_HTML_SitesGenerator_h
#define _ESPAPP_Default_HTML_SitesGenerator_h

#ifndef ESPAPP_CUSTOM_HTML_SITES_GENERATORA

#include <ESPAPP_Core.h>
#include <ESPAPP_Parameters_HTTP_Server.h>
#include <ESPAPP_HTML_UI.h>
#include <ESPAPP_HTTP_Server.h>
#include <ESPAPP_FirmwareInstalator.h>


class ESPAPP_HTML_SitesGenerator
{
private:
  ESPAPP_Core *System;
  ESPAPP_HTML_UI *UI;
  String *HTMLResponse = new String();
  ESPAPP_HTTPServer *Server;
  void setHTTPResponseCode(int responseCode = ESPAPP_HTTP_RESPONSE_CODE_OK);
  void configureSite(void);

  void siteNotFound(String *site);

public:
  ESPAPP_HTML_SitesGenerator(ESPAPP_Core *_System, ESPAPP_HTTPServer *_HTTPServer, String *_HTMLResponse);
  ~ESPAPP_HTML_SitesGenerator();
  bool processHTTPRequest(void);
};

#endif // ESPAPP_CUSTOM_HTML_SITES_GENERATOR
#endif // _ESPAPP_HTML_SitesGenerator_h