#include "ESPAPP_HTTP_Server_Container.h"

ESPAPP_HTTPServerContainer::ESPAPP_HTTPServerContainer(ESPAPP_Core *_System)
{
    this->System = _System;
    this->Server = new ESPAPP_HTTPServer(_System);
    this->Site = new ESPAPP_HTML_SitesGenerator(_System, this->Server, this->Server->outputStream);
};

ESPAPP_HTTPServerContainer::~ESPAPP_HTTPServerContainer() {
};

bool ESPAPP_HTTPServerContainer::init(void)
{
#ifdef DEBUG
    this->System->Msg->printInformation(F("Initializing HTTP Server"), F("HTTP Server"));
    this->System->Msg->printBulletPoint(F("Setting HTTP handlers"));
#endif

    onNotFound(std::bind(&ESPAPP_HTTPServerContainer::handleHTTPRequests, this));
    handle("/", std::bind(&ESPAPP_HTTPServerContainer::handleHTTPRequests, this));
    handle("/favicon.ico", std::bind(&ESPAPP_HTTPServerContainer::handleFavicon, this));
    handle("/upload", std::bind(&ESPAPP_HTTPServerContainer::handleHTTPRequests, this),
           std::bind(&ESPAPP_HTTPServerContainer::handleHTTPFileUpload, this));
    handle("/css", std::bind(&ESPAPP_HTTPServerContainer::handleCSS, this));
    handle("/js", std::bind(&ESPAPP_HTTPServerContainer::handleCSS, this));

    // handle("/log", handleDownloadLogFile);
    // handleFirmwareUpgrade("/upgrade", handleHTTPRequests, handleUpload);

    return this->Server->init();
}

void ESPAPP_HTTPServerContainer::handleHTTPRequests(void)
{

#ifdef DEBUG
    this->System->Msg->printHeader(1, 0, ESPAPP_MSG_HEADER_DEFAULT_LENGTH, ESPAPP_MSG_HEADER_TYPE_DASH);
    this->System->Msg->printInformation(F("New HTTP Request"), F("HTTP Server"));
#endif
    this->Server->readHTTPRequest();
    this->Site->processHTTPRequest();
    this->Server->pushHTMLResponse();
    this->handleCommand();

#ifdef DEBUG
    this->System->Msg->printHeader(1, 1, ESPAPP_MSG_HEADER_DEFAULT_LENGTH, ESPAPP_MSG_HEADER_TYPE_DASH);
#endif
}

void ESPAPP_HTTPServerContainer::handleFavicon(void)
{
    this->Server->processFaviconRequest();
}

void ESPAPP_HTTPServerContainer::handleHTTPFileUpload(void)
{
    this->Server->processUploadFile();
}

void ESPAPP_HTTPServerContainer::handleCSS(void)
{
    this->Server->processCSSFileRequest();
}

void ESPAPP_HTTPServerContainer::handleJS(void)
{
    this->Server->processJSFileRequest();
}

void ESPAPP_HTTPServerContainer::handleCommand(void)
{
    if (this->Server->HTTPRequest->siteId == ESPAPP_HTTP_SITE_REBOOT)
    {
        this->System->Events->triggerEvent(EVENT_REBOOT);           
    }
}

#ifndef ESP32 /* ESP82xx */
void ESPAPP_HTTPServer::handle(const char *uri,
                               ESP8266WebServer::THandlerFunction handler)
{
    this->Server->on(uri, handler);
}

void ESPAPP_HTTPServer::handleFirmwareUpgrade(
    const char *uri, ESP8266WebServer::THandlerFunction handlerUpgrade,
    ESP8266WebServer::THandlerFunction handlerUpload)
{
    this->Server->on(uri, HTTP_POST, handlerUpgrade, handlerUpload);
}

void ESPAPP_HTTPServer::onNotFound(ESP8266WebServer::THandlerFunction fn)
{
    this->Server->onNotFound(fn);
}
#else /* ESP32 */
void ESPAPP_HTTPServerContainer::handle(const char *uri,
                                        WebServer::THandlerFunction handler)
{
    this->Server->HTTPServer->on(uri, handler);
}

void ESPAPP_HTTPServerContainer::handle(const char *uri,
                                        WebServer::THandlerFunction handler,
                                        WebServer::THandlerFunction handlerUpload)
{
    this->Server->HTTPServer->on(uri, HTTP_POST, handler, handlerUpload);
}

void ESPAPP_HTTPServerContainer::onNotFound(WebServer::THandlerFunction fn)
{
    this->Server->HTTPServer->onNotFound(fn);
}

/*
void ESPAPP_HTTPServer::handleFirmwareUpgrade(
    const char *uri, WebServer::THandlerFunction handlerUpgrade,
    WebServer::THandlerFunction handlerUpload) {
  this->Server->on(uri, HTTP_POST, handlerUpgrade, handlerUpload);
}



*/
#endif