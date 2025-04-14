#include "ESPAPP_HTML_UI.h"

ESPAPP_HTML_UI::ESPAPP_HTML_UI(ESPAPP_Core *_System)
{
#ifdef DEBUG
    this->System->Msg->printInformation(F("Initializing HTML UI"), F("HTML UI"));
#endif
    System = _System;
    cssLinks.reserve((sizeof(HTML_UI_CSS) / sizeof(HTML_UI_CSS[0])) * (strlen_P(HTML_UI_SITE_CSS_FILE_TAG) - 5 + strlen_P(css_core) + 1)); // max 3 CSS files * 5 = {{v}} + max length of css_core + 1 = null terminator
    jsLinks.reserve((sizeof(HTML_UI_JS) / sizeof(HTML_UI_JS[0])) * (strlen_P(HTML_UI_SITE_JS_FILE_TAG) - 5 + strlen_P(js_menu) + 1));      // max 3 JS files * 5 = {{v}} + max length of js_menu + 1 = null terminator


#ifdef DEBUG
    this->System->Msg->printBulletPoint(F("UI Initialized"));
#endif
}

ESPAPP_HTML_UI::~ESPAPP_HTML_UI()
{
}

void ESPAPP_HTML_UI::setLang(String *site, const __FlashStringHelper *lang)
{
    site->replace(F("{{s.lang}}"), lang);
}

void ESPAPP_HTML_UI::setRefresh(String *site, uint8_t refresh, const char *url)
{

#ifdef DEBUG
    this->System->Msg->printInformation(F("Setting refresh"), F("HTTP"));
    this->System->Msg->printBulletPoint(F("URL to redirect: "));
    this->System->Msg->printValue(url);
#endif

    char timeout[4];
    if (refresh == ESPAPP_NONE)
    {
        sprintf(timeout, "%d", 0);
    }
    else
    {
        sprintf(timeout, "%d", refresh);
    }

#ifdef DEBUG
    this->System->Msg->printBulletPoint(F("Timeout: "));
    this->System->Msg->printValue(timeout);
#endif

    site->replace(F("{{s.refresh}}"), FPSTR(HTML_UI_SITE_REFRESH_TAG));
    this->replaceTagValue(site, timeout);
    this->replaceTagTitle(site, url);
}

void ESPAPP_HTML_UI::setTitle(String *site, const __FlashStringHelper *title)
{
    site->replace(F("{{s.title}}"), title);
}

void ESPAPP_HTML_UI::setLogo(String *site, const __FlashStringHelper *logo)
{
    site->replace(F("{{f.logo}}"), logo);
}

void ESPAPP_HTML_UI::setSubtitle1(String *site, const __FlashStringHelper *subtitle1)
{
    site->replace(F("{{f.subtitle-1}}"), subtitle1);
}

void ESPAPP_HTML_UI::setSubtitle2(String *site, const __FlashStringHelper *subtitle2)
{
    site->replace(F("{{f.subtitle-2}}"), subtitle2);
}

void ESPAPP_HTML_UI::setMenu(String *site, const __FlashStringHelper *menu)
{
    site->replace(F("{{menu}}"), menu);
}

void ESPAPP_HTML_UI::setBody(String *site, const __FlashStringHelper *body)
{
    site->replace(F("{{body}}"), body);
}

void ESPAPP_HTML_UI::setURL(String *site, const __FlashStringHelper *url)
{
    site->replace(F("{{f.url}}"), url);
}

void ESPAPP_HTML_UI::setLogoURL(String *site, const __FlashStringHelper *logoURL)
{
    site->replace(F("{{f.logo-url}}"), logoURL);
}

void ESPAPP_HTML_UI::setVersion(String *site, const __FlashStringHelper *version)
{
    site->replace(F(HTML_UI_TAG_FIRMWARE_VERSION), version);
}

void ESPAPP_HTML_UI::setFreeHeap(String *site)
{
    char freeHeap[8];
    sprintf(freeHeap, "%dkB", ESP.getFreeHeap() / 1024);
    site->replace(F(HTML_UI_TAG_FREE_HEAP), freeHeap);
}

void ESPAPP_HTML_UI::setWANAccess(String *site, boolean access)
{
    site->replace(F(HTML_UI_TAG_WAN_YES_NO), (access ? F("Yes") : F("No")));
}

void ESPAPP_HTML_UI::embedCSSFiles(String *site)
{
    for (uint8_t i = 0; i < sizeof(HTML_UI_CSS) / sizeof(HTML_UI_CSS[0]); i++)
    {
        this->cssLinks.concat(FPSTR(HTML_UI_SITE_CSS_FILE_TAG));
        this->replaceTagValue(&this->cssLinks, (char *)pgm_read_dword(&(HTML_UI_CSS[i])));
    }

    site->replace(F(HTML_UI_TAG_CSS), this->cssLinks);
    this->cssLinks.clear();
}

void ESPAPP_HTML_UI::embedJSFiles(String *site)
{
    for (uint8_t i = 0; i < sizeof(HTML_UI_JS) / sizeof(HTML_UI_JS[0]); i++)
    {
        this->jsLinks.concat(FPSTR(HTML_UI_SITE_JS_FILE_TAG));
        this->replaceTagValue(&this->jsLinks, (char *)pgm_read_dword(&(HTML_UI_JS[i])));
    }

    site->replace(F(HTML_UI_TAG_JS), this->jsLinks);
    this->jsLinks.clear();
}

void ESPAPP_HTML_UI::clearOrphantTags(String *site)
{
    site->replace(F("{{s.lang}}"), F("en"));
    site->replace(F("{{s.refresh}}"), FPSTR(HTML_UI_EMPTY_STRING));
    site->replace(F("{{s.title}}"), F(ESPAPP_DEFAULT_DEVICE_NAME));

   // if (!this->isLargeCSS())
   // {
  //      site->replace(F(HTML_UI_TAG_CSS), FPSTR(HTML_UI_SITE_LIGHT_CSS));
  //  }
 //   else
 //   {
        this->embedCSSFiles(site);
        this->embedJSFiles(site);
//}

    site->replace(F(HTML_UI_TAG_CSS), FPSTR(HTML_UI_EMPTY_STRING));
    site->replace(F(HTML_UI_TAG_JS), FPSTR(HTML_UI_EMPTY_STRING));
    site->replace(F("{{f.logo}}"), FPSTR(HTML_UI_EMPTY_STRING));
    site->replace(F("{{f.subtitle-1}}"), FPSTR(HTML_UI_EMPTY_STRING));
    site->replace(F("{{f.subtitle-2}}"), FPSTR(HTML_UI_EMPTY_STRING));
    site->replace(F("{{menu}}"), FPSTR(HTML_UI_EMPTY_STRING));
    site->replace(F("{{f.url}}"), FPSTR(HTML_UI_EMPTY_STRING));
    site->replace(F("{{f.logo-url}}"), FPSTR(HTML_UI_EMPTY_STRING));

    this->setFreeHeap(site);
}

//------------------------------------------------------------

void ESPAPP_HTML_UI::siteStart(String *site)
{
    site->concat(FPSTR(HTML_UI_SITE_HEADER));
}

void ESPAPP_HTML_UI::siteEnd(String *site)
{
    site->concat(FPSTR(HTML_UI_SITE_FOOTER));
    this->clearOrphantTags(site);
}

void ESPAPP_HTML_UI::startNavigationBlock(String *site)
{
    site->concat(FPSTR(HTML_UI_SITE_MENU_BLOCK_START));
}

void ESPAPP_HTML_UI::endNavigationBlock(String *site)
{
    site->concat(FPSTR(HTML_UI_SITE_MENU_BLOCK_END));
}

void ESPAPP_HTML_UI::startNavigationList(String *item, const __FlashStringHelper *title, const __FlashStringHelper *icon)
{
    item->concat(FPSTR(HTML_UI_SITE_MENU_SECTION_START));
    this->replaceTagTitle(item, title);
    this->replaceTagIcon(item, icon);
}

void ESPAPP_HTML_UI::endNavigationList(String *item)
{
    item->concat(FPSTR(HTML_UI_SITE_MENU_SECTION_END));
}

void ESPAPP_HTML_UI::addNavigationItem(String *item, const __FlashStringHelper *title, ESPAPP_HTTP_REQUEST *url, const char *parameters, const __FlashStringHelper *icon)
{
    item->concat(FPSTR(HTML_UI_SITE_MENU_SECTION_ITEM));
    this->replaceTagTitle(item, title);
    this->replaceTagUrlParams(item, url, parameters);
    this->replaceTagIcon(item, icon);
}

void ESPAPP_HTML_UI::addNavigationItemExternal(String *item,
                                               const __FlashStringHelper *title,
                                               const __FlashStringHelper *url, const __FlashStringHelper *icon)
{
    item->concat(FPSTR(HTML_UI_SITE_MENU_SECTION_ITEM_EXTERNAL));
    this->replaceTagTitle(item, title);
    this->replaceTagIcon(item, icon);
    this->replaceTagValue(item, url);
}

void ESPAPP_HTML_UI::startBodySection(String *site)
{
    site->concat(FPSTR(HTML_UI_SITE_BODY_START));
}

/** Section releated methods  */
void ESPAPP_HTML_UI::openSection(String *site, const char *title,
                                 const __FlashStringHelper *description)
{
    site->concat(FPSTR(HTML_UI_SECTION_START));
    this->replaceTagTitle(site, title);
    this->replaceTagValue(site, description);
}

void ESPAPP_HTML_UI::openSection(String *site,
                                 const __FlashStringHelper *title,
                                 const __FlashStringHelper *description)
{
    site->concat(FPSTR(HTML_UI_SECTION_START));
    this->replaceTagTitle(site, title);
    this->replaceTagValue(site, description);
}

void ESPAPP_HTML_UI::closeSection(String *site)
{
    site->concat(FPSTR(HTML_UI_SECTION_END));
}

void ESPAPP_HTML_UI::openMessageSection(
    String *site, const char *title, const __FlashStringHelper *description)
{
    this->openSection(site, title, description);
    site->concat(F("<ul class=\"lst\">"));
}

void ESPAPP_HTML_UI::openMessageSection(
    String *site, const __FlashStringHelper *title,
    const __FlashStringHelper *description)
{
    this->openSection(site, title, description);
    site->concat(F("<ul class=\"lst\">"));
}

void ESPAPP_HTML_UI::closeMessageSection(String *site)
{
    site->concat(F("</ul>"));
    this->closeSection(site);
}

void ESPAPP_HTML_UI::addMessageItem(String *site, const char *item)
{
    this->addListItem(site, item);
}

void ESPAPP_HTML_UI::showSavedMessages(String *site, const __FlashStringHelper *title,
                                       const __FlashStringHelper *description)
{
    if (this->System->Message->newMessageAvailable())
    {

        this->openMessageSection(site, title, description);

        char **messages;
        uint8_t count = 0;

        this->System->Message->getMessages(messages, count);
        for (int i = 0; i < count; i++)
        {
            this->addMessageItem(site, messages[i]);
            delete[] messages[i];
        }
        delete[] messages;

        this->closeMessageSection(site);
    }
}

/** Form */

void ESPAPP_HTML_UI::startForm(String *site, ESPAPP_HTTP_REQUEST *url, const char *parameters)
{
    site->concat(FPSTR(HTML_UI_FORM_START));
    this->replaceTagUrlParams(site, url, parameters);
}

void ESPAPP_HTML_UI::endForm(String *site, const __FlashStringHelper *submiButtonLabel)
{
    site->concat(FPSTR(HTML_UI_FORM_END));
    site->replace(FPSTR(HTML_UI_TAG_TITLE), submiButtonLabel);
}

void ESPAPP_HTML_UI::addInputFormItem(String *item, const __FlashStringHelper *type,
                                      const __FlashStringHelper *name, const char *label,
                                      const char *value, const char *size,
                                      const char *min, const char *max,
                                      const char *step, const char *hint,
                                      boolean readonly)
{

    item->concat(FPSTR(HTML_UI_FORM_ITEM_INPUT));

    /** Basic Input */
    item->replace(FPSTR(HTML_UI_TAG_TITLE), label);
    item->replace(FPSTR(HTML_UI_TAG_NAME), name);
    item->replace(FPSTR(HTML_UI_TAG_TYPE), type);
    item->replace(FPSTR(HTML_UI_TAG_VALUE), value);

    /** Add: Readonly attributed */
    if (readonly)
    {
        item->replace(F("{{ir}}"), PSTR(HTML_UI_FORM_ITEM_INPUT_ATTRIBUTE_READONLY));
    }
    else
    {
        item->replace(F("{{ir}}"), FPSTR(HTML_UI_EMPTY_STRING));
    }

    /** Add hint if text. If it's not set it will be added automaticaly if min/max/minlength/maxlength provided  */
    if (strcmp(hint, (PGM_P)FPSTR(HTML_UI_INPUT_SKIP_ATTRIBUTE)) != 0)
    {

        this->replaceTagHint(item, FPSTR(HTML_UI_FORM_ITEM_INPUT_HINT));
        this->replaceTagValue(item, hint);
    }

    /** Input Type: Number */
    if (strcmp((PGM_P)type, (PGM_P)FPSTR(HTML_UI_INPUT_TYPE_NUMBER)) == 0)
    {

        // min
        if (strcmp(min, (PGM_P)FPSTR(HTML_UI_INPUT_SKIP_ATTRIBUTE)) != 0)
        {
            item->replace(F("{{im}}"), FPSTR(HTML_UI_FORM_ITEM_INPUT_ATTRIBUTE_MIN));
            item->replace(FPSTR(HTML_UI_TAG_VALUE), min);
        }

        // max
        if (strcmp(max, (PGM_P)FPSTR(HTML_UI_INPUT_SKIP_ATTRIBUTE)) != 0)
        {
            item->replace(F("{{ix}}"), FPSTR(HTML_UI_FORM_ITEM_INPUT_ATTRIBUTE_MAX));
            item->replace(FPSTR(HTML_UI_TAG_VALUE), max);
        }

        // step
        if (strcmp(step, (PGM_P)FPSTR(HTML_UI_INPUT_SKIP_ATTRIBUTE)) != 0)
        {
            item->replace(F("{{ist}}"), FPSTR(HTML_UI_FORM_ITEM_INPUT_ATTRIBUTE_STEP));
            item->replace(FPSTR(HTML_UI_TAG_VALUE), step);
        }

        // Hint
        if (strcmp(hint, (PGM_P)FPSTR(HTML_UI_INPUT_SKIP_ATTRIBUTE)) == 0)
        {
            if (strcmp(min, (PGM_P)FPSTR(HTML_UI_INPUT_SKIP_ATTRIBUTE)) != 0 && strcmp(max, (PGM_P)FPSTR(HTML_UI_INPUT_SKIP_ATTRIBUTE)) != 0)
            {
                this->replaceTagHint(item, FPSTR(HTML_UI_FORM_ITEM_INPUT_HINT));
                this->replaceTagValue(item, FPSTR(HTML_UI_FORM_ITEM_INPUT_HINT_RANGE_NUMBER));
                item->replace(F("{{l}}"), min);
                item->replace(F("{{m}}"), max);
            }
            else if (strcmp(min, (PGM_P)FPSTR(HTML_UI_INPUT_SKIP_ATTRIBUTE)) != 0)
            {
                this->replaceTagHint(item, FPSTR(HTML_UI_FORM_ITEM_INPUT_HINT));
                this->replaceTagValue(item, FPSTR(HTML_UI_FORM_ITEM_INPUT_HINT_MIN_NUMBER));
                this->replaceTagValue(item, min);
            }
            else if (strcmp(max, (PGM_P)FPSTR(HTML_UI_INPUT_SKIP_ATTRIBUTE)) != 0)
            {
                this->replaceTagHint(item, FPSTR(HTML_UI_FORM_ITEM_INPUT_HINT));
                this->replaceTagValue(item, FPSTR(HTML_UI_FORM_ITEM_INPUT_HINT_MAX_NUMBER));
                this->replaceTagValue(item, max);
            }
        }
        /** Input Type: Text and Password */
    }
    else if (strcmp((PGM_P)type, (PGM_P)FPSTR(HTML_UI_INPUT_TYPE_TEXT)) == 0 || strcmp((PGM_P)type, (PGM_P)FPSTR(HTML_UI_INPUT_TYPE_PASSWORD)) == 0)
    {

        // size
        if (strcmp(size, (PGM_P)FPSTR(HTML_UI_INPUT_SKIP_ATTRIBUTE)) != 0)
        {
            item->replace(F("{{is}}"), FPSTR(HTML_UI_FORM_ITEM_INPUT_ATTRIBUTE_SIZE));
            item->replace(FPSTR(HTML_UI_TAG_VALUE), size);
        }

        // minLength
        if (strcmp(min, (PGM_P)FPSTR(HTML_UI_INPUT_SKIP_ATTRIBUTE)) != 0)
        {
            item->replace(F("{{im}}"), FPSTR(HTML_UI_FORM_ITEM_INPUT_ATTRIBUTE_MINLENGTH));
            item->replace(FPSTR(HTML_UI_TAG_VALUE), min);
        }

        // maxLength
        if (strcmp(max, (PGM_P)FPSTR(HTML_UI_INPUT_SKIP_ATTRIBUTE)) != 0)
        {
            item->replace(F("{{ix}}"), FPSTR(HTML_UI_FORM_ITEM_INPUT_ATTRIBUTE_MAXLENGTH));
            item->replace(FPSTR(HTML_UI_TAG_VALUE), max);
        }

        // Hint
        if (strcmp(hint, (PGM_P)FPSTR(HTML_UI_INPUT_SKIP_ATTRIBUTE)) == 0)
        {
            if (strcmp(min, (PGM_P)FPSTR(HTML_UI_INPUT_SKIP_ATTRIBUTE)) != 0 && strcmp(max, (PGM_P)FPSTR(HTML_UI_INPUT_SKIP_ATTRIBUTE)) != 0)
            {
                this->replaceTagHint(item, FPSTR(HTML_UI_FORM_ITEM_INPUT_HINT));
                this->replaceTagValue(item, FPSTR(HTML_UI_FORM_ITEM_INPUT_HINT_RANGE_TEXT));
                item->replace(F("{{l}}"), min);
                item->replace(F("{{m}}"), max);
            }
            else if (strcmp(min, (PGM_P)FPSTR(HTML_UI_INPUT_SKIP_ATTRIBUTE)) != 0)
            {
                item->replace(FPSTR(HTML_UI_TAG_HINT), FPSTR(HTML_UI_FORM_ITEM_INPUT_HINT));
                item->replace(FPSTR(HTML_UI_TAG_VALUE), FPSTR(HTML_UI_FORM_ITEM_INPUT_HINT_MIN_TEXT));
                this->replaceTagValue(item, min);
            }
            else if (strcmp(max, (PGM_P)FPSTR(HTML_UI_INPUT_SKIP_ATTRIBUTE)) != 0)
            {
                item->replace(FPSTR(HTML_UI_TAG_HINT), FPSTR(HTML_UI_FORM_ITEM_INPUT_HINT));
                item->replace(FPSTR(HTML_UI_TAG_VALUE), FPSTR(HTML_UI_FORM_ITEM_INPUT_HINT_MAX_TEXT));
                this->replaceTagValue(item, max);
            }
        }
    }
    // Clear unused attributes
    item->replace(F("{{im}}"), FPSTR(HTML_UI_EMPTY_STRING));
    item->replace(F("{{ix}}"), FPSTR(HTML_UI_EMPTY_STRING));
    item->replace(F("{{ist}}"), FPSTR(HTML_UI_EMPTY_STRING));
    item->replace(F("{{is}}"), FPSTR(HTML_UI_EMPTY_STRING));
    this->replaceTagHint(item, FPSTR(HTML_UI_EMPTY_STRING));
}

void ESPAPP_HTML_UI::addRadioButtonOrCheckBoxFormItem(String *item, const __FlashStringHelper *type, const __FlashStringHelper *name, const char *label,
                                                      const char *value, boolean checked,
                                                      const char *hint,
                                                      boolean disabled)

{
    item->concat(FPSTR(HTML_UI_FORM_ITEM_CHECK_OR_RADIO_BOX));
    item->replace(FPSTR(HTML_UI_TAG_TYPE), type);
    item->replace(FPSTR(HTML_UI_TAG_TITLE), label);
    item->replace(FPSTR(HTML_UI_TAG_NAME), name);
    item->replace(FPSTR(HTML_UI_TAG_VALUE), value);
    item->replace(F("{{i.c}}"), checked ? FPSTR(HTML_UI_FORM_ITEM_ATTRIBUTE_CHECKED) : FPSTR(HTML_UI_EMPTY_STRING));
    item->replace(F("{{i.d}}"), disabled ? FPSTR(HTML_UI_FORM_ITEM_ATTRIBUTE_DISABLED) : FPSTR(HTML_UI_EMPTY_STRING));
    if (strcmp(hint, (PGM_P)FPSTR(HTML_UI_INPUT_SKIP_ATTRIBUTE)) != 0)
    {
        this->replaceTagHint(item, FPSTR(HTML_UI_FORM_ITEM_INPUT_HINT));
        this->replaceTagValue(item, hint);
    }
    else
    {
        this->replaceTagHint(item, FPSTR(HTML_UI_EMPTY_STRING));
    }
}

void ESPAPP_HTML_UI::addCheckBoxFormItem(String *item, const __FlashStringHelper *name,
                                         const char *label,
                                         const char *value, boolean checked,
                                         const char *hint,
                                         boolean disabled)
{
    this->addRadioButtonOrCheckBoxFormItem(item, FPSTR(HTML_UI_INPUT_TYPE_CHECKBOX), name, label, value, checked, hint,
                                           disabled);
}

void ESPAPP_HTML_UI::addRadioButtonFormItem(
    String *item, const __FlashStringHelper *name, const char *label, const char *value,
    boolean checked, const char *hint, boolean disabled)
{
    this->addRadioButtonOrCheckBoxFormItem(item, FPSTR(HTML_UI_INPUT_TYPE_RADIO), name, label, value, checked, hint,
                                           disabled);
}

void ESPAPP_HTML_UI::addSelectFormItemOpen(
    String *item, const __FlashStringHelper *name,
    const __FlashStringHelper *label)
{
    item->concat(FPSTR(HTML_UI_ITEM_SELECT_OPEN));
    this->replaceTagTitle(item, label);
    this->replaceTagValue(item, name);
}

void ESPAPP_HTML_UI::addSelectOptionFormItem(String *item, const char *label,
                                             const char *value,
                                             boolean selected)
{
    item->concat(FPSTR(HTML_UI_ITEM_SELECT_OPTION));
    item->replace(FPSTR(HTML_UI_TAG_VALUE), value);
    item->replace(FPSTR(HTML_UI_TAG_TITLE), label);
    item->replace(F("{{i.s}}"), selected ? F(" selected=\"selected\"") : F(""));
}

void ESPAPP_HTML_UI::addSelectFormItemClose(String *item, const char *hint)
{
    item->concat(FPSTR(HTML_UI_ITEM_SELECT_CLOSE));
    if (strcmp(hint, (PGM_P)FPSTR(HTML_UI_INPUT_SKIP_ATTRIBUTE)) != 0)
    {
        this->replaceTagValue(item, hint);
    }
    else
    {
        this->replaceTagValue(item, FPSTR(HTML_UI_EMPTY_STRING));
    }
}

void ESPAPP_HTML_UI::addParagraphTag(String *item, bool indented)
{
    if (indented)
    {
        item->concat(FPSTR(HTML_UI_FORM_ITEM_PARAGRAPH));
    }
    else
    {
        item->concat(FPSTR(HTML_UI_ITEM_PARAGRAPH));
    }
}

void ESPAPP_HTML_UI::addParagraph(String *item, const __FlashStringHelper *text, bool indented)
{
    this->addParagraphTag(item, indented);
    this->replaceTagValue(item, text);
}

void ESPAPP_HTML_UI::addParagraph(String *item, const char *text, bool indented)
{
    this->addParagraphTag(item, indented);
    this->replaceTagValue(item, text);
}

void ESPAPP_HTML_UI::startList(String *site)
{
    site->concat(FPSTR(HTML_UI_ITEM_LIST_START));
}

void ESPAPP_HTML_UI::endList(String *site)
{
    site->concat(FPSTR(HTML_UI_ITEM_LIST_END));
}

void ESPAPP_HTML_UI::addListItem(String *site, const char *item)
{
    site->concat(FPSTR(HTML_UI_ITEM_LIST_ITEM));
    site->replace(FPSTR(HTML_UI_TAG_VALUE), item);
}

/** Files explorer */
void ESPAPP_HTML_UI::startFileExplorer(String *site)
{
    site->concat(FPSTR(HTML_UI_FILE_EXPLORER_HEADER));
}

void ESPAPP_HTML_UI::endFileExplorer(String *site)
{
    site->concat(FPSTR(HTML_UI_FILE_EXPLORER_FOOTER));
}

void ESPAPP_HTML_UI::addFileExplorerFolderItem(String *site, const char *name, size_t size)
{
    site->concat(FPSTR(HTML_UI_FILE_EXPLORER_FOLDER_ITEM));
    this->replaceTagValue(site, name);
    site->replace(FPSTR(HTML_UI_TAG_TITLE), String(size));
    if (strcmp(name, (PGM_P)FPSTR(path_root)) != 0 && size == 0)
    {
        this->replaceTagHint(site, FPSTR(HTML_UI_EMPTY_STRING));
    }
    else
    {
        this->replaceTagHint(site, FPSTR(HTML_UI_FORM_ITEM_ATTRIBUTE_DISABLED));
    }
}

void ESPAPP_HTML_UI::addFileExplorerFileItem(String *site, const char *file, size_t size, const char *directory)
{
    site->concat(FPSTR(HTML_UI_FILE_EXPLORER_FILE_ITEM));
    this->replaceTagTitle(site, file);
    site->replace(F("{{f.s}}"), String(size));
    this->replaceTagValue(site, directory);
}

void ESPAPP_HTML_UI::addFileExplorerUploadForm(String *site, const char *directory)
{
    site->concat(FPSTR(HTML_UI_FILE_EXPLORER_UPLOAD_FORM));
    this->replaceTagValue(site, directory);
}
/*
void ESPAPP_HTML_UI::checkCSSFilesExist()
{
#ifdef DEBUG
    this->System->Msg->printBulletPoint(F("Checking if CSS Files are downloaded"));
#endif

    this->largeCSS = true;

    for (uint8_t i = 0; i < sizeof(HTML_UI_CSS) / sizeof(HTML_UI_CSS[0]); i++)
    {
        sprintf(this->System->Flash->fileName, "%s%s%s", FPSTR(path_root), FPSTR(path_ui), (char *)pgm_read_dword(&(HTML_UI_CSS[i])));
        if (!this->System->Flash->fileExist(this->System->Flash->fileName))
        {
            this->largeCSS = false;
            break;
        }
    }

#ifdef DEBUG
    this->System->Msg->printBulletPoint(F("CSS Files exists : "));
    this->System->Msg->printValue(this->largeCSS);
#endif
}
*/
/** Private */
void ESPAPP_HTML_UI::replaceTagTitle(String *item, const __FlashStringHelper *title)
{
    item->replace(FPSTR(HTML_UI_TAG_TITLE), title);
}

void ESPAPP_HTML_UI::replaceTagTitle(String *item, const char *title)
{
    item->replace(FPSTR(HTML_UI_TAG_TITLE), title);
}

void ESPAPP_HTML_UI::replaceTagIcon(String *item, const __FlashStringHelper *icon)
{
    item->replace(FPSTR(HTML_UI_TAG_ICON), icon);
}

void ESPAPP_HTML_UI::replaceTagUrlParams(String *item, ESPAPP_HTTP_REQUEST *url, const char *parameters)
{

    if (url->siteId != ESPAPP_NONE)
    {
        item->replace(F("{{mi.s}}"), (PGM_P)F("site=") + String(url->siteId) + (PGM_P)F("&"));
    }
    else
    {
        item->replace(F("{{mi.s}}"), FPSTR(HTML_UI_EMPTY_STRING));
    }

    if (url->command != ESPAPP_NONE)
    {
        item->replace(F("{{mi.c}}"), (PGM_P)F("cmd=") + String(url->command) + (PGM_P)F("&"));
    }
    else
    {
        item->replace(F("{{mi.c}}"), FPSTR(HTML_UI_EMPTY_STRING));
    }

    if (url->action != ESPAPP_NONE)
    {
        item->replace(F("{{mi.a}}"), (PGM_P)F("action=") + String(url->command) + (PGM_P)F("&"));
    }
    else
    {
        item->replace(F("{{mi.a}}"), FPSTR(HTML_UI_EMPTY_STRING));
    }

    if (url->option != ESPAPP_NONE)
    {
        item->replace(F("{{mi.o}}"), (PGM_P)F("option=") + String(url->command) + (PGM_P)F("&"));
    }
    else
    {
        item->replace(F("{{mi.o}}"), FPSTR(HTML_UI_EMPTY_STRING));
    }

    if (parameters != nullptr && strlen(parameters) > 0)
    {
        item->replace(F("{{mi.p}}"), parameters);
    }
    else
    {
        item->replace(F("{{mi.p}}"), FPSTR(HTML_UI_EMPTY_STRING));
    }
}

void ESPAPP_HTML_UI::replaceTagValue(String *item, const __FlashStringHelper *value)
{
    item->replace(FPSTR(HTML_UI_TAG_VALUE), value);
}

void ESPAPP_HTML_UI::replaceTagValue(String *item, const char *value)
{
    item->replace(FPSTR(HTML_UI_TAG_VALUE), value);
}

void ESPAPP_HTML_UI::replaceTagHint(String *item, const __FlashStringHelper *value)
{
    item->replace(FPSTR(HTML_UI_TAG_HINT), value);
}

void ESPAPP_HTML_UI::replaceTagHint(String *item, const char *value)
{
    item->replace(FPSTR(HTML_UI_TAG_HINT), value);
}

void ESPAPP_HTML_UI::setUrlParams(ESPAPP_HTTP_REQUEST *url, uint8_t siteId, uint8_t command, uint8_t action, uint8_t option)
{
    url->siteId = siteId;
    url->command = command;
    url->action = action;
    url->option = option;
}

void ESPAPP_HTML_UI::removeMenu(void)
{
    this->isMenuSet = false;
}

bool ESPAPP_HTML_UI::showMenu()
{
    bool previousMenuState = this->isMenuSet;
    this->isMenuSet = true;
    return previousMenuState;
}
