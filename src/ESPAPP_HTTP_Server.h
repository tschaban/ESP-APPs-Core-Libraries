/* ESPAPP Firmware for smarthome devices, More info: https://ESPAPP.smartnydom.pl/ */

#ifndef _ESPAPP_HTTP_Server_h
#define _ESPAPP_HTTP_Server_h

#include <ESPAPP_Core.h>

#ifdef ESP32
#include <WebServer.h>
#else /* ESP8266 */
#include <ESP8266WebServer.h>
#endif // ESP32/ESP8266
// #include <WiFiClient.h>


class ESPAPP_HTTPServer
{

private:
    ESPAPP_Core *System;
    
    HTTPUpload uploadFile;
    size_t fileSize = 0;
    std::vector<uint8_t> fileBuffer;
    bool fileExededSize = false;
    bool fileUploadSuccess = false;
    

public:
    ESPAPP_HTTPServer(ESPAPP_Core *_System);
    ~ESPAPP_HTTPServer();

    WebServer *HTTPServer;
    ESPAPP_HTTP_REQUEST *HTTPRequest = new ESPAPP_HTTP_REQUEST;

    String *outputStream = new String();

    bool init(void);
    void listener(void);
    void readHTTPRequest(void);
    void readDirectoryFromHTTPRequestParameter1(char *path);
    bool pushHTMLResponse();    
    
    void processUploadFile(void);
    bool fileUpladedSuccessfully(void);
    bool saveUploadedFile(void);
    bool downloadFile(const char *directory, const char *filename);
    
    bool processCSSFileRequest(void);
    bool processJSFileRequest(void);
    bool processFaviconRequest(void);

};

#endif // _ESPAPP_HTTP_Server_h
