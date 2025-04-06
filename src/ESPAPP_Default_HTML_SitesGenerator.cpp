#include "ESPAPP_Default_HTML_SitesGenerator.h"

ESPAPP_HTML_SitesGenerator::ESPAPP_HTML_SitesGenerator(ESPAPP_Core *_System, ESPAPP_HTTPServer *_HTTPServer, String *_HTMLResponse)
{
    this->System = _System;
    this->HTMLResponse = _HTMLResponse;
    this->Server = _HTTPServer;
    /** While initialized site gets it default sceleton */
    this->UI = new ESPAPP_HTML_UI(_System);

#ifdef ESPAPP_CUSTOM_HTML_SITES_GENERATOR
    this->CustomSiteGenerator = new ESPAPP_HTML_Custom_Sites_Generator(_System, this->Server, this->UI);
#endif
};

ESPAPP_HTML_SitesGenerator::~ESPAPP_HTML_SitesGenerator() {};

bool ESPAPP_HTML_SitesGenerator::processHTTPRequest(void)
{
#ifdef DEBUG
    this->System->Msg->printBulletPoint(F("Generating site: started"));
#endif

    bool success = false;
    this->HTMLResponse->clear();

    /** Menu */
    if (this->System->getOperatingMode() == ESPAPP_OPERATING_MODE::FIRST_TIME_BOOT)
    {
        this->HTMLResponse->concat(FPSTR(HTML_UI_SITE_HEADER_LIGHT));
        switch (this->Server->HTTPRequest->siteId)
        {
        case ESPAPP_HTTP_SITE_WIFI_CONFIGURATION:
        {
#ifdef ESPAPP_CUSTOM_HTML_SITES_GENERATOR
            ESPAPP_HTTP_REQUEST *urlParams = new ESPAPP_HTTP_REQUEST;
            this->UI->setUrlParams(urlParams, ESPAPP_HTTP_SITE_REBOOT);
            this->UI->startForm(this->HTMLResponse, urlParams, (PGM_P)FPSTR(HTML_UI_EMPTY_STRING));
#else
            this->UI->setUrlParams(this->urlParams, ESPAPP_HTTP_SITE_REBOOT);
            this->UI->startForm(this->HTMLResponse, this->urlParams, (PGM_P)FPSTR(HTML_UI_EMPTY_STRING));
#endif

            this->UI->addParagraph(this->HTMLResponse, F("WiFi Configuration"));
            this->UI->addSelectFormItemOpen(this->HTMLResponse, FPSTR(HTML_UI_FORM_INPUT_COMMON_0), F(""));

#ifdef DEBUG
            this->System->Msg->printBulletPoint(F("Scanning for WiFi networks"));
#endif

            int8_t numberOfNetworks = WiFi.scanNetworks();
            char _ssid[33];
#ifdef DEBUG
            this->System->Msg->printBulletPoint(F("Number of networks: "));
            this->System->Msg->printValue(numberOfNetworks);
#endif

            if (numberOfNetworks > 0)
            {
                for (int i = 0; i < numberOfNetworks; i++)
                {
#ifdef DEBUG
                    Serial << endl
                           << F(" - ") << WiFi.SSID(i);
#endif

                    WiFi.SSID(i).toCharArray(_ssid, sizeof(_ssid));
                    _ssid[strlen(_ssid) + 1] = '\0';
                    /*
                                    sprintf(_ssidLabel, "%s (%s: %s)", _ssid, L_WIFI_SIGNAL,
                                            WiFi.RSSI(i) >= -30
                                                ? L_WIFI_RSSI_30
                                            : WiFi.RSSI(i) >= -67
                                                ? L_WIFI_RSSI_67
                                            : WiFi.RSSI(i) >= -70
                                                ? L_WIFI_RSSI_70
                                            : WiFi.RSSI(i) >= -80 ? L_WIFI_RSSI_80
                                                                  : L_WIFI_RSSI_90);
                      */
                    this->UI->addSelectOptionFormItem(this->HTMLResponse, _ssid, _ssid, false);
                }
            }

            this->UI->addSelectFormItemClose(this->HTMLResponse, "");
            this->UI->addInputFormItem(this->HTMLResponse, FPSTR(HTML_UI_INPUT_TYPE_PASSWORD), FPSTR(HTML_UI_FORM_INPUT_COMMON_1), "Password", "");
            this->UI->endForm(this->HTMLResponse, FPSTR(HTML_UI_SUBMITT_BUTTON_SAVE));
            setHTTPResponseCode(ESPAPP_HTTP_RESPONSE_CODE_NOT_FOUND);
            break;
        }
        case ESPAPP_HTTP_SITE_REBOOT:
        {

            ESPAPP_NETWORK *configuration = new ESPAPP_NETWORK;
            success = this->System->File->read(configuration);
            char _redirectLink[ESPAPP_DEVICE_ID_LENGTH + 13]; // + http://.local

            if (success)
            {

                if (this->Server->HTTPServer->arg(FPSTR(HTML_UI_FORM_INPUT_COMMON_0)).length() > 0)
                {
                    this->Server->HTTPServer->arg(FPSTR(HTML_UI_FORM_INPUT_COMMON_0)).toCharArray(configuration->primary->ssid, sizeof(configuration->primary->ssid));
                }
                else
                {
                    configuration->primary->ssid[0] = ESPAPP_EMPTY_STRING_TERMINATED;
                }

                if (this->Server->HTTPServer->arg(FPSTR(HTML_UI_FORM_INPUT_COMMON_1)).length() > 0)
                {
                    this->Server->HTTPServer->arg(FPSTR(HTML_UI_FORM_INPUT_COMMON_1)).toCharArray(configuration->primary->password, sizeof(configuration->primary->password));
                }
                else
                {
                    configuration->primary->password[0] = ESPAPP_EMPTY_STRING_TERMINATED;
                }

                this->System->File->save(configuration);
                this->System->setConnectionMode(ESPAPP_NETWORK_CONNECTION_MODE::CLIENT);
                this->System->setOperatingMode(ESPAPP_OPERATING_MODE::CONFIGURATION);
            }

            delete configuration;

            this->UI->openSection(this->HTMLResponse, F("Rebooting"), F("Rebooting device"));
            if (success)
            {

                char _redirectLink[ESPAPP_DEVICE_ID_LENGTH + 13]; // + http://.local
                this->System->getDeviceID(_redirectLink, true);
                sprintf(_redirectLink, "http://%s.local", _redirectLink);

                this->UI->addParagraph(this->HTMLResponse, F("Configuration saved"));
                this->UI->addParagraph(this->HTMLResponse, F("Connecting to the WiFi Router"));
                this->UI->addParagraph(this->HTMLResponse, F("If you are not redirected, please click <a href=\"{{v}}\">here</a>"));
                this->UI->replaceTagValue(this->HTMLResponse, _redirectLink);
            }
            else
            {

                char _redirectLink[2]; // + http://.local
                sprintf(_redirectLink, "%s", FPSTR(HTML_UI_SLASH_CHAR));

                this->UI->addParagraph(this->HTMLResponse, F("Something went wrong: configuration NOT saved"));
                this->UI->addParagraph(this->HTMLResponse, F("Restarting in Hotspot mode"));
            }

            this->UI->addParagraph(this->HTMLResponse, F("You have 10 seconds to streatch your legs"));
            this->UI->closeSection(this->HTMLResponse);

            this->UI->setRefresh(this->HTMLResponse, 10, _redirectLink);
            this->setHTTPResponseCode(ESPAPP_HTTP_RESPONSE_CODE_OK);

            this->System->Events->triggerEvent(EVENT_REBOOT);
            success = true;
            break;
        }
        default:
            this->siteNotFound(this->HTMLResponse);
            success = true;
            break;
        }
    }

    else
    {

        /** Formatting HTTP Request for File Explorer */
        if (this->Server->HTTPRequest->siteId == ESPAPP_HTTP_SITE_FILE_EXPLORER)
        {

            /** Setting Action  */
            if ((this->Server->HTTPServer->hasArg("a")))
            {
#ifdef DEBUG
                this->System->Msg->printBulletPoint(F("Action detected: "));
                this->System->Msg->printValue(this->Server->HTTPServer->arg("a").c_str());
#endif
                this->Server->HTTPRequest->action = this->Server->HTTPServer->arg("a").toInt();
            }

            /** Setting active directory */
            if (strlen(this->Server->HTTPRequest->parameter1) == 0)
            {
                this->Server->readDirectoryFromHTTPRequestParameter1(this->Server->HTTPRequest->parameter1);
            }
        }

        /** Generating site */
        this->configureSite();

        /** Header */
        this->UI->siteStart(this->HTMLResponse);

        this->UI->startNavigationBlock(this->HTMLResponse);

        /** Generating site MENU */
        if (this->UI->showMenu())
        {

#ifdef ESPAPP_CUSTOM_HTML_SITES_GENERATOR
            this->CustomSiteGenerator->addMenu(this->HTMLResponse);
#else
            this->addMenu(this->HTMLResponse);
#endif
        }

        this->UI->endNavigationBlock(this->HTMLResponse);

        //** Body */
        this->UI->startBodySection(this->HTMLResponse);

        this->UI->showSavedMessages(this->HTMLResponse, F(""), F(""));

        switch (this->Server->HTTPRequest->siteId)
        {
        case ESPAPP_HTTP_SITE_FILE_EXPLORER:
        {

            switch (this->Server->HTTPRequest->action)
            {

            case ESPAPP_HTTP_SITE_FILE_EXPLORER_COMMAND_UPLOAD:
                if (this->Server->fileUpladedSuccessfully())
                {
                    this->Server->saveUploadedFile();
                }
                break;
            case ESPAPP_HTTP_SITE_FILE_EXPLORER_ACTION_DOWNLOAD:
                if (this->Server->HTTPServer->hasArg(FPSTR(ESPAPP_FORM_TAG_FILENAME)))
                {
                    success = this->Server->downloadFile(this->Server->HTTPRequest->parameter1, this->Server->HTTPServer->arg(FPSTR(ESPAPP_FORM_TAG_FILENAME)).c_str());
                    if (success)
                    {
                        /** Not neet to process further, if file has been downloaded */
                        return true;
                    }
                }
                break;
            case ESPAPP_HTTP_SITE_FILE_EXPLORER_ACTION_VIEW:
                break;
            case ESPAPP_HTTP_SITE_FILE_EXPLORER_ACTION_DELETE:
                if (this->Server->HTTPServer->hasArg(FPSTR(ESPAPP_FORM_TAG_FILENAME)))
                {
                    this->System->Flash->deleteFile(this->Server->HTTPRequest->parameter1, this->Server->HTTPServer->arg(FPSTR(ESPAPP_FORM_TAG_FILENAME)).c_str());
                }
                break;
            case ESPAPP_HTTP_SITE_FILE_EXPLORER_ACTION_DELETE_FOLDER:
                if (strlen(this->Server->HTTPRequest->parameter1) > 0)
                {
                    this->System->Flash->deleteFolder(this->Server->HTTPRequest->parameter1);
                }
                break;
#ifdef DEBUG
            case ESPAPP_NONE:
                this->System->Msg->printBulletPoint(F("No action set"));
                break;
            default:
                this->System->Msg->printError(F("Action not implmented: "), F("File Explorer"));
                this->System->Msg->printValue(this->Server->HTTPRequest->action);
                break;
#endif
            }

            ESPAPP_FILE files[10];
            size_t count = 0;

            this->UI->openSection(this->HTMLResponse, F("Files explorer"), F("Files list"));
            this->UI->startFileExplorer(this->HTMLResponse);

            this->System->Flash->listFolders(this->Server->HTTPRequest->parameter1, files, 10, count);

            if (strlen(this->Server->HTTPRequest->parameter1) > 0 && strcmp(this->Server->HTTPRequest->parameter1, (PGM_P)FPSTR(path_root)) != 0)
            {
                this->UI->addFileExplorerFolderItem(this->HTMLResponse, (PGM_P)FPSTR(path_root), 0);
            }

            for (size_t i = 0; i < count; i++)
            {
                this->UI->addFileExplorerFolderItem(this->HTMLResponse, files[i].name, files[i].size);
            }

            count = 0;
            this->System->Flash->listFiles(this->Server->HTTPRequest->parameter1, files, 10, count);

            for (size_t i = 0; i < count; i++)
            {
                this->UI->addFileExplorerFileItem(this->HTMLResponse, files[i].name, files[i].size, this->Server->HTTPRequest->parameter1);
            }

            this->UI->endFileExplorer(this->HTMLResponse);

            this->UI->closeSection(this->HTMLResponse);

            this->UI->openSection(this->HTMLResponse, F("Upload file"), F("Upload file to the current folder"));

            this->UI->addFileExplorerUploadForm(this->HTMLResponse, this->Server->HTTPRequest->parameter1);

            this->UI->closeSection(this->HTMLResponse);

            setHTTPResponseCode(ESPAPP_HTTP_RESPONSE_CODE_OK);
            success = true;
            break;
        }
        case ESPAPP_HTTP_SITE_FIRMWARE_COMMAND: // Firmware commands
        {
            this->UI->openSection(this->HTMLResponse, F("Commands"), F("Firmware commands"));
            this->UI->addParagraph(this->HTMLResponse, F("<a href=\"/?site=101&cmd=10&action=2\">Reboot device (Client Mode)</a>"), true);       // Refactor it
            this->UI->addParagraph(this->HTMLResponse, F("<a href=\"/?site=101&cmd=10&action=1\">Reboot device (Access Point Mode)</a>"), true); // Refactor it
            this->UI->addParagraph(this->HTMLResponse, F("<a href=\"/?site=103\">Install UI Components</a>"), true);                             // Refactor it
            this->UI->closeSection(this->HTMLResponse);
            this->setHTTPResponseCode(ESPAPP_HTTP_RESPONSE_CODE_OK);
            success = true;
            break;
        }
        case ESPAPP_HTTP_SITE_REBOOT:
        {
            this->UI->openSection(this->HTMLResponse, F("Rebooting"), F("Rebooting device"));
            this->UI->addParagraph(this->HTMLResponse, F("Rebooting device..."));
            this->UI->addParagraph(this->HTMLResponse, F("You have 10 seconds to streatch your legs"));
            this->UI->addParagraph(this->HTMLResponse, F("If you are not redirected to the main page, please click <a href=\"/\">here</a>"));
            this->UI->closeSection(this->HTMLResponse);

            this->UI->setRefresh(this->HTMLResponse, 10, (PGM_P)FPSTR(HTML_UI_SLASH_CHAR));
            this->setHTTPResponseCode(ESPAPP_HTTP_RESPONSE_CODE_OK);

            this->System->setConnectionMode((ESPAPP_NETWORK_CONNECTION_MODE)this->Server->HTTPRequest->action);
            this->System->Events->triggerEvent(EVENT_REBOOT);
            success = true;
            break;
        }
        case ESPAPP_HTTP_SITE_SYSTEM_PARAMETERS:
        {
            this->UI->openSection(this->HTMLResponse, F("System"), F("System parameters"));
            char textBuffer[100];
            sprintf(textBuffer, "Free heap: %d", ESP.getFreeHeap());
            this->UI->addParagraph(this->HTMLResponse, textBuffer, true);
            sprintf(textBuffer, "Boot Mode: %d", (uint8_t)this->System->getOperatingMode());
            this->UI->addParagraph(this->HTMLResponse, textBuffer, true);
            sprintf(textBuffer, "Network Mode: %d", (uint8_t)this->System->getConnectionMode());
            this->UI->addParagraph(this->HTMLResponse, textBuffer, true);

            this->UI->closeSection(this->HTMLResponse);
            this->setHTTPResponseCode(ESPAPP_HTTP_RESPONSE_CODE_OK);
            success = true;
            break;
        }
        case ESPAPP_HTTP_SITE_DOWNLOAD_UI_COMPONENTS:
        {

#ifdef DEBUG
            this->System->Msg->printInformation(F("Download UI components event handled"), F("SYSTEM EVENT"));
#endif

            // ESPAPP_FirmwareInstalator *Instalator = new ESPAPP_FirmwareInstalator(System);
            // InstallationStats stats;
            // Instalator->install("https://api.smartnydom.pl/espapp/ui-configuration.json", stats);
            // Instalator->install("http://192.168.2.146:8080/static/covid/data.json", stats);
            // Instalator->install("http://files.smartnydom.pl/espapp/config/ui-configuration.json", stats);
            // delete Instalator;

            this->UI->openSection(this->HTMLResponse, F("UI Components"), F("Downloading UI components"));
            this->UI->addParagraph(this->HTMLResponse, F("Downloading UI components..."));
            this->UI->addParagraph(this->HTMLResponse, F("You have 10 seconds to streatch your legs"));
            this->UI->closeSection(this->HTMLResponse);
            this->setHTTPResponseCode(ESPAPP_HTTP_RESPONSE_CODE_OK);
            this->UI->setRefresh(this->HTMLResponse, 10, (PGM_P)F("/"));
            this->System->Events->triggerEvent(EVENT_DOWNLOAD_UI_COMPONENTS);
            success = true;
            break;
        }
#ifndef ESPAPP_CUSTOM_HTML_SITES_GENERATOR
        case ESPAPP_HTTP_SITE_TOPOGRAPHY:
        {

            this->UI->setUrlParams(this->urlParams, 1, ESPAPP_HTTP_SITE_TOPOGRAPHY);

            this->UI->openSection(this->HTMLResponse, F("UI Topography"), F("This is an example of UI elements"));
            this->UI->startForm(this->HTMLResponse, this->urlParams, (PGM_P)FPSTR(HTML_UI_EMPTY_STRING));

            this->UI->addInputFormItem(this->HTMLResponse, FPSTR(HTML_UI_INPUT_TYPE_TEXT), F("text1"), "Text filed: Simple", "Text");
            this->UI->addInputFormItem(this->HTMLResponse, FPSTR(HTML_UI_INPUT_TYPE_TEXT), F("text2"), "Text filed: Min", "Min 1 char", "30", "1");
            this->UI->addInputFormItem(this->HTMLResponse, FPSTR(HTML_UI_INPUT_TYPE_TEXT), F("text3"), "Text filed: Max", "Max 30 char", "30", (PGM_P)FPSTR(HTML_UI_INPUT_SKIP_ATTRIBUTE), "30");
            this->UI->addInputFormItem(this->HTMLResponse, FPSTR(HTML_UI_INPUT_TYPE_TEXT), F("text4"), "Text filed: Range", "Range: 20-30", "30", "20", "30");
            this->UI->addInputFormItem(this->HTMLResponse, FPSTR(HTML_UI_INPUT_TYPE_TEXT), F("text5"), "Text filed", "With custom hint", "30", (PGM_P)FPSTR(HTML_UI_INPUT_SKIP_ATTRIBUTE), (PGM_P)FPSTR(HTML_UI_INPUT_SKIP_ATTRIBUTE), (PGM_P)FPSTR(HTML_UI_INPUT_SKIP_ATTRIBUTE), "Custom");

            this->UI->addInputFormItem(this->HTMLResponse, FPSTR(HTML_UI_INPUT_TYPE_PASSWORD), F("password"), "Password", "1234567890", "16", (PGM_P)FPSTR(HTML_UI_INPUT_SKIP_ATTRIBUTE), "16");

            this->UI->addInputFormItem(this->HTMLResponse, FPSTR(HTML_UI_INPUT_TYPE_NUMBER), F("number"), "Number filed", "1000");

            this->UI->addCheckBoxFormItem(this->HTMLResponse, F("checkbox1"), "Checkbox 1", "1", false);
            this->UI->addCheckBoxFormItem(this->HTMLResponse, F("checkbox2"), "Checkbox 2", "1", true, (PGM_P)FPSTR(HTML_UI_EMPTY_STRING), true);
            this->UI->addCheckBoxFormItem(this->HTMLResponse, F("checkbox3"), "Checkbox 3", "1", true, "Hint text");

            this->UI->addRadioButtonFormItem(this->HTMLResponse, F("radio"), "Radio 1", "1", false);
            this->UI->addRadioButtonFormItem(this->HTMLResponse, F("radio"), "Radio 2", "2", false, (PGM_P)FPSTR(HTML_UI_EMPTY_STRING));
            this->UI->addRadioButtonFormItem(this->HTMLResponse, F("radio"), "Radio 3", "3", true, "Hint text");

            this->UI->addParagraph(this->HTMLResponse, F("This is a Normal paragraph"));
            this->UI->addParagraph(this->HTMLResponse, F("This is a Intended paragraph"), true);

            this->UI->addSelectFormItemOpen(this->HTMLResponse, F("select1"), F("Select filed: Simple"));
            this->UI->addSelectOptionFormItem(this->HTMLResponse, "Option 1", "1", false);
            this->UI->addSelectOptionFormItem(this->HTMLResponse, "Option 2", "2", true);
            this->UI->addSelectOptionFormItem(this->HTMLResponse, "Option 3", "3", false);
            this->UI->addSelectFormItemClose(this->HTMLResponse);

            this->UI->addSelectFormItemOpen(this->HTMLResponse, F("select2"), F("Select filed: Hint"));
            this->UI->addSelectOptionFormItem(this->HTMLResponse, "Option 1", "1", true);
            this->UI->addSelectOptionFormItem(this->HTMLResponse, "Option 2", "2", false);
            this->UI->addSelectOptionFormItem(this->HTMLResponse, "Option 3", "3", false);
            this->UI->addSelectFormItemClose(this->HTMLResponse, (PGM_P)F("That is a hint"));

            this->UI->startList(this->HTMLResponse);
            this->UI->addListItem(this->HTMLResponse, (PGM_P)F("List item 1"));
            this->UI->addListItem(this->HTMLResponse, (PGM_P)F("List item 2"));
            this->UI->addListItem(this->HTMLResponse, (PGM_P)F("List item 3"));
            this->UI->addListItem(this->HTMLResponse, (PGM_P)F("List item 4"));
            this->UI->endList(this->HTMLResponse);

            this->UI->endForm(this->HTMLResponse, FPSTR(HTML_UI_SUBMITT_BUTTON_SAVE));

            this->UI->addParagraph(this->HTMLResponse, F("This is a paragraph 1"));
            this->UI->addParagraph(this->HTMLResponse, F("This is a paragraph 2"));
            this->UI->addParagraph(this->HTMLResponse, F("This is a paragraph 3"));

            this->UI->closeSection(this->HTMLResponse);

            this->UI->openMessageSection(this->HTMLResponse, F("This is Message Section"), F("This is an example of message section"));

            this->UI->addMessageItem(this->HTMLResponse, "This is a message item 1");
            this->UI->addMessageItem(this->HTMLResponse, "This is a message item 2");
            this->UI->addMessageItem(this->HTMLResponse, "This is a message item 3");

            this->UI->closeMessageSection(this->HTMLResponse);

            setHTTPResponseCode(ESPAPP_HTTP_RESPONSE_CODE_NOT_FOUND);

            this->setHTTPResponseCode(ESPAPP_HTTP_RESPONSE_CODE_OK);
            success = true;
            break;
        }
#endif
        default:
        {

#ifdef ESPAPP_CUSTOM_HTML_SITES_GENERATOR
            success = this->CustomSiteGenerator->processHTTPRequest(this->HTMLResponse);
            if (!success)
            {
                this->siteNotFound(this->HTMLResponse);
            }
#else
            this->siteNotFound(this->HTMLResponse);
#endif

            success = true;
            break;
        }
        }

        /** Setting CSS and JS Files  */
        this->UI->embedCSSFiles(this->HTMLResponse);
        this->UI->embedJSFiles(this->HTMLResponse);
    }

/** Set site global parametrs */
#ifdef ESPAPP_CUSTOM_HTML_SITES_GENERATOR
    this->CustomSiteGenerator->setSiteGlobalAttributes(this->HTMLResponse);
#else
    this->setSiteGlobalAttributes(this->HTMLResponse);
#endif

    this->UI->setLang(this->HTMLResponse, F("pl"));
    this->UI->setVersion(this->HTMLResponse, F("1.0.0"));
    this->UI->setWANAccess(this->HTMLResponse, 1); // @TODO not implemented yet

    /** Site closure  */
    this->UI->siteEnd(this->HTMLResponse);

#ifdef ESPAPP_CUSTOM_HTML_SITES_GENERATOR
    this->CustomSiteGenerator->clearOrphantTags(this->HTMLResponse);
#endif

#ifdef DEBUG
    this->System->Msg->printBulletPoint(F("Site generated"));
    if (this->HTMLResponse->length() > ESPAPP_HTTP_HTML_MAX_RESPONSE_SIZE)
    {
        this->System->Msg->printError(F("Site too long"), F("HTTP response"));
        this->System->Msg->printBulletPoint(F("Max size: "));
        this->System->Msg->printValue(ESPAPP_HTTP_HTML_MAX_RESPONSE_SIZE);
    }
    this->System->Msg->printBulletPoint(F("Site size: "));
    this->System->Msg->printValue(this->HTMLResponse->length());
#endif

    return success;
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

void ESPAPP_HTML_SitesGenerator::siteNotFound(String *site)
{
    this->UI->openSection(site, F("Error: 404"), F("Site not found"));
    this->UI->addParagraph(site, F("OMG you are lost!"), true);
    this->UI->addParagraph(site, F("Please go back to the main page"), true);    
    this->UI->closeSection(site);
    this->setHTTPResponseCode(ESPAPP_HTTP_RESPONSE_CODE_NOT_FOUND);
}

#ifndef ESPAPP_CUSTOM_HTML_SITES_GENERATOR

void ESPAPP_HTML_SitesGenerator::addMenu(String *site)
{
    /** Generate sample menu for the site */
    this->UI->startNavigationList(this->HTMLResponse, F("Menu"), FPSTR(HTML_UI_ICON_RIGHTWARDS_ARROW));
    this->UI->setUrlParams(this->urlParams, 1);
    this->UI->addNavigationItem(this->HTMLResponse, F("Index"), this->urlParams, "", FPSTR(HTML_UI_NO_ICON));
    this->UI->setUrlParams(this->urlParams, ESPAPP_HTTP_SITE_FIRMWARE_COMMAND);
    this->UI->addNavigationItem(this->HTMLResponse, F("Actions"), this->urlParams, "", FPSTR(HTML_UI_NO_ICON));
    this->UI->setUrlParams(this->urlParams, ESPAPP_HTTP_SITE_SYSTEM_PARAMETERS);
    this->UI->addNavigationItem(this->HTMLResponse, F("System"), this->urlParams, "", FPSTR(HTML_UI_ICON_ARROW));
    this->UI->setUrlParams(this->urlParams, ESPAPP_HTTP_SITE_FILE_EXPLORER);
    this->UI->addNavigationItem(this->HTMLResponse, F("Explorer"), this->urlParams, "", FPSTR(HTML_UI_ICON_DISK));
    this->UI->setUrlParams(this->urlParams, ESPAPP_HTTP_SITE_TOPOGRAPHY, 1, 1, 1);
    this->UI->addNavigationItem(this->HTMLResponse, F("Topography"), this->urlParams, "", FPSTR(HTML_UI_ICON_RIGHTWARDS_ARROW));
    // External Link
    this->UI->addNavigationItemExternal(this->HTMLResponse, F("SmartnyDom"), F("https://www.smartnydom.pl"), FPSTR(HTML_UI_ICON_ARROW));
    this->UI->endNavigationList(this->HTMLResponse);
}

void ESPAPP_HTML_SitesGenerator::setSiteGlobalAttributes(String *site)
{
    this->UI->setLogo(this->HTMLResponse, F("https://s.smartnydom.pl/i/esp32-firmware"));
    this->UI->setLogo(this->HTMLResponse, F("https://s.smartnydom.pl/i/esp32-firmware"));
    this->UI->setSubtitle1(this->HTMLResponse, F("ESPAPP"));
    this->UI->setSubtitle2(this->HTMLResponse, F("Version: 4.0.0"));
    this->UI->setURL(this->HTMLResponse, F("https://www.smartnydom.pl"));
    this->UI->setLogoURL(this->HTMLResponse, F("https://s.smartnydom.pl/i/SD00-0000-0001"));
}

#endif