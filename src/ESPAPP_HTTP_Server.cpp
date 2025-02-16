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
  this->System->Msg->printValue(this->HTTPRequest->HTTPResponseCode);
#endif
  this->HTTPServer->sendHeader(F("Cache-Control"), F("no-cache"));
  // this->HTTPServer->setContentLength(CONTENT_LENGTH_UNKNOWN);
  this->HTTPServer->sendHeader(F("Content-Length"), String(this->outputStream->length()));
  this->HTTPServer->setContentLength(this->outputStream->length());
  this->HTTPServer->send(this->HTTPRequest->HTTPResponseCode, F("text/html"), this->outputStream->c_str());

#ifdef DEBUG
  this->System->Msg->printBulletPoint(F("Site published"));
#endif
  return true;
}

void ESPAPP_HTTPServer::readHTTPRequest(void)
{

  this->HTTPRequest->siteId = this->HTTPServer->hasArg(F("site")) ? this->HTTPServer->arg(F("site")).toInt() : ESP_APP_NONE;
  this->HTTPRequest->command = this->HTTPServer->hasArg(F("cmd")) ? this->HTTPServer->arg(F("cmd")).toInt() : ESP_APP_NONE;
  this->HTTPRequest->action = this->HTTPServer->hasArg(F("action")) ? this->HTTPServer->arg(F("action")).toInt() : ESP_APP_NONE;
  this->HTTPRequest->option = this->HTTPServer->hasArg(F("option")) ? this->HTTPServer->arg(F("option")).toInt() : ESP_APP_NONE;
   
  if (this->HTTPServer->hasArg(F("p1"))) {
    this->HTTPServer->arg(F("p1")).toCharArray(this->HTTPRequest->parameter1, ESP_APP_HTTP_REQUEST_PARAMETER_1_MAX_LENGTH);
  } else {
    strcpy(this->HTTPRequest->parameter1, ESP_APP_EMPTY_STRING);
  }

#ifdef DEBUG
  this->System->Msg->printBulletPoint(F("Site ID: "));
  this->System->Msg->printValue(this->HTTPRequest->siteId);
  this->System->Msg->printBulletPoint(F("Command: "));
  this->System->Msg->printValue(this->HTTPRequest->command);
  this->System->Msg->printBulletPoint(F("Action: "));
  this->System->Msg->printValue(this->HTTPRequest->action);
  this->System->Msg->printBulletPoint(F("Option: "));
  this->System->Msg->printValue(this->HTTPRequest->option);
#endif
}

bool ESPAPP_HTTPServer::processUploadFile(uint8_t locationId)
{
  bool success = false;
  HTTPUpload &upload = this->HTTPServer->upload();

  static size_t fileSize = 0;
  static std::vector<uint8_t> fileBuffer;
  static bool fileExededSize = false;

  if (upload.status == UPLOAD_FILE_START)
  {

#ifdef DEBUG
    this->System->Msg->printBulletPoint(F("Upload Started"));
    this->System->Msg->printBulletPoint(F("Buffer size: "));
    this->System->Msg->printValue(HTTP_UPLOAD_BUFLEN);
#endif

    fileSize = 0;
    fileBuffer.clear();
    fileExededSize = false;
  }
  else if (upload.status == UPLOAD_FILE_WRITE)
  {

    if (!fileExededSize)
    {
      if (fileSize + upload.currentSize > ESP_APP_FILE_MAX_SIZE)
      {
        fileExededSize = true;
      }
      else
      {
        fileSize += upload.currentSize;
        fileBuffer.insert(fileBuffer.end(), upload.buf, upload.buf + upload.currentSize);
#ifdef DEBUG
        this->System->Msg->printValue(F("."));
#endif
      }
#ifdef DEBUG
    }
    else
    {
      this->System->Msg->printValue(F("!"));
#endif
    }
  }
  else if (upload.status == UPLOAD_FILE_END)
  {
    this->System->Msg->printBulletPoint(F("Uploaded finished"));

    if (!fileExededSize)
    {

      char uploadFileName[ESP_APP_FILE_MAX_FILE_NAME_LENGTH];
      upload.filename.toCharArray(uploadFileName, upload.filename.length() + 1);

      char directory[ESP_APP_FILE_MAX_FILE_NAME_LENGTH];
      strcpy_P(directory, (char *)pgm_read_dword(&(ESP_APP_DIRECTORIES[locationId])));

#ifdef DEBUG
      this->System->Msg->printBulletPoint(F("Upload: Size: "));
      this->System->Msg->printValue(upload.totalSize, F("B"));
      this->System->Msg->printBulletPoint(F("Upload: File name: "));
      this->System->Msg->printValue(uploadFileName);
      this->System->Msg->printBulletPoint(F("Save to directory: "));
      this->System->Msg->printValue(directory);
#endif

      if (strlen(uploadFileName) > 0)
      {
#ifdef DEBUG
        this->System->Msg->printBulletPoint(F("Saving in the file system"));
#endif
        success = this->System->Flash->uploadFile(directory, uploadFileName,
                                                  fileBuffer.data(), upload.totalSize);
      }
#ifdef DEBUG
      else
      {
        this->System->Msg->printError(F("Empty file name"), F("HTTP Server"));
      }
#endif
    }
#ifdef DEBUG
    else
    {
      this->System->Msg->printError(F("Max uploaded file size exceeded"), F("HTTP Server"));
      this->System->Msg->printValue((uint16_t)ESP_APP_FILE_MAX_SIZE / 1024);
      this->System->Msg->printValue(F("kB"));
    }
#endif
  }

  return success;
}