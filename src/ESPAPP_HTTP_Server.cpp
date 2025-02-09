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

// Debug: Print all arguments
    int argsCount = this->HTTPServer->args();
    this->System->Msg->printBulletPoint(F("Total arguments: "));
    this->System->Msg->printValue(argsCount);
    Serial << endl;
    for (int i = 0; i < argsCount; i++) {
        Serial << this->HTTPServer->argName(i);
        this->System->Msg->printValue("=");
        Serial << this->HTTPServer->arg(i) << endl;
    }




  this->HTTPRequest->siteId = this->HTTPServer->hasArg(F("site")) ? this->HTTPServer->arg(F("site")).toInt() : ESP_APP_NONE;
  this->HTTPRequest->command = this->HTTPServer->hasArg(F("cmd")) ? this->HTTPServer->arg(F("cmd")).toInt() : ESP_APP_NONE;
  this->HTTPRequest->action = this->HTTPServer->hasArg(F("action")) ? this->HTTPServer->arg(F("action")).toInt() : ESP_APP_NONE;
  this->HTTPRequest->option = this->HTTPServer->hasArg(F("option")) ? this->HTTPServer->arg(F("option")).toInt() : ESP_APP_NONE;

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

  if (this->HTTPRequest->siteId == ESPAPP_HTTP_SITE_UPLOAD && this->HTTPRequest->command == ESPAPP_HTTP_COMMAND_UPLOAD)
  {
    if (this->HTTPServer->hasArg(F("directory")))
    {
      this->HTTPServer->arg(F("directory")).toCharArray(this->HTTPRequest->file->directory, this->HTTPServer->arg(F("directory")).length() + 1);
    }
    else
    {
      sprintf(this->HTTPRequest->file->directory, (const char *)F(ESP_APP_EMPTY_STRING));
    }

#ifdef DEBUG
    this->System->Msg->printBulletPoint(F("Directory: "));
    this->System->Msg->printValue(this->HTTPRequest->file->directory);

#endif
  }
}

bool ESPAPP_HTTPServer::processUploadFile()
{
  bool success = false;

  char uploadFileName[ESP_APP_FILE_MAX_FILE_NAME_LENGTH];

  HTTPUpload &upload = this->HTTPServer->upload();

  this->System->Msg->printError(F("Upload: STAT: "), F("UPLOAD"));
  this->System->Msg->printValue(upload.status);

  char directory[ESP_APP_FILE_MAX_DIRECTORY_NAME_LENGTH] = "cfg";

//strcpy_P(directory, (char *)pgm_read_dword(&(ESP_APP_DIRECTORIES[locationId])));

Serial << "##### = " << directory << endl;



  if (upload.status == UPLOAD_FILE_START)
  {
    this->System->Msg->printBulletPoint(F("Upload: File name: "));
    this->System->Msg->printValue(upload.filename);
    this->System->Msg->printBulletPoint(F("Upload: File size: "));
    this->System->Msg->printValue(upload.totalSize, F("B"));
    this->System->Msg->printBulletPoint(F("Upload: Status: "));
    this->System->Msg->printValue(upload.status);
    this->System->Msg->printBulletPoint(F("Upload: Current size: "));
    this->System->Msg->printValue(upload.currentSize, F("B"));
    this->System->Msg->printBulletPoint(F("Upload: Buffer size: "));
    this->System->Msg->printValue(upload.bufsize, F("B"));
    this->System->Msg->printBulletPoint(F("Upload: Total size: "));
    this->System->Msg->printValue(upload.totalSize, F("B"));
  }
  else if (upload.status == UPLOAD_FILE_WRITE)
  {
    this->System->Msg->printValue(F("."));
  }
  else if (upload.status == UPLOAD_FILE_END)
  {
    if (strlen(upload.filename) > 0)
    {
      this->System->Msg->printBulletPoint(F("Uploaded finished"));
      this->System->Msg->printBulletPoint(F("File size: "));
      this->System->Msg->printValue(upload.totalSize, F("B"));
      this->System->Msg->printBulletPoint(F("Saving in the file system"));

      success = this->System->Flash->uploadFile(directory, upload.filename,
                                                upload.buf, upload.totalSize);
    }
  }
  return success;


 upload.filename.toCharArray(uploadFileName, upload.filename.length() + 1);
  size_t fileSize = 0;
  std::vector<uint8_t> fileBuffer;

#ifdef DEBUG
  this->System->Msg->printBulletPoint(F("Upload: File name: "));
  this->System->Msg->printValue(uploadFileName);
  this->System->Msg->printBulletPoint(F("Upload: File size: "));
  this->System->Msg->printValue(upload.totalSize, F("B"));
  this->System->Msg->printBulletPoint(F("Upload: Status: "));
  this->System->Msg->printValue(upload.status);
  this->System->Msg->printBulletPoint(F("Upload: Current size: "));
  this->System->Msg->printValue(upload.currentSize, F("B"));
  this->System->Msg->printBulletPoint(F("Upload: Buffer size: "));
  this->System->Msg->printValue(fileBuffer.size(), F("B"));
  this->System->Msg->printBulletPoint(F("Upload: Total size: "));
  this->System->Msg->printValue(upload.totalSize, F("B"));

#endif

  if (upload.status == UPLOAD_FILE_START)
  {
    fileSize = 0;
    fileBuffer.clear();

#ifdef DEBUG
    this->System->Msg->printBulletPoint(F("Uploaded started: "));
#endif
  }
  else if (upload.status == UPLOAD_FILE_WRITE)
  {
    fileSize += upload.currentSize;
    fileBuffer.insert(fileBuffer.end(), upload.buf, upload.buf + upload.currentSize);
#ifdef DEBUG
    this->System->Msg->printValue(F("."));
#endif
  }
  else if (upload.status == UPLOAD_FILE_END)
  {
    if (strlen(uploadFileName) > 0)
    {
#ifdef DEBUG
      this->System->Msg->printBulletPoint(F("Uploaded finished"));
      this->System->Msg->printBulletPoint(F("File size: "));
      this->System->Msg->printValue(fileSize, F("B"));
      this->System->Msg->printBulletPoint(F("Saving in the file system"));
#endif



      success = this->System->Flash->uploadFile(this->HTTPRequest->file->directory, uploadFileName,
                                                fileBuffer.data(), fileSize);
    }
  }
  return success;
}