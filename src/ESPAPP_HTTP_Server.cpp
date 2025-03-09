#include "ESPAPP_HTTP_Server.h"
ESPAPP_HTTPServer::ESPAPP_HTTPServer(ESPAPP_Core *_System)
{
  this->HTTPServer = new WebServer(80);
  this->System = _System;
  this->outputStream->reserve(ESPAPP_HTTP_HTML_MAX_RESPONSE_SIZE);
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

void ESPAPP_HTTPServer::readHTTPRequestDirectoryName(char *path)
{
  if (this->HTTPServer->hasArg(FPSTR(ESPAPP_FORM_INPUT_NAME_DIRECTORY)))
  {
    sprintf(path, "%s%s", FPSTR(path_root), this->HTTPServer->arg(FPSTR(ESPAPP_FORM_INPUT_NAME_DIRECTORY)).c_str());
  }
  else
  {
    sprintf(path, "%s", FPSTR(path_root));
  }

#ifdef DEBUG
  this->System->Msg->printBulletPoint(F("Directory: "));
  this->System->Msg->printValue(path);
#endif
}

void ESPAPP_HTTPServer::readHTTPRequest(void)
{

  this->HTTPRequest->siteId = this->HTTPServer->hasArg(F("site")) ? this->HTTPServer->arg(F("site")).toInt() : ESPAPP_NONE;
  this->HTTPRequest->command = this->HTTPServer->hasArg(F("cmd")) ? this->HTTPServer->arg(F("cmd")).toInt() : ESPAPP_NONE;
  this->HTTPRequest->action = this->HTTPServer->hasArg(F("action")) ? this->HTTPServer->arg(F("action")).toInt() : ESPAPP_NONE;
  this->HTTPRequest->option = this->HTTPServer->hasArg(F("option")) ? this->HTTPServer->arg(F("option")).toInt() : ESPAPP_NONE;

  if (this->HTTPServer->hasArg(F("p1")))
  {
    this->HTTPServer->arg(F("p1")).toCharArray(this->HTTPRequest->parameter1, ESPAPP_HTTP_REQUEST_PARAMETER_1_MAX_LENGTH);
  }
  else
  {
    strcpy(this->HTTPRequest->parameter1, ESPAPP_EMPTY_STRING);
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

void ESPAPP_HTTPServer::processUploadFile(void)
{
  uploadFile = this->HTTPServer->upload();

  if (uploadFile.status == UPLOAD_FILE_START)
  {

#ifdef DEBUG
    this->System->Msg->printBulletPoint(F("Upload Started"));
    this->System->Msg->printBulletPoint(F("Buffer size: "));
    this->System->Msg->printValue(HTTP_UPLOAD_BUFLEN);
#endif

    fileSize = 0;
    fileBuffer.clear();
    fileExededSize = false;
    fileUploadSuccess = false;
  }
  else if (uploadFile.status == UPLOAD_FILE_WRITE)
  {

    if (!fileExededSize)
    {
      if (fileSize + uploadFile.currentSize > ESPAPP_FILE_MAX_SIZE)
      {
        fileExededSize = true;
      }
      else
      {
        fileSize += uploadFile.currentSize;
        fileBuffer.insert(fileBuffer.end(), uploadFile.buf, uploadFile.buf + uploadFile.currentSize);
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
  else if (uploadFile.status == UPLOAD_FILE_END)
  {
#ifdef DEBUG
    this->System->Msg->printBulletPoint(F("Uploaded finished"));
    this->System->Msg->printBulletPoint(F("Upload: File name: "));
    this->System->Msg->printValue(uploadFile.filename);
    this->System->Msg->printBulletPoint(F("Upload: Size: "));
    this->System->Msg->printValue(uploadFile.totalSize, F("B"));
#endif

    if (!fileExededSize)
    {
      fileUploadSuccess = true;
    }
#ifdef DEBUG
    else
    {
      this->System->Msg->printError(F("Max uploaded file size exceeded"), F("HTTP Server"));
      this->System->Msg->printValue((uint16_t)ESPAPP_FILE_MAX_SIZE / 1024);
      this->System->Msg->printValue(F("kB"));
    }
#endif
  }
}

bool ESPAPP_HTTPServer::fileUpladedSuccessfully(void)
{
  bool tmp = fileUploadSuccess;
  fileUploadSuccess = false;
  return tmp;
}

bool ESPAPP_HTTPServer::saveUploadedFile(void)
{
  bool success = false;
  char uploadFileName[ESPAPP_FILE_MAX_FILE_NAME_LENGTH];
  uploadFile.filename.toCharArray(uploadFileName, uploadFile.filename.length() + 1);

  char directory[ESPAPP_FILE_MAX_FILE_NAME_LENGTH];
  this->readHTTPRequestDirectoryName(directory);

#ifdef DEBUG
  this->System->Msg->printBulletPoint(F("Save to directory: "));
  this->System->Msg->printValue(directory);
#endif

  if (strlen(uploadFileName) > 0)
  {
#ifdef DEBUG
    this->System->Msg->printBulletPoint(F("Saving in the file system"));
#endif
    success = this->System->Flash->uploadFile(directory, uploadFileName,
                                              fileBuffer.data(), uploadFile.totalSize);
  }
#ifdef DEBUG
  else
  {
    this->System->Msg->printError(F("Empty file name"), F("HTTP Server"));
  }
#endif
  return success;
}

bool ESPAPP_HTTPServer::processFaviconRequest(void)
{
  return true;
}

bool ESPAPP_HTTPServer::processCSSFileRequest(void)
{
  bool success = true;
#ifdef DEBUG
  this->System->Msg->printInformation(F("Processing CSS file request"), F("HTTP Server"));
#endif

  char pathToCSSFile[strlen_P(path_data) + strlen_P(path_root) + ESPAPP_FILE_MAX_FILE_NAME_LENGTH];

  if (this->HTTPServer->hasArg(F("name")))
  {
    sprintf(pathToCSSFile, "%s%s%s", FPSTR(path_ui), FPSTR(path_root), this->HTTPServer->arg(F("name")).c_str());

#ifdef DEBUG
    this->System->Msg->printBulletPoint(F("CSS: "));
    this->System->Msg->printValue(pathToCSSFile);
#endif
  }
  else
  {
    success = false;
#ifdef DEBUG
    this->System->Msg->printError(F("CSS file name not provided"), F("HTTP Server"));
#endif
  }

  if (success && this->System->Flash->initialized())
  {
    File cssFile = this->System->Flash->fileSystem.open(pathToCSSFile, ESPAPP_OPEN_FILE_READING);
    if (cssFile)
    {
      this->HTTPServer->streamFile(cssFile, "text/css");
      cssFile.close();
      
     // if (gzip)
     // {
     //   this->HTTPServer->sendHeader(F("Content-Encoding"), F("gzip"));
     // }


      #ifdef DEBUG
      this->System->Msg->printBulletPoint(F("CSS file streamed successfully"));
#endif
    }
    else
    {
      success = false;
    }
  }

  if (!success)
  {
    this->HTTPServer->send(404, "text/plain", "File Not Found");
#ifdef DEBUG
    this->System->Msg->printError(F("CSS file not found"), F("HTTP Server"));
#endif
  }

  return success;
}

bool ESPAPP_HTTPServer::processJSFileRequest(void)
{
  bool success = true;
#ifdef DEBUG
  this->System->Msg->printInformation(F("Processing JS file request"), F("HTTP Server"));
#endif

  char pathToJSFile[strlen_P(path_data) + strlen_P(path_root) + ESPAPP_FILE_MAX_FILE_NAME_LENGTH];

  if (this->HTTPServer->hasArg(F("name")))
  {
    sprintf(pathToJSFile, "%s%s%s", FPSTR(path_ui), FPSTR(path_root), this->HTTPServer->arg(F("name")).c_str());

#ifdef DEBUG
    this->System->Msg->printBulletPoint(F("JS: "));
    this->System->Msg->printValue(pathToJSFile);
#endif
  }
  else
  {
    success = false;
#ifdef DEBUG
    this->System->Msg->printError(F("JS file name not provided"), F("HTTP Server"));
#endif
  }

  if (success && this->System->Flash->initialized())
  {
    File jsFile = this->System->Flash->fileSystem.open(pathToJSFile, ESPAPP_OPEN_FILE_READING);
    if (jsFile)
    {
      this->HTTPServer->streamFile(jsFile, "application/javascript");
      jsFile.close();
      
     // if (gzip)
     // {
     //   this->HTTPServer->sendHeader(F("Content-Encoding"), F("gzip"));
     // }


      #ifdef DEBUG
      this->System->Msg->printBulletPoint(F("JS file streamed successfully"));
#endif
    }
    else
    {
      success = false;
    }
  }

  if (!success)
  {
    this->HTTPServer->send(404, "text/plain", "File Not Found");
#ifdef DEBUG
    this->System->Msg->printError(F("JS file not found"), F("HTTP Server"));
#endif
  }

  return success;
}