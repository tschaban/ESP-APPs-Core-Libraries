#include "ESPAPP_HTML_UI.h"

ESPAPP_HTML_UI::ESPAPP_HTML_UI(ESPAPP_Core *_System)
{
    System = _System;
}

ESPAPP_HTML_UI::~ESPAPP_HTML_UI()
{
}

void ESPAPP_HTML_UI::setLang(String *site, const __FlashStringHelper *lang)
{
    site->replace(F("{{s.lang}}"), lang);
}

void ESPAPP_HTML_UI::setRefresh(String *site, const __FlashStringHelper *refresh)
{
    site->replace(F("{{s.refresh}}"), refresh);
}

void ESPAPP_HTML_UI::setTitle(String *site, const __FlashStringHelper *title)
{
    site->replace(F("{{s.title}}"), title);
}

void ESPAPP_HTML_UI::setStyle(String *site, const __FlashStringHelper *css)
{
    site->replace(F("{{s.style}}"), css);
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

void ESPAPP_HTML_UI::embedCSSFiles(String *site, const char *cssFiles[], size_t count)
{
    String cssLinks;
    for (size_t i = 0; i < count; i++)
    {
        String cssTag = FPSTR(HTML_UI_SITE_CSS_FILE_TAG);
        cssTag.replace(F("{{c.a}}"), cssFiles[i]);
        cssLinks += cssTag;
    }
    site->replace(F("{{s.css}}"), cssLinks);
}

void ESPAPP_HTML_UI::embedJSFiles(String *site, const char *jsFiles[], size_t count)
{
    String jsLinks;
    for (size_t i = 0; i < count; i++)
    {
        String jsTag = FPSTR(HTML_UI_SITE_JS_FILE_TAG);
        jsTag.replace(F("{{c.a}}"), jsFiles[i]);
        jsLinks += jsTag;
    }
    site->replace(F("{{s.js}}"), jsLinks);
}

void ESPAPP_HTML_UI::clearOrphantTags(String *site)
{
    site->replace(F("{{s.lang}}"), F("en"));
    site->replace(F("{{s.refresh}}"), FPSTR(HTML_UI_EMPTY_STRING));
    site->replace(F("{{s.title}}"), F(ESPAPP_DEFAULT_DEVICE_NAME));
    site->replace(F("{{s.style}}"), FPSTR(HTML_UI_CSS));
    site->replace(F("{{s.css}}"), FPSTR(HTML_UI_EMPTY_STRING));
    site->replace(F("{{s.js}}"), FPSTR(HTML_UI_EMPTY_STRING));
    site->replace(F("{{f.logo}}"), FPSTR(HTML_UI_EMPTY_STRING));
    site->replace(F("{{f.subtitle-1}}"), FPSTR(HTML_UI_EMPTY_STRING));
    site->replace(F("{{f.subtitle-2}}"), FPSTR(HTML_UI_EMPTY_STRING));
    site->replace(F("{{menu}}"), FPSTR(HTML_UI_EMPTY_STRING));
    site->replace(F("{{f.url}}"), FPSTR(HTML_UI_EMPTY_STRING));
    site->replace(F("{{f.logo-url}}"), FPSTR(HTML_UI_EMPTY_STRING));
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
    item->replace(F("{{s.u}}"), url);
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
    site->replace(F("{{T}}"), title);
    site->replace(F("{{D}}"), description);
}

void ESPAPP_HTML_UI::openSection(String *site,
                                 const __FlashStringHelper *title,
                                 const __FlashStringHelper *description)
{
    site->concat(FPSTR(HTML_UI_SECTION_START));
    site->replace(F("{{T}}"), title);
    site->replace(F("{{D}}"), description);
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
                                      const char *name, const char *label,
                                      const char *value, const char *size,
                                      const char *min, const char *max,
                                      const char *step, const char *hint,
                                      boolean readonly)
{

    item->concat(FPSTR(HTML_UI_FORM_ITEM_INPUT));

    /** Basic Input */
    item->replace(FPSTR(HTML_UI_TAG_TITLE), label);
    item->replace(FPSTR(HTML_UI_TAG_NAME), name);
    item->replace(F("{{it}}"), type);
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

        item->replace(FPSTR(HTML_UI_TAG_HINT), FPSTR(HTML_UI_FORM_ITEM_INPUT_HINT));
        item->replace(FPSTR(HTML_UI_TAG_VALUE), hint);
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

                item->replace(FPSTR(HTML_UI_TAG_HINT), FPSTR(HTML_UI_FORM_ITEM_INPUT_HINT));
                item->replace(FPSTR(HTML_UI_TAG_VALUE), FPSTR(HTML_UI_FORM_ITEM_INPUT_HINT_RANGE_NUMBER));
                item->replace(F("{{l}}"), min);
                item->replace(F("{{m}}"), max);
            }
            else if (strcmp(min, (PGM_P)FPSTR(HTML_UI_INPUT_SKIP_ATTRIBUTE)) != 0)
            {
                item->replace(FPSTR(HTML_UI_TAG_HINT), FPSTR(HTML_UI_FORM_ITEM_INPUT_HINT));
                item->replace(FPSTR(HTML_UI_TAG_VALUE), FPSTR(HTML_UI_FORM_ITEM_INPUT_HINT_MIN_NUMBER));
                item->replace(F("{{v}}"), min);
            }
            else if (strcmp(max, (PGM_P)FPSTR(HTML_UI_INPUT_SKIP_ATTRIBUTE)) != 0)
            {
                item->replace(FPSTR(HTML_UI_TAG_HINT), FPSTR(HTML_UI_FORM_ITEM_INPUT_HINT));
                item->replace(FPSTR(HTML_UI_TAG_VALUE), FPSTR(HTML_UI_FORM_ITEM_INPUT_HINT_MAX_NUMBER));
                item->replace(F("{{v}}"), max);
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
                item->replace(FPSTR(HTML_UI_TAG_HINT), FPSTR(HTML_UI_FORM_ITEM_INPUT_HINT));
                item->replace(FPSTR(HTML_UI_TAG_VALUE), FPSTR(HTML_UI_FORM_ITEM_INPUT_HINT_RANGE_TEXT));
                item->replace(F("{{l}}"), min);
                item->replace(F("{{m}}"), max);
            }
            else if (strcmp(min, (PGM_P)FPSTR(HTML_UI_INPUT_SKIP_ATTRIBUTE)) != 0)
            {
                item->replace(FPSTR(HTML_UI_TAG_HINT), FPSTR(HTML_UI_FORM_ITEM_INPUT_HINT));
                item->replace(FPSTR(HTML_UI_TAG_VALUE), FPSTR(HTML_UI_FORM_ITEM_INPUT_HINT_MIN_TEXT));
                item->replace(F("{{v}}"), min);
            }
            else if (strcmp(max, (PGM_P)FPSTR(HTML_UI_INPUT_SKIP_ATTRIBUTE)) != 0)
            {
                item->replace(FPSTR(HTML_UI_TAG_HINT), FPSTR(HTML_UI_FORM_ITEM_INPUT_HINT));
                item->replace(FPSTR(HTML_UI_TAG_VALUE), FPSTR(HTML_UI_FORM_ITEM_INPUT_HINT_MAX_TEXT));
                item->replace(F("{{v}}"), max);
            }
        }
    }
    // Clear unused attributes
    item->replace(F("{{im}}"), FPSTR(HTML_UI_EMPTY_STRING));
    item->replace(F("{{ix}}"), FPSTR(HTML_UI_EMPTY_STRING));
    item->replace(F("{{ist}}"), FPSTR(HTML_UI_EMPTY_STRING));
    item->replace(F("{{is}}"), FPSTR(HTML_UI_EMPTY_STRING));
    item->replace(FPSTR(HTML_UI_TAG_HINT), FPSTR(HTML_UI_EMPTY_STRING));
}
void ESPAPP_HTML_UI::_addSelectionFormItem(
    String *item, boolean type, const char *name, const char *label,
    const char *value, boolean checked, const char *hint, boolean disabled)
{
    item->concat(FPSTR(HTML_UI_FORM_ITEM_CHECKBOX));
    item->replace(F("{{i.t}}"), type ? F("checkbox") : F("radio"));
    item->replace(F("{{i.n}}"), name);
    item->replace(F("{{i.l}}"), label);
    item->replace(F("{{i.v}}"), value);
    item->replace(F("{{i.c}}"), checked ? F(" checked=\"checked\"") : F(""));
    item->replace(F("{{i.d}}"), disabled ? F(" disabled=\"disabled\"") : F(""));
    item->replace(F("{{i.h}}"),
                  strcmp(hint, (PGM_P)FPSTR(HTML_UI_INPUT_SKIP_ATTRIBUTE)) != 0
                      ? "<span class=\"hint\">(" + String(hint) + ")</span>"
                      : "");
}

void ESPAPP_HTML_UI::addCheckboxFormItem(String *item, const char *name,
                                         const char *label,
                                         const char *value, boolean checked,
                                         const char *hint,
                                         boolean disabled)
{
    item->concat(FPSTR(HTML_UI_FORM_ITEM_CHECKBOX));
    item->replace(FPSTR(HTML_UI_TAG_TITLE), label);
    item->replace(FPSTR(HTML_UI_TAG_NAME), name);
    item->replace(FPSTR(HTML_UI_TAG_VALUE), value);
    item->replace(F("{{i.c}}"), checked ? FPSTR(HTML_UI_FORM_ITEM_ATTRIBUTE_CHECKED) : FPSTR(HTML_UI_EMPTY_STRING));
    item->replace(F("{{i.d}}"), disabled ? FPSTR(HTML_UI_FORM_ITEM_ATTRIBUTE_DISABLED) : FPSTR(HTML_UI_EMPTY_STRING));
    if (strcmp(hint, (PGM_P)FPSTR(HTML_UI_INPUT_SKIP_ATTRIBUTE)) != 0)
    {
        item->replace(FPSTR(HTML_UI_TAG_HINT), FPSTR(HTML_UI_FORM_ITEM_INPUT_HINT));
        item->replace(FPSTR(HTML_UI_TAG_VALUE), hint);
    }
    else
    {
        item->replace(FPSTR(HTML_UI_TAG_HINT), FPSTR(HTML_UI_EMPTY_STRING));
    }
}

void ESPAPP_HTML_UI::addRadioButtonFormItem(
    String *item, const char *name, const char *label, const char *value,
    boolean checked, const char *hint, boolean disabled)
{
    this->_addSelectionFormItem(item, false, name, label, value, checked, hint,
                                disabled);
}

void ESPAPP_HTML_UI::addSelectFormItemOpen(
    String *item, const __FlashStringHelper *name,
    const __FlashStringHelper *label)
{
    item->concat(FPSTR(HTML_UI_ITEM_SELECT_OPEN));
    item->replace(F("{{i.l}}"), label);
    item->replace(F("{{i.n}}"), name);
}

void ESPAPP_HTML_UI::addSelectOptionFormItem(String *item, const char *label,
                                             const char *value,
                                             boolean selected)
{
    item->concat(FPSTR(HTML_UI_ITEM_SELECT_OPTION));
    item->replace(F("{{i.v}}"), value);
    item->replace(F("{{i.l}}"), label);
    item->replace(F("{{i.s}}"), selected ? F(" selected=\"selected\"") : F(""));
}

void ESPAPP_HTML_UI::addSelectFormItemClose(String *item)
{
    item->concat(FPSTR(HTML_UI_ITEM_SELECT_CLOSE));
}

void ESPAPP_HTML_UI::addParagraph(String *item, const __FlashStringHelper *text)
{
    item->concat(FPSTR(HTML_UI_ITEM_PARAGRAPH));
    item->replace(F("{{i.v}}"), text);
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
    site->replace(F("{{i.v}}"), item);
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
    site->replace(F("{{f.n}}"), name);
    site->replace(F("{{f.s}}"), String(size));
}

void ESPAPP_HTML_UI::addFileExplorerFileItem(String *site, const char *file, size_t size, const char *directory)
{
    site->concat(FPSTR(HTML_UI_FILE_EXPLORER_FILE_ITEM));
    site->replace(F("{{f.n}}"), file);
    site->replace(F("{{f.s}}"), String(size));
    site->replace(F("{{f.d}}"), directory);
}

void ESPAPP_HTML_UI::addFileExplorerUploadForm(String *site, const char *directory)
{
    site->concat(FPSTR(HTML_UI_FILE_EXPLORER_UPLOAD_FORM));
    site->replace(F("{{f.d}}"), directory);
}

/** Private */
void ESPAPP_HTML_UI::replaceTagTitle(String *item, const __FlashStringHelper *title)
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

void ESPAPP_HTML_UI::setUrlParams(ESPAPP_HTTP_REQUEST *url, uint8_t siteId, uint8_t command, uint8_t action, uint8_t option)
{
    url->siteId = siteId;
    url->command = command;
    url->action = action;
    url->option = option;
}