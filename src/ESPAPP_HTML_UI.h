#ifndef _ESPAPP_HTML_UI_H_
#define _ESPAPP_HTML_UI_H_

#include <ESPAPP_Core.h>
#include <ESPAPP_HTML_UI_Assests.h>

class ESPAPP_HTML_UI
{
private:
    ESPAPP_Core *System;
    bool isMenuSet = true;

    void clearOrphantTags(String *site);
    void replaceTagTitle(String *item, const __FlashStringHelper *title);
    void replaceTagTitle(String *item, const char *value);

    void replaceTagIcon(String *item, const __FlashStringHelper *icon);

    void replaceTagValue(String *item, const __FlashStringHelper *value);
    

    void replaceTagHint(String *item, const __FlashStringHelper *value);
    void replaceTagHint(String *item, const char *value);

    void replaceTagUrlParams(String *item, ESPAPP_HTTP_REQUEST *url, const char *parameters);

    void addRadioButtonOrCheckBoxFormItem(String *item, const __FlashStringHelper *type, const __FlashStringHelper *name, const char *label,
                                          const char *value, boolean checked,
                                          const char *hint = (PGM_P)FPSTR(HTML_UI_INPUT_SKIP_ATTRIBUTE),
                                          boolean disabled = false);

    void addParagraphTag(String *item, bool indented = false);

public:
    ESPAPP_HTML_UI(ESPAPP_Core *_System);
    ~ESPAPP_HTML_UI();

    void replaceTagValue(String *item, const char *value);

    void setLang(String *site, const __FlashStringHelper *lang);
    void setRefresh(String *site, uint8_t refresh, const char *url);
    void removeMenu(void);
    bool showMenu(void);
    void setTitle(String *site, const __FlashStringHelper *title);
    void setStyle(String *site, const __FlashStringHelper *css);
    void embedCSSFiles(String *site, const char *cssFiles[], size_t count);
    void embedJSFiles(String *site, const char *jsFiles[], size_t count);
    void setLogo(String *site, const __FlashStringHelper *logo);
    void setSubtitle1(String *site, const __FlashStringHelper *subtitle1);
    void setSubtitle2(String *site, const __FlashStringHelper *subtitle2);
    void setMenu(String *site, const __FlashStringHelper *menu);
    void setBody(String *site, const __FlashStringHelper *body);
    void setURL(String *site, const __FlashStringHelper *url);
    void setLogoURL(String *site, const __FlashStringHelper *logoURL);
    void setUrlParams(ESPAPP_HTTP_REQUEST *url, uint8_t siteId = ESPAPP_NONE, uint8_t command = ESPAPP_NONE, uint8_t action = ESPAPP_NONE, uint8_t option = ESPAPP_NONE);
    void setVersion(String *site, const __FlashStringHelper *version);

    /** Site level */
    void siteStart(String *site);
    void siteEnd(String *site);

    /** Navigation block */
    void startNavigationBlock(String *site);
    void endNavigationBlock(String *site);

    void startNavigationList(String *item, const __FlashStringHelper *title, const __FlashStringHelper *icon);
    void endNavigationList(String *item);

    void addNavigationItem(String *item, const __FlashStringHelper *title, ESPAPP_HTTP_REQUEST *url, const char *parameters, const __FlashStringHelper *icon);
    void addNavigationItemExternal(String *item, const __FlashStringHelper *title,
                                   const __FlashStringHelper *url, const __FlashStringHelper *icon);

    /** Body */
    void startBodySection(String *site);

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
    void addMessageItem(String *site, const char *item);
 
    /** Show saved messages */
    void showSavedMessages(String *site, const __FlashStringHelper *title,
        const __FlashStringHelper *description);
     

    /** List */
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
    void addFileExplorerFileItem(String *site, const char *file, size_t size, const char *directory);
    void addFileExplorerUploadForm(String *site, const char *directory);

    /** Form's items */
    void addInputFormItem(String *item, const __FlashStringHelper *type, const __FlashStringHelper *name,
                          const char *label, const char *value,
                          const char *size = (PGM_P)FPSTR(HTML_UI_INPUT_SKIP_ATTRIBUTE),
                          const char *min = (PGM_P)FPSTR(HTML_UI_INPUT_SKIP_ATTRIBUTE),
                          const char *max = (PGM_P)FPSTR(HTML_UI_INPUT_SKIP_ATTRIBUTE),
                          const char *step = (PGM_P)FPSTR(HTML_UI_INPUT_SKIP_ATTRIBUTE),
                          const char *hint = (PGM_P)FPSTR(HTML_UI_INPUT_SKIP_ATTRIBUTE),
                          boolean readonly = false);

    void addCheckBoxFormItem(String *item, const __FlashStringHelper  *name, const char *label,
                             const char *value, boolean checked,
                             const char *hint = (PGM_P)FPSTR(HTML_UI_INPUT_SKIP_ATTRIBUTE),
                             boolean disabled = false);

    void addRadioButtonFormItem(String *item, const __FlashStringHelper  *name, const char *label,
                                const char *value, boolean checked,
                                const char *hint = (PGM_P)FPSTR(HTML_UI_INPUT_SKIP_ATTRIBUTE),
                                boolean disabled = false);

    void addSelectFormItemOpen(String *item, const __FlashStringHelper *name,
                               const __FlashStringHelper *label);

    void addSelectOptionFormItem(String *item, const char  *label,
                                 const char *value, boolean selected);

    void addSelectFormItemClose(String *item, const char *hint = (PGM_P)FPSTR(HTML_UI_INPUT_SKIP_ATTRIBUTE));

    void addParagraph(String *item, const __FlashStringHelper *text, bool indented = false);
    void addParagraph(String *item, const char *text, bool indented = false);
};

#endif