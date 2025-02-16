#ifndef _ESPAPP_HTML_UI_H_
#define _ESPAPP_HTML_UI_H_

#include <ESPAPP_Core.h>
#include <ESPAPP_HTML_UI_Assests.h>

class ESPAPP_HTML_UI
{
private:
    ESPAPP_Core *System;

    void clearOrphantTags(String *site);
    void replaceTagTitle(String *item, const __FlashStringHelper *title);
    void replaceTagIcon(String *item, const __FlashStringHelper *icon);
    void replaceTagUrlParams(String *item, ESPAPP_HTTP_REQUEST *url, const char *parameters);
    void _addSelectionFormItem(String *item, boolean type, const char *name,
                               const char *label, const char *value,
                               boolean checked,
                               const char *hint = ESPAPP_FORM_SKIP_ATTRIBUTE,
                               boolean disabled = false);

public:
    ESPAPP_HTML_UI(ESPAPP_Core *_System);
    ~ESPAPP_HTML_UI();

    void setLang(String *site, const __FlashStringHelper *lang);
    void setRefresh(String *site, const __FlashStringHelper *refresh);
    void setTitle(String *site, const __FlashStringHelper *title);
    void setCSS(String *site, const __FlashStringHelper *css);
    void setLogo(String *site, const __FlashStringHelper *logo);
    void setSubtitle1(String *site, const __FlashStringHelper *subtitle1);
    void setSubtitle2(String *site, const __FlashStringHelper *subtitle2);
    void setMenu(String *site, const __FlashStringHelper *menu);
    void setBody(String *site, const __FlashStringHelper *body);
    void setURL(String *site, const __FlashStringHelper *url);
    void setLogoURL(String *site, const __FlashStringHelper *logoURL);
    void setUrlParams(ESPAPP_HTTP_REQUEST *url, uint8_t siteId = ESPAPP_NONE, uint8_t command = ESPAPP_NONE, uint8_t action = ESPAPP_NONE, uint8_t option = ESPAPP_NONE);

    /** Site level */
    void startHeaderSection(String *site);
    void startBodySection(String *site);
    void siteEnd(String *site);

    /** Menu */
    void startMenuSection(String *item, const __FlashStringHelper *title, const __FlashStringHelper *icon);
    void endMenuSection(String *item);

    void addMenuSubMenuHeader(String *item, const __FlashStringHelper *title);
    void addMenuItem(String *item, const __FlashStringHelper *title, ESPAPP_HTTP_REQUEST *url, const char *parameters, const __FlashStringHelper *icon);
    void addMenuItemExternal(String *item, const __FlashStringHelper *title,
                             const __FlashStringHelper *url, const __FlashStringHelper *icon);


    /** Section */
    void openSection(String *site, const char *title,
                     const __FlashStringHelper *description);

    void openSection(String *site, const __FlashStringHelper *title,
                     const __FlashStringHelper *description);

    void closeSection(String *site);

    /** Message Section */
    void openMessageSection(String *site, const char *title,
                            const __FlashStringHelper *description);

    void openMessageSection(String *site, const __FlashStringHelper *title,
                            const __FlashStringHelper *description);

    void closeMessageSection(String *site);

    void startList(String *site);
    void endList(String *site);
    void addListItem(String *site, const char *item);

    /** Form */
    void startForm(String *site, ESPAPP_HTTP_REQUEST *url, const char *parameters);
    void endForm(String *site, const __FlashStringHelper *submiButtonLabel);

    /** Files Explorer */
    void startFileExplorer(String *site);
    void endFileExplorer(String *site);
    void addFileExplorerFolderItem(String *site, const char *name, size_t size);
    void addFileExplorerFileItem(String *site, const char *name, size_t size);

    /** Form's items */
    void addInputFormItem(String *item, const char *type, const char *name,
                          const char *label, const char *value,
                          const char *size = ESPAPP_FORM_SKIP_ATTRIBUTE,
                          const char *min = ESPAPP_FORM_SKIP_ATTRIBUTE,
                          const char *max = ESPAPP_FORM_SKIP_ATTRIBUTE,
                          const char *step = ESPAPP_FORM_SKIP_ATTRIBUTE,
                          const char *hint = ESPAPP_FORM_SKIP_ATTRIBUTE,
                          boolean readonly = false);

    void addCheckboxFormItem(String *item, const char *name, const char *label,
                             const char *value, boolean checked,
                             const char *hint = ESPAPP_FORM_SKIP_ATTRIBUTE,
                             boolean disabled = false);

    void addRadioButtonFormItem(String *item, const char *name, const char *label,
                                const char *value, boolean checked,
                                const char *hint = ESPAPP_FORM_SKIP_ATTRIBUTE,
                                boolean disabled = false);

    void addSelectFormItemOpen(String *item, const __FlashStringHelper *name,
                               const __FlashStringHelper *label);
    void addSelectOptionFormItem(String *item, const char *label,
                                 const char *value, boolean selected);
    void addSelectFormItemClose(String *item);

    void addParagraph(String *item, const __FlashStringHelper *text);
};

#endif