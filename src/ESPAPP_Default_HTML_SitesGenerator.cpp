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

    this->configureSite();

    /** Header */
    this->UI->siteStart(this->HTMLResponse);

    this->UI->startNavigationBlock(this->HTMLResponse);

    /** Menu */
    ESPAPP_HTTP_REQUEST *urlParams = new ESPAPP_HTTP_REQUEST;

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

        // Handling a command

        if (strlen(this->Server->HTTPRequest->parameter1) == 0)
        {
            this->Server->readHTTPRequestDirectoryName(this->Server->HTTPRequest->parameter1);
        }

        // Handling a command
        if (this->Server->HTTPRequest->command == ESPAPP_HTTP_SITE_FILE_EXPLORER_COMMAND_UPLOAD)
        {
            if (this->Server->fileUpladedSuccessfully())
            {
                this->Server->saveUploadedFile();
            }
        }
        else if (this->Server->HTTPRequest->command == ESPAPP_HTTP_SITE_FILE_EXPLORER_COMMAND_DELETE)
        {
            if (this->Server->HTTPServer->hasArg("f"))
            {
                this->System->Flash->deleteFile(this->Server->HTTPRequest->parameter1, this->Server->HTTPServer->arg("f").c_str());
            }
        }

        ESPAPP_FILE files[10];
        size_t count = 0;

        this->UI->openSection(this->HTMLResponse, F("Files explorer"), F("Files list"));
        this->UI->startFileExplorer(this->HTMLResponse);

        this->System->Flash->listFolders(this->Server->HTTPRequest->parameter1, files, 10, count);

        if (strcmp(this->Server->HTTPRequest->parameter1, (PGM_P)FPSTR(path_root)) != 0)
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
        break;
    }
    case ESPAPP_HTTP_SITE_FIRMWARE_COMMAND: // Firmware commands
    {
        this->UI->openSection(this->HTMLResponse, F("Commands"), F("Firmware commands"));
        this->UI->addParagraph(this->HTMLResponse, F("<a href=\"/?site=101&cmd=15\">Reboot device</a>"),true); // Refactor it
        this->UI->closeSection(this->HTMLResponse);
        this->setHTTPResponseCode(ESPAPP_HTTP_RESPONSE_CODE_OK);
        break; 
    }
    case ESPAPP_HTTP_SITE_REBOOT:
    {
        this->UI->openSection(this->HTMLResponse, F("Rebooting"), F("Rebooting device"));
        this->UI->addParagraph(this->HTMLResponse, F("Rebooting device in 10 seconds"));
        this->UI->closeSection(this->HTMLResponse);

        this->UI->setRefresh(this->HTMLResponse, 10, F("/"));
        this->setHTTPResponseCode(ESPAPP_HTTP_RESPONSE_CODE_OK);

        
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
        "/css?name=pure-min.css.gz", "/css?name=layout-v2.css"};
    const char *jsFiles[] = {
        "/js?name=responsive-menu-min.js.gz"};
    this->UI->embedCSSFiles(this->HTMLResponse, cssFiles, 2);
    this->UI->embedJSFiles(this->HTMLResponse, jsFiles, 1);

    /** Set site global parametrs */
    this->UI->setLang(this->HTMLResponse, F("pl"));
    this->UI->setLogo(this->HTMLResponse, F("https://s.smartnydom.pl/i/esp32-firmware"));
    this->UI->setLang(this->HTMLResponse, F("pl"));
    this->UI->setLogo(this->HTMLResponse, F("https://s.smartnydom.pl/i/esp32-firmware"));
    this->UI->setSubtitle1(this->HTMLResponse, F("ESPAPP"));
    this->UI->setSubtitle2(this->HTMLResponse, F("Version"));
    this->UI->setURL(this->HTMLResponse, F("https://www.smartnydom.pl"));
    this->UI->setLogoURL(this->HTMLResponse, F("https://s.smartnydom.pl/i/SD00-0000-0001"));

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