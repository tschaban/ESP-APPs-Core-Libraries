/* ESPAPP Firmware for smarthome devices, More info: https://ESPAPP.smartnydom.pl/ */

#ifndef _ESPAPP_HTTP_Server_h
#define _ESPAPP_HTTP_Server_h

#include <ESPAPP_Core.h>

#ifdef ESP32
#include <WebServer.h>
#else /* ESP8266 */
#include <ESP8266WebServer.h>
#endif // ESP32/ESP8266
//#include <WiFiClient.h>

class ESPAPP_HTTPServer {

private:
ESPAPP_Core *System;

public:
ESPAPP_HTTPServer(ESPAPP_Core *_System);
~ESPAPP_HTTPServer();

WebServer *HTTPServer;
ESPAPP_HTTP_REQUEST *HTTPRequest = new ESPAPP_HTTP_REQUEST;

String *outputStream = new String();



bool init(void);
void listener(void);
void readHTTPRequest(void);
bool pushHTMLResponse();




};

#endif // _ESPAPP_HTTP_Server_h
