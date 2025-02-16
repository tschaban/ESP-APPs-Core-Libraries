#include "ESPAPP_Default_HTML_SitesGenerator.h"

#ifndef ESPAPP_CUSTOM_HTML_SITES_GENERATORA

ESPAPP_HTML_SitesGenerator::ESPAPP_HTML_SitesGenerator(ESPAPP_Core *_System, String *_HTMLResponse, ESPAPP_HTTP_REQUEST *_HTTPRequest)
{
    this->System = _System;
    this->HTMLResponse = _HTMLResponse;
    this->HTTPRequest = _HTTPRequest;
    /** While initialized site gets it default sceleton */
    this->UI = new ESPAPP_HTML_UI(_System);
};

ESPAPP_HTML_SitesGenerator::~ESPAPP_HTML_SitesGenerator() {};

bool ESPAPP_HTML_SitesGenerator::processHTTPRequest(void)
{
#ifdef DEBUG
    this->System->Msg->printBulletPoint(F("Generating site: "));
#endif

    bool success = false;
    this->HTMLResponse->clear();

    switch (this->HTTPRequest->siteId)
    {
    case ESPAPP_HTTP_SITE_UPLOAD:
    {
        this->UI->startHeaderSection(this->HTMLResponse);
        this->UI->startMenuSection(this->HTMLResponse, F("Menu"), FPSTR(HTML_UI_ICON_RIGHTWARDS_ARROW));
        this->UI->endMenuSection(this->HTMLResponse);
        this->UI->startBodySection(this->HTMLResponse);

        ESPAPP_FILE files[10];
        size_t count = 0;
    
        this->UI->openSection(this->HTMLResponse, F("Files explorer"), F("Files list"));
        this->UI->startFileExplorer(this->HTMLResponse);

        this->System->Flash->listFolders(this->HTTPRequest->parameter1, files, 10, count);

        for (size_t i = 0; i < count; i++)
        {
            this->UI->addFileExplorerFolderItem(this->HTMLResponse, files[i].name, files[i].size);
        }

        count = 0;
        this->System->Flash->listFiles(this->HTTPRequest->parameter1, files, 10, count);

        for (size_t i = 0; i < count; i++)
        {
            this->UI->addFileExplorerFileItem(this->HTMLResponse, files[i].name, files[i].size);
        }

        this->UI->endFileExplorer(this->HTMLResponse);

        this->UI->closeSection(this->HTMLResponse);

        this->UI->openSection(this->HTMLResponse, F("Upload file"), F("Upload file to the current folder"));
        this->HTMLResponse->concat(F("<form action=\"/upload?site=0&cmd=0\" method=\"POST\" enctype=\"multipart/form-data\">"));
        this->HTMLResponse->concat(F("<input type=\"file\" name=\"uploadedFile\">"));
        this->HTMLResponse->concat(F("<button class=\"b bs\" type=\"submit\">Upload</button>"));
        this->HTMLResponse->concat(F("</form>"));
        this->UI->closeSection(this->HTMLResponse);

        /** Site configuration */
        this->UI->setLang(this->HTMLResponse, F("pl"));
        this->UI->setLogo(this->HTMLResponse, F("https://s.smartnydom.pl/i/esp32-firmware"));
        this->UI->setSubtitle1(this->HTMLResponse, F("ESPAPP"));
        this->UI->setSubtitle2(this->HTMLResponse, F("Version"));
        this->UI->setURL(this->HTMLResponse, F("https://www.smartnydom.pl"));
        this->UI->setLogoURL(this->HTMLResponse, F("https://s.smartnydom.pl/i/SD00-0000-0001"));

        /** Site closure  */
        this->UI->siteEnd(this->HTMLResponse);
        setHTTPResponseCode(ESPAPP_HTTP_RESPONSE_CODE_OK);
        break;
    }
    default:
    {
        /** Header */
        this->UI->startHeaderSection(this->HTMLResponse);

        /** Menu */

        ESPAPP_HTTP_REQUEST *urlParams = new ESPAPP_HTTP_REQUEST;

        this->UI->startMenuSection(this->HTMLResponse, F("Menu"), FPSTR(HTML_UI_ICON_RIGHTWARDS_ARROW));

        this->UI->setUrlParams(urlParams, 0);
        this->UI->addMenuItem(this->HTMLResponse, F("File explorer"), urlParams, "", FPSTR(HTML_UI_NO_ICON));
        this->UI->addMenuItem(this->HTMLResponse, F("Item 2"), urlParams, "additional=value", FPSTR(HTML_UI_NO_ICON));
        this->UI->setUrlParams(urlParams, 2);
        this->UI->addMenuItem(this->HTMLResponse, F("Item 3"), urlParams, "", FPSTR(HTML_UI_NO_ICON));
        this->UI->setUrlParams(urlParams, 3, 1);
        this->UI->addMenuItem(this->HTMLResponse, F("Item 4"), urlParams, "", FPSTR(HTML_UI_ICON_ARROW));
        this->UI->setUrlParams(urlParams, 4, 1, 1);
        this->UI->addMenuItem(this->HTMLResponse, F("Item 5"), urlParams, "", FPSTR(HTML_UI_NO_ICON));
        this->UI->setUrlParams(urlParams, 5, 1, 1, 1);
        this->UI->addMenuItem(this->HTMLResponse, F("Item 6"), urlParams, "", FPSTR(HTML_UI_NO_ICON));

        // SubMenu 1
        this->UI->addMenuSubMenuHeader(this->HTMLResponse, F("SubMenu 1"));
        this->UI->setUrlParams(urlParams, 1, 1, 1, 1);
        this->UI->addMenuItem(this->HTMLResponse, F("Item 1"), urlParams, "aditional=test", FPSTR(HTML_UI_ICON_ARROW));
        this->UI->setUrlParams(urlParams, 1, ESPAPP_NONE, ESPAPP_NONE, ESPAPP_NONE);
        this->UI->addMenuItem(this->HTMLResponse, F("Item 1"), urlParams, "", FPSTR(HTML_UI_ICON_ARROW));

        // External Link
        this->UI->addMenuItemExternal(this->HTMLResponse, F("Smart'my Dom"), F("https://www.smartnydom.pl"), FPSTR(HTML_UI_ICON_ARROW));

        this->UI->endMenuSection(this->HTMLResponse);

        //** Body */
        this->UI->startBodySection(this->HTMLResponse);

        this->UI->setUrlParams(urlParams, 1, 200);
        this->UI->startForm(this->HTMLResponse, urlParams, "additional=value");

        this->UI->openSection(this->HTMLResponse, F("This is Section"), F("This is an example of section"));

        this->UI->addInputFormItem(this->HTMLResponse, "text", "integer", "This is a numer field", "1", "25", "0", "200", "1", "This is a hint", false);
        this->UI->addInputFormItem(this->HTMLResponse, "text", "string", "This is a string  field", "This is text", "100", ESPAPP_FORM_SKIP_ATTRIBUTE, ESPAPP_FORM_SKIP_ATTRIBUTE, ESPAPP_FORM_SKIP_ATTRIBUTE, "This is a hint", false);

        this->UI->addCheckboxFormItem(this->HTMLResponse, "checkbox", "This is a checkbox", "1", true, "This is a hint", false);

        this->UI->addRadioButtonFormItem(this->HTMLResponse, "radio", "This is a radio button", "1", true, "This is a hint", false);

        this->UI->addSelectFormItemOpen(this->HTMLResponse, F("select"), F("This is a select field"));
        this->UI->addSelectOptionFormItem(this->HTMLResponse, "Option 1", "1", true);
        this->UI->addSelectOptionFormItem(this->HTMLResponse, "Option 2", "2", false);
        this->UI->addSelectFormItemClose(this->HTMLResponse);

        this->UI->addParagraph(this->HTMLResponse, F("This is a paragraph"));

        this->UI->closeSection(this->HTMLResponse);
        this->UI->endForm(this->HTMLResponse, FPSTR(HTML_UI_SUBMITT_BUTTON_SAVE));

        this->UI->openMessageSection(this->HTMLResponse, F("This is Message Section"), F("This is an example of message section"));

        this->UI->addParagraph(this->HTMLResponse, F("This is a paragraph"));

        this->UI->closeMessageSection(this->HTMLResponse);

        /** Site configuration */
        this->UI->setLang(this->HTMLResponse, F("pl"));
        this->UI->setLogo(this->HTMLResponse, F("https://s.smartnydom.pl/i/esp32-firmware"));
        this->UI->setSubtitle1(this->HTMLResponse, F("ESPAPP"));
        this->UI->setSubtitle2(this->HTMLResponse, F("Version"));
        this->UI->setURL(this->HTMLResponse, F("https://www.smartnydom.pl"));
        this->UI->setLogoURL(this->HTMLResponse, F("https://s.smartnydom.pl/i/SD00-0000-0001"));

        /** Site closure  */
        this->UI->siteEnd(this->HTMLResponse);
        setHTTPResponseCode(ESPAPP_HTTP_RESPONSE_CODE_NOT_FOUND);
        success = true;
        break;
    }
    }

#ifdef DEBUG
    this->System->Msg->printValue(F("Completed"));
#endif

    return success;
}

void ESPAPP_HTML_SitesGenerator::setHTTPResponseCode(int responseCode)
{
    this->HTTPRequest->HTTPResponseCode = responseCode;
}

#endif