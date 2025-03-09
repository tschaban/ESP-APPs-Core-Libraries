#ifndef _ESPAPP_HTTP_Server_Container_h
#define _ESPAPP_HTTP_Server_Container_h

#include <ESPAPP_Core.h>
#include <ESPAPP_HTTP_Server.h>


#ifdef ESPAPP_CUSTOM_HTML_SITES_GENERATORA
#include <ESPAPP_HTML_SitesGenerator.h>
#else
#include <ESPAPP_Default_HTML_SitesGenerator.h>
#endif

class ESPAPP_HTTPServerContainer
{
private:
    ESPAPP_Core *System;
    ESPAPP_HTML_UI *UI;
    ESPAPP_HTML_SitesGenerator *Site;

    void handleHTTPRequests(void);
    void handleHTTPFileUpload(void);
    void handleFavicon(void);
    void handleCSS(void);
    void handleJS(void);
    void handleCommand(void);

    
#ifdef ESP32
    void onNotFound(WebServer::THandlerFunction fn);
    void handle(const char *uri, WebServer::THandlerFunction handler);
    void handle(const char *uri, WebServer::THandlerFunction handler, WebServer::THandlerFunction handlerUpload);    
    // void handleFirmwareUpgrade(const char *uri,  WebServer::THandlerFunction handlerUpgrade,  WebServer::THandlerFunction handlerUpload);
#else // ESP8266
    void onNotFound(ESP8266WebServer::THandlerFunction fn);
    void handle(const char *uri, ESP8266WebServer::THandlerFunction handler);
    void handleFirmwareUpgrade(const char *uri,
                               ESP8266WebServer::THandlerFunction handlerUpgrade,
                               ESP8266WebServer::THandlerFunction handlerUpload);

#endif // ESP32/ESP8266

public:
    ESPAPP_HTTPServerContainer(ESPAPP_Core *_System);
    ~ESPAPP_HTTPServerContainer();

    ESPAPP_HTTPServer *Server;
    bool init(void);
};

#endif // _ESPAPP_HTTP_Server_Container_h