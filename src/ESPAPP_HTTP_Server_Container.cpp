#include "ESPAPP_HTTP_Server_Container.h"

ESPAPP_HTTPServerContainer::ESPAPP_HTTPServerContainer(ESPAPP_Core *_System)
{
    this->System = _System;
    this->Server = new ESPAPP_HTTPServer(_System);
    this->Site = new ESPAPP_HTML_SitesGenerator(_System, this->Server->outputStream, this->Server->HTTPRequest);
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

    // handle("/log", handleDownloadLogFile);
    // handleFirmwareUpgrade("/upgrade", handleHTTPRequests, handleUpload);

    return this->Server->init();
}

void ESPAPP_HTTPServerContainer::handleHTTPRequests(void)
{

#ifdef DEBUG
    this->System->Msg->printHeader(1, 0, ESP_APP_MSG_HEADER_DEFAULT_LENGTH, ESP_APP_MSG_HEADER_TYPE_DASH);
    this->System->Msg->printInformation(F("New HTTP Request"), F("HTTP Server"));
#endif
    this->Server->readHTTPRequest();
    this->Site->processHTTPRequest();
    this->Server->pushHTMLResponse();

#ifdef DEBUG
    this->System->Msg->printHeader(1, 1, ESP_APP_MSG_HEADER_DEFAULT_LENGTH, ESP_APP_MSG_HEADER_TYPE_DASH);
#endif
}

void ESPAPP_HTTPServerContainer::handleFavicon(void)
{
    /* Do nothing */
}

void ESPAPP_HTTPServerContainer::handleHTTPFileUpload(void)
{

    this->Server->processUploadFile(1);
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