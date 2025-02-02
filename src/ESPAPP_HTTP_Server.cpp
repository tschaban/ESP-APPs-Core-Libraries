#include "ESPAPP_HTTP_Server.h"

ESPAPP_HTTPServer::ESPAPP_HTTPServer(ESPAPP_Core *_System)
{
  this->HTTPServer = new WebServer(80);
  this->System = _System;
  this->outputStream->reserve(ESP_APP_HTTP_HTML_MAX_RESPONSE_SIZE);
};

ESPAPP_HTTPServer::~ESPAPP_HTTPServer() {};

bool ESPAPP_HTTPServer::init(void)
{
  this->HTTPServer->begin(80);

#ifdef DEBUG
  this->System->Msg->printBulletPoint(F("HTTP Server is up&running"));
#endif

  return true;
}

void ESPAPP_HTTPServer::listener(void)
{
  this->HTTPServer->handleClient();
}

bool ESPAPP_HTTPServer::pushHTMLResponse()
{
#ifdef DEBUG
  this->System->Msg->printBulletPoint(F("Pushing HTML response: "));
  this->System->Msg->printValue(this->outputStream->length(), F("B"));
  this->System->Msg->printBulletPoint(F("Response HTTP code: "));
  this->System->Msg->printValue(HTTPRequest->HTTPResponseCode);
#endif
  this->HTTPServer->sendHeader(F("Cache-Control"), F("no-cache"));
  //this->HTTPServer->setContentLength(CONTENT_LENGTH_UNKNOWN);
  this->HTTPServer->sendHeader(F("Content-Length"), String(this->outputStream->length()));
  this->HTTPServer->setContentLength(this->outputStream->length());
  this->HTTPServer->send(HTTPRequest->HTTPResponseCode, F("text/html"), this->outputStream->c_str());

#ifdef DEBUG
  this->System->Msg->printBulletPoint(F("Site published"));
#endif
  return true;
}

void ESPAPP_HTTPServer::readHTTPRequest(void)
{

  HTTPRequest->siteId = this->HTTPServer->hasArg(F("site")) ? this->HTTPServer->arg(F("site")).toInt() : ESP_APP_NONE;
  HTTPRequest->command = this->HTTPServer->hasArg(F("cmd")) ? this->HTTPServer->arg(F("cmd")).toInt() : ESP_APP_NONE;
  HTTPRequest->action = this->HTTPServer->hasArg(F("action")) ? this->HTTPServer->arg(F("action")).toInt() : ESP_APP_NONE;
  HTTPRequest->option = this->HTTPServer->hasArg(F("option")) ? this->HTTPServer->arg(F("option")).toInt() : ESP_APP_NONE;

#ifdef DEBUG
  this->System->Msg->printBulletPoint(F("Site ID: "));
  this->System->Msg->printValue(HTTPRequest->siteId);
  this->System->Msg->printBulletPoint(F("Command: "));
  this->System->Msg->printValue(HTTPRequest->command);
  this->System->Msg->printBulletPoint(F("Action: "));
  this->System->Msg->printValue(HTTPRequest->action);
  this->System->Msg->printBulletPoint(F("Option: "));
  this->System->Msg->printValue(HTTPRequest->option);

#endif
}



