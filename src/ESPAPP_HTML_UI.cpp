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

void ESPAPP_HTML_UI::setCSS(String *site, const __FlashStringHelper *css)
{
    site->replace(F("{{s.css}}"), css);
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

void ESPAPP_HTML_UI::clearOrphantTags(String *site)
{
    site->replace(F("{{s.lang}}"), F("en"));
    site->replace(F("{{s.refresh}}"), FPSTR(HTML_UI_EMPTY_STRING));
    site->replace(F("{{s.title}}"), F(ESPAPP_DEFAULT_DEVICE_NAME));
    site->replace(F("{{s.css}}"), FPSTR(HTML_UI_CSS));
    site->replace(F("{{f.logo}}"), FPSTR(HTML_UI_EMPTY_STRING));
    site->replace(F("{{f.subtitle-1}}"), FPSTR(HTML_UI_EMPTY_STRING));
    site->replace(F("{{f.subtitle-2}}"), FPSTR(HTML_UI_EMPTY_STRING));
    site->replace(F("{{menu}}"), FPSTR(HTML_UI_EMPTY_STRING));
    site->replace(F("{{f.url}}"), FPSTR(HTML_UI_EMPTY_STRING));
    site->replace(F("{{f.logo-url}}"), FPSTR(HTML_UI_EMPTY_STRING));
}

//------------------------------------------------------------

void ESPAPP_HTML_UI::startHeaderSection(String *site)
{
    site->concat(FPSTR(HTML_UI_SITE_HEADER_START));
}

void ESPAPP_HTML_UI::startBodySection(String *site)
{
    site->concat(FPSTR(HTML_UI_SITE_BODY_START));
}

void ESPAPP_HTML_UI::siteEnd(String *site)
{
    site->concat(FPSTR(HTML_UI_SITE_END));
    this->clearOrphantTags(site);
}

/** Menu realted methods */

void ESPAPP_HTML_UI::startMenuSection(String *item, const __FlashStringHelper *title, const __FlashStringHelper *icon)
{
    item->concat(FPSTR(HTML_UI_MENU_START));
    this->replaceTagTitle(item, title);
    this->replaceTagIcon(item, icon);
}

void ESPAPP_HTML_UI::endMenuSection(String *item)
{
    item->concat(FPSTR(HTML_UI_MENU_END));
}

void ESPAPP_HTML_UI::addMenuSubMenuHeader(String *item, const __FlashStringHelper *title)
{
    item->concat(FPSTR(HTML_UI_MENU_SUBMENU_HEADER));
    this->replaceTagTitle(item, title);
}

void ESPAPP_HTML_UI::addMenuItem(String *item, const __FlashStringHelper *title, ESPAPP_HTTP_REQUEST *url, const char *parameters, const __FlashStringHelper *icon)
{
    item->concat(FPSTR(HTML_UI_MENU_ITEM));
    this->replaceTagTitle(item, title);
    this->replaceTagUrlParams(item, url, parameters);
    this->replaceTagIcon(item, icon);
}

void ESPAPP_HTML_UI::addMenuItemExternal(String *item,
                                         const __FlashStringHelper *title,
                                         const __FlashStringHelper *url, const __FlashStringHelper *icon)
{
    item->concat(FPSTR(HTML_UI_MENU_ITEM_EXTERNAL));
    this->replaceTagTitle(item, title);
    this->replaceTagIcon(item, icon);
    item->replace(F("{{s.u}}"), url);
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
    site->replace(F("{{b.t}}"), submiButtonLabel);
}

void ESPAPP_HTML_UI::addInputFormItem(String *item, const char *type,
                                      const char *name, const char *label,
                                      const char *value, const char *size,
                                      const char *min, const char *max,
                                      const char *step, const char *hint,
                                      boolean readonly)
{
    item->concat(F("<div class=\"cf\"><label>"));
    item->concat(label);
    item->concat(F("</label><input name=\""));
    item->concat(name);
    item->concat(F("\" type=\""));
    item->concat(type);
    item->concat(F("\" "));
    if (readonly)
    {
        item->concat(F("readonly=\"readonly\" "));
    }
    if (strcmp(size, "?") != 0)
    {
        item->concat(F("maxlength=\""));
        item->concat(size);
        item->concat(F("\" "));
    }
    if (strcmp(type, "number") == 0)
    {
        if (strcmp(min, "?") != 0)
        {
            item->concat(F("min=\""));
            item->concat(min);
            item->concat(F("\" "));
        }
        if (strcmp(max, "?") != 0)
        {
            item->concat(F("max=\""));
            item->concat(max);
            item->concat(F("\" "));
        }
        if (strcmp(step, "?") != 0)
        {
            item->concat(F("step=\""));
            item->concat(step);
            item->concat(F("\" "));
        }
    }
    item->concat(F("value=\""));
    item->concat(value);
    item->concat(F("\">"));
    if (strcmp(size, "?") != 0)
    {
        item->concat(F("<span class=\"hint\">Max "));
        item->concat(size);
        item->concat(F(" "));
        item->concat(F("chars"));
        item->concat(F("</span>"));
    }
    if (strcmp(type, "number") == 0)
    {
        if (strcmp(min, "?") != 0 && strcmp(max, "?") != 0)
        {
            item->concat(F("<span class=\"hint\">"));
            item->concat(F("range"));
            item->concat(F(" "));
            item->concat(min);
            item->concat(F(" - "));
            item->concat(max);
            if (strcmp(hint, "?") != 0)
            {
                item->concat(F(" "));
                item->concat(hint);
            }
            item->concat(F("</span>"));
        }
        else if (strcmp(hint, "?") != 0)
        {
            item->concat(F("<span class=\"hint\">"));
            item->concat(hint);
            item->concat(F("</span>"));
        }
    }
    item->concat(F("</div>"));
}

void ESPAPP_HTML_UI::_addSelectionFormItem(
    String *item, boolean type, const char *name, const char *label,
    const char *value, boolean checked, const char *hint, boolean disabled)
{
    item->concat(FPSTR(HTML_UI_ITEM_CHECKBOX));
    item->replace(F("{{i.t}}"), type ? F("checkbox") : F("radio"));
    item->replace(F("{{i.n}}"), name);
    item->replace(F("{{i.l}}"), label);
    item->replace(F("{{i.v}}"), value);
    item->replace(F("{{i.c}}"), checked ? F(" checked=\"checked\"") : F(""));
    item->replace(F("{{i.d}}"), disabled ? F(" disabled=\"disabled\"") : F(""));
    item->replace(F("{{i.h}}"),
                  strcmp(hint, ESPAPP_FORM_SKIP_ATTRIBUTE) != 0
                      ? "<span class=\"hint\">(" + String(hint) + ")</span>"
                      : "");
}

void ESPAPP_HTML_UI::addCheckboxFormItem(String *item, const char *name,
                                         const char *label,
                                         const char *value, boolean checked,
                                         const char *hint,
                                         boolean disabled)
{
    this->_addSelectionFormItem(item, true, name, label, value, checked, hint,
                                disabled);
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

void ESPAPP_HTML_UI::startList(String *site) {
    site->concat(FPSTR(HTML_UI_ITEM_LIST_START));
}

void ESPAPP_HTML_UI::endList(String *site) {
    site->concat(FPSTR(HTML_UI_ITEM_LIST_END));
}

void ESPAPP_HTML_UI::addListItem(String *site, const char *item) { 
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

void ESPAPP_HTML_UI::addFileExplorerFileItem(String *site, const char *name, size_t size)
{
    site->concat(FPSTR(HTML_UI_FILE_EXPLORER_FILE_ITEM));
    site->replace(F("{{f.n}}"), name);
    site->replace(F("{{f.s}}"), String(size));
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