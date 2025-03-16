#include "ESPAPP_Default_HTML_SitesGenerator.h"

#ifdef ESPAPP_CUSTOM_HTML_SITES_GENERATORA

ESPAPP_HTML_SitesGenerator::ESPAPP_HTML_SitesGenerator(ESPAPP_Core *_System, ESPAPP_HTTPServer *_HTTPServer, String *_HTMLResponse)
{
    this->System = _System;
    this->HTMLResponse = _HTMLResponse;
    this->Server = _HTTPServer;
    /** While initialized site gets it default sceleton */
    this->UI = new ESPAPP_HTML_UI(_System);
};

ESPAPP_HTML_SitesGenerator::~ESPAPP_HTML_SitesGenerator() {};

bool ESPAPP_HTML_SitesGenerator::processHTTPRequest(void)
{
#
    return true;
}

void ESPAPP_HTML_SitesGenerator::setHTTPResponseCode(int responseCode)
{
    this->Server->HTTPRequest->HTTPResponseCode = responseCode;
}

void ESPAPP_HTML_SitesGenerator::configureSite(void)
{
    switch (this->Server->HTTPRequest->siteId)
    {
    case ESPAPP_HTTP_SITE_REBOOT:
    {
        this->UI->removeMenu();
        break;
    }
    }
}

#endif