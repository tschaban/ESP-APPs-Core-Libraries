#include "ESPAPP_Default_HTML_SitesGenerator.h"

#ifndef ESPAPP_CUSTOM_HTML_SITES_GENERATORA

ESPAPP_HTML_SitesGenerator::ESPAPP_HTML_SitesGenerator(ESPAPP_Core *_System, ESPAPP_HTTPServer *_HTTPServer, String *_HTMLResponse)
{
    this->System = _System;
    this->HTMLResponse = _HTMLResponse;
    this->Server = _HTTPServer;
    /** While initialized site gets it default sceleton */
    this->UI = new ESPAPP_HTML_UI(_System);
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
    ESPAPP_HTTP_REQUEST *urlParams = new ESPAPP_HTTP_REQUEST; // @TODO store it in class?

    if (this->Server->HTTPRequest->siteId != ESPAPP_HTTP_SITE_WIFI_CONFIGURATION)
    {

        /** Formatting HTTP Request for File Explorere */
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

        if (this->UI->showMenu())
        {

            this->UI->startNavigationList(this->HTMLResponse, F("Menu"), FPSTR(HTML_UI_ICON_RIGHTWARDS_ARROW));

            this->UI->setUrlParams(urlParams, 1);
            this->UI->addNavigationItem(this->HTMLResponse, F("Index"), urlParams, "", FPSTR(HTML_UI_NO_ICON));

            this->UI->setUrlParams(urlParams, ESPAPP_HTTP_SITE_FIRMWARE_COMMAND);
            this->UI->addNavigationItem(this->HTMLResponse, F("Commands"), urlParams, "", FPSTR(HTML_UI_NO_ICON));

            this->UI->setUrlParams(urlParams, ESPAPP_HTTP_SITE_FILE_EXPLORER);
            this->UI->addNavigationItem(this->HTMLResponse, F("FS Explorer"), urlParams, "", FPSTR(HTML_UI_NO_ICON));

            this->UI->setUrlParams(urlParams, 2, 1);
            this->UI->addNavigationItem(this->HTMLResponse, F("Dummy site: 1"), urlParams, "", FPSTR(HTML_UI_ICON_RIGHTWARDS_ARROW));
            this->UI->setUrlParams(urlParams, 4, 1, 1, 1);
            this->UI->addNavigationItem(this->HTMLResponse, F("Dummy site: 2"), urlParams, "", FPSTR(HTML_UI_ICON_ARROW));

            // External Link
            this->UI->addNavigationItemExternal(this->HTMLResponse, F("Smart'my Dom"), F("https://www.smartnydom.pl"), FPSTR(HTML_UI_ICON_ARROW));
            this->UI->endNavigationList(this->HTMLResponse);
        }

        this->UI->endNavigationBlock(this->HTMLResponse);
        //** Body */
        this->UI->startBodySection(this->HTMLResponse);

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

            this->UI->setRefresh(this->HTMLResponse, 10, F("/"));
            this->setHTTPResponseCode(ESPAPP_HTTP_RESPONSE_CODE_OK);

            this->System->setConnectionMode(this->Server->HTTPRequest->action);

            this->System->Events->triggerEvent(EVENT_REBOOT);
            success = true;
            break;
        }
        default:
        {

            this->UI->setUrlParams(urlParams, 1, 200);

            this->UI->openSection(this->HTMLResponse, F("UI Elements"), F("This is an example of UI elements"));

            this->UI->startForm(this->HTMLResponse, urlParams, (PGM_P)FPSTR(HTML_UI_EMPTY_STRING));

            this->UI->addInputFormItem(this->HTMLResponse, FPSTR(HTML_UI_INPUT_TYPE_TEXT), "text1", "Text filed: Simple", "Text");
            this->UI->addInputFormItem(this->HTMLResponse, FPSTR(HTML_UI_INPUT_TYPE_TEXT), "text2", "Text filed: Min", "Min 1 char", "30", "1");
            this->UI->addInputFormItem(this->HTMLResponse, FPSTR(HTML_UI_INPUT_TYPE_TEXT), "text3", "Text filed: Max", "Max 30 char", "30", (PGM_P)FPSTR(HTML_UI_INPUT_SKIP_ATTRIBUTE), "30");
            this->UI->addInputFormItem(this->HTMLResponse, FPSTR(HTML_UI_INPUT_TYPE_TEXT), "text4", "Text filed: Range", "Range: 20-30", "30", "20", "30");
            this->UI->addInputFormItem(this->HTMLResponse, FPSTR(HTML_UI_INPUT_TYPE_TEXT), "text5", "Text filed", "With custom hint", "30", (PGM_P)FPSTR(HTML_UI_INPUT_SKIP_ATTRIBUTE), (PGM_P)FPSTR(HTML_UI_INPUT_SKIP_ATTRIBUTE), (PGM_P)FPSTR(HTML_UI_INPUT_SKIP_ATTRIBUTE), "Custom");

            this->UI->addInputFormItem(this->HTMLResponse, FPSTR(HTML_UI_INPUT_TYPE_PASSWORD), "password", "Password", "1234567890", "16", (PGM_P)FPSTR(HTML_UI_INPUT_SKIP_ATTRIBUTE), "16");

            this->UI->addInputFormItem(this->HTMLResponse, FPSTR(HTML_UI_INPUT_TYPE_NUMBER), "number", "Number filed", "1000");

            this->UI->addCheckBoxFormItem(this->HTMLResponse, "checkbox1", "Checkbox 1", "1", false);
            this->UI->addCheckBoxFormItem(this->HTMLResponse, "checkbox2", "Checkbox 2", "1", true, (PGM_P)FPSTR(HTML_UI_EMPTY_STRING), true);
            this->UI->addCheckBoxFormItem(this->HTMLResponse, "checkbox3", "Checkbox 3", "1", true, "Hint text");

            this->UI->addRadioButtonFormItem(this->HTMLResponse, "radio", "Radio 1", "1", false);
            this->UI->addRadioButtonFormItem(this->HTMLResponse, "radio", "Radio 2", "2", false, (PGM_P)FPSTR(HTML_UI_EMPTY_STRING));
            this->UI->addRadioButtonFormItem(this->HTMLResponse, "radio", "Radio 3", "3", true, "Hint text");

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

            this->UI->closeSection(this->HTMLResponse);

            this->UI->openMessageSection(this->HTMLResponse, F("This is Message Section"), F("This is an example of message section"));

            this->UI->addParagraph(this->HTMLResponse, F("This is a paragraph"));

            this->UI->closeMessageSection(this->HTMLResponse);

            setHTTPResponseCode(ESPAPP_HTTP_RESPONSE_CODE_NOT_FOUND);

            success = true;
            break;
        }
        }

        /** Setting CSS and JS Files  */
        const char *cssFiles[] = {
            "/css?name=pure-min.css.gz", "/css?name=layout-v2.css", "/css?name=pure-custom.css"};
        const char *jsFiles[] = {
            "/js?name=responsive-menu-min.js.gz"};
        this->UI->embedCSSFiles(this->HTMLResponse, cssFiles, 3);
        this->UI->embedJSFiles(this->HTMLResponse, jsFiles, 1);
    }
    else
    {

        this->UI->setUrlParams(urlParams, 0);

        this->HTMLResponse->concat(FPSTR(HTML_UI_SITE_WIFI_CONFIGURATION));

        this->UI->startForm(this->HTMLResponse, urlParams, (PGM_P)FPSTR(HTML_UI_EMPTY_STRING));

        this->UI->addParagraph(this->HTMLResponse, F("WiFi Configuration"));

        this->UI->addSelectFormItemOpen(this->HTMLResponse, F("ssid"), F(""));

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

        this->UI->addInputFormItem(this->HTMLResponse, FPSTR(HTML_UI_INPUT_TYPE_PASSWORD), "password", "Password", "");

        this->UI->endForm(this->HTMLResponse, FPSTR(HTML_UI_SUBMITT_BUTTON_SAVE));

        setHTTPResponseCode(ESPAPP_HTTP_RESPONSE_CODE_NOT_FOUND);
    }

    /** Set site global parametrs */
    this->UI->setLang(this->HTMLResponse, F("pl"));
    this->UI->setLogo(this->HTMLResponse, F("https://s.smartnydom.pl/i/esp32-firmware"));
    this->UI->setLang(this->HTMLResponse, F("pl"));
    this->UI->setLogo(this->HTMLResponse, F("https://s.smartnydom.pl/i/esp32-firmware"));
    this->UI->setSubtitle1(this->HTMLResponse, F("ESPAPP"));
    this->UI->setSubtitle2(this->HTMLResponse, F("Version: 4.0.0"));
    this->UI->setURL(this->HTMLResponse, F("https://www.smartnydom.pl"));
    this->UI->setLogoURL(this->HTMLResponse, F("https://s.smartnydom.pl/i/SD00-0000-0001"));

    this->UI->setVersion(this->HTMLResponse, F("Build: 1000"));

    /** Site closure  */
    this->UI->siteEnd(this->HTMLResponse);

#ifdef DEBUG
    this->System->Msg->printBulletPoint(F("Site generated"));
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

#endif