#ifndef _ESPAPP_Default_HTML_SitesGenerator_h
#define _ESPAPP_Default_HTML_SitesGenerator_h

#include <ESPAPP_Core.h>
#include <ESPAPP_Parameters_HTTP_Server.h>
#include <ESPAPP_HTML_UI.h>
#include <ESPAPP_HTTP_Server.h>
#include <ESPAPP_FirmwareInstalator.h>

#ifdef ESPAPP_CUSTOM_HTML_SITES_GENERATOR
#include <ESPAPP_HTML_Custom_Sites_Generator.h>
#endif

class ESPAPP_HTML_SitesGenerator
{
private:
  ESPAPP_Core *System;
  ESPAPP_HTML_UI *UI;
  String *HTMLResponse;
  ESPAPP_HTTPServer *Server;
#ifndef ESPAPP_CUSTOM_HTML_SITES_GENERATOR
  ESPAPP_HTTP_REQUEST *urlParams = new ESPAPP_HTTP_REQUEST;
  void addMenu(String *site);
  void setSiteGlobalAttributes(String *site);
#endif

  void setHTTPResponseCode(int responseCode = ESPAPP_HTTP_RESPONSE_CODE_OK);
  void configureSite(void);
  void siteNotFound(String *site);

#ifdef ESPAPP_CUSTOM_HTML_SITES_GENERATOR
  ESPAPP_HTML_Custom_Sites_Generator *CustomSiteGenerator = nullptr;
#endif

public:
  ESPAPP_HTML_SitesGenerator(ESPAPP_Core *_System, ESPAPP_HTTPServer *_HTTPServer, String *_HTMLResponse);
  ~ESPAPP_HTML_SitesGenerator();
  bool processHTTPRequest(void);
};

#endif // _ESPAPP_HTML_SitesGenerator_h