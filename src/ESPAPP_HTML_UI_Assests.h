#ifndef _ESPAPP_HTML_UI_ASSETS_h
#define _ESPAPP_HTML_UI_ASSETS_h

/* Build on https://pure-css.github.io/start/*/

/* Form constants */
const char HTML_UI_EMPTY_STRING[] PROGMEM = "";

/** Form input: types */
const char HTML_UI_INPUT_TYPE_NUMBER[] PROGMEM = "number";
const char HTML_UI_INPUT_TYPE_TEXT[] PROGMEM = "text";
const char HTML_UI_INPUT_TYPE_PASSWORD[] PROGMEM = "password";
const char HTML_UI_INPUT_TYPE_CHECKBOX[] PROGMEM = "checkbox";
const char HTML_UI_INPUT_TYPE_RADIO[] PROGMEM = "radio";

/** Form input: attributes */
const char HTML_UI_INPUT_SKIP_ATTRIBUTE[] PROGMEM = "?";
const char HTML_UI_FORM_ITEM_ATTRIBUTE_CHECKED[] PROGMEM = " checked=\"checked\"";
const char HTML_UI_FORM_ITEM_ATTRIBUTE_DISABLED[] PROGMEM = " disabled=\"disabled\"";

const char HTML_UI_SUBMITT_BUTTON_SAVE[] PROGMEM = "Save";

/** TAGS */
const char HTML_UI_TAG_TYPE[] PROGMEM = "{{it}}";
const char HTML_UI_TAG_TITLE[] PROGMEM = "{{t}}";
const char HTML_UI_TAG_ICON[] PROGMEM = "{{i}}";
const char HTML_UI_TAG_VALUE[] PROGMEM = "{{v}}";
const char HTML_UI_TAG_NAME[] PROGMEM = "{{n}}";
const char HTML_UI_TAG_HINT[] PROGMEM = "{{h}}";

/** HTML TAGS templates */
const char HTML_UI_SITE_CSS_FILE_TAG[] PROGMEM = "<link rel=\"stylesheet\" href=\"{{c.a}}\">";
const char HTML_UI_SITE_JS_FILE_TAG[] PROGMEM = "<script src=\"{{c.a}}\"></script>";

/** Icons */
const char HTML_UI_NO_ICON[] PROGMEM = "";
const char HTML_UI_ICON_RIGHTWARDS_ARROW[] PROGMEM = "&#10150; ";
const char HTML_UI_ICON_ARROW[] PROGMEM = "&#8227; ";

/* Sites structure */
const char HTML_UI_SITE_HEADER[] PROGMEM =
    "<!doctype html><html lang=\"{{s.lang}}\"><head><meta charset=\"utf-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><link href=\"https://fonts.googleapis.com/css2?family=Titillium+Web&display=swap\" rel=\"stylesheet\"><title>{{s.title}}</title>{{s.refresh}}{{s.css}}{{s.js}}</head><body><div id=\"layout\">";

const char HTML_UI_SITE_BODY_START[] PROGMEM = "<div id=\"main\" class=\"c\">";

const char HTML_UI_SITE_FOOTER[] PROGMEM = "</div></div></body></html>";


/** Navigation Block */
const char HTML_UI_SITE_MENU_BLOCK_START[] PROGMEM = "<a href=\"#menu\" id=\"menuLink\" class=\"menu-link\"><span></span></a><div id=\"menu\"><header><img src=\"{{f.logo}}\"alt=\"{{s.title}}\"><small>{{f.subtitle-1}}<br/>{{f.subtitle-2}}</small></header>";
const char HTML_UI_SITE_MENU_BLOCK_END[] PROGMEM = "</div>";

const char HTML_UI_SITE_MENU_SECTION_START[] PROGMEM = "<div class=\"pure-menu\"><a class=\"pure-menu-heading\" href=\"#{{t}}\">{{i}}{{t}}</a><ul class=\"pure-menu-list\">";
const char HTML_UI_SITE_MENU_SECTION_END[] PROGMEM = "</ul></div>";

const char HTML_UI_SITE_MENU_SECTION_ITEM[] PROGMEM = "<li class=\"pure-menu-item\"><a href=\"/?{{mi.s}}{{mi.c}}{{mi.a}}{{mi.o}}{{mi.p}}\" class=\"pure-menu-link\">{{i}}{{t}}</a></li>";
const char HTML_UI_SITE_MENU_SECTION_ITEM_EXTERNAL[] PROGMEM = "<li class=\"pure-menu-item\"><a href=\"{{s.u}}\" class=\"pure-menu-link\">{{i}}{{t}}</a></li>";

/** Section */
const char HTML_UI_SECTION_START[] PROGMEM = "<div><h1 class=\"cs\">{{T}}</h1><section class=\"s\"><header><h2 class=\"st\">{{D}}</h2></header><div class=\"sc\">";
const char HTML_UI_SECTION_END[] PROGMEM = "</div></section></div>";

/** Forms */
const char HTML_UI_FORM_START[] PROGMEM = "<form class=\"pure-form pure-form-aligned\" method=\"post\" action=\"/?{{mi.s}}{{mi.c}}{{mi.a}}{{mi.o}}{{mi.p}}\">";
const char HTML_UI_FORM_END[] PROGMEM = "<div class=\"pure-controls\"><button type=\"submit\" class=\"pure-button pure-button-primary\">{{t}}</button></div></form>";

/** Form: input */
const char HTML_UI_FORM_ITEM_INPUT[] PROGMEM =
    "<div class=\"pure-control-group\"><label>{{t}}</label><input name=\"{{n}}\" type=\"{{it}}\" value=\"{{v}}\"{{im}}{{ix}}{{ist}}{{is}}{{ir}}>{{h}}</div>";

const char HTML_UI_FORM_ITEM_INPUT_ATTRIBUTE_READONLY[] PROGMEM = " readonly=\"readonly\"";
const char HTML_UI_FORM_ITEM_INPUT_ATTRIBUTE_MIN[] PROGMEM = " min=\"{{v}}\"";
const char HTML_UI_FORM_ITEM_INPUT_ATTRIBUTE_MAX[] PROGMEM = " max=\"{{v}}\"";
const char HTML_UI_FORM_ITEM_INPUT_ATTRIBUTE_STEP[] PROGMEM = " step=\"{{v}}\"";
const char HTML_UI_FORM_ITEM_INPUT_ATTRIBUTE_SIZE[] PROGMEM = " size=\"{{v}}\"";
const char HTML_UI_FORM_ITEM_INPUT_ATTRIBUTE_MINLENGTH[] PROGMEM = " minlength=\"{{v}}\"";
const char HTML_UI_FORM_ITEM_INPUT_ATTRIBUTE_MAXLENGTH[] PROGMEM = " maxlength=\"{{v}}\"";

const char HTML_UI_FORM_ITEM_INPUT_HINT[] PROGMEM = "<span class=\"pure-form-message-inline\">{{v}}</span>";
const char HTML_UI_FORM_ITEM_INPUT_HINT_MAX_TEXT[] PROGMEM = "(Max: {{v}} chars)";
const char HTML_UI_FORM_ITEM_INPUT_HINT_MIN_TEXT[] PROGMEM = "(Min: {{v}} chars)";
const char HTML_UI_FORM_ITEM_INPUT_HINT_RANGE_TEXT[] PROGMEM = "(Range: {{l}} - {{m}} chars)";
const char HTML_UI_FORM_ITEM_INPUT_HINT_MIN_NUMBER[] PROGMEM = "(Min: {{v}})";
const char HTML_UI_FORM_ITEM_INPUT_HINT_MAX_NUMBER[] PROGMEM = "(Max: {{v}})";
const char HTML_UI_FORM_ITEM_INPUT_HINT_RANGE_NUMBER[] PROGMEM = "(Range: {{l}} - {{m}})";


/** Form: Radiobutton and Checkbox */
const char HTML_UI_FORM_ITEM_CHECK_OR_RADIO_BOX[] PROGMEM =
    "<div class=\"pure-control-group\"><label></label><input name=\"{{n}}\" type=\"{{it}}\" value=\"{{v}}\"{{i.c}}{{i.d}} /> {{t}}{{h}}</div>";

/** Form: Paragraph */
const char HTML_UI_ITEM_PARAGRAPH[] PROGMEM = "<p>{{v}}</p>";
const char HTML_UI_FORM_ITEM_PARAGRAPH[] PROGMEM = "<div class=\"pure-control-group\"><label></label>{{v}}</div>";

/** Form: Select */    
const char HTML_UI_ITEM_SELECT_OPEN[] PROGMEM =
        "<div class=\"pure-control-group\"><label>{{t}}</label><select name=\"{{n}}\">";
const char HTML_UI_ITEM_SELECT_OPTION[] PROGMEM = "<option value=\"{{v}}\" {{i.s}}>{{t}}</option>";
const char HTML_UI_ITEM_SELECT_CLOSE[] PROGMEM = "</select>{{h}}</div>";

/** Lists */
const char HTML_UI_ITEM_LIST_START[] PROGMEM = "<fieldset><ul class=\"lst\">";
const char HTML_UI_ITEM_LIST_END[] PROGMEM = "</ul></fieldset>";
const char HTML_UI_ITEM_LIST_ITEM[] PROGMEM = "<li class=\"m\">{{v}}</li>";

/** Files explorer */
const char HTML_UI_FILE_EXPLORER_HEADER[] PROGMEM = "<table class=\"pure-table pure-table-horizontal\"><thead><tr><th>File name</th><th>Size</th><th>Acton</th></tr></thead><tbody>";
const char HTML_UI_FILE_EXPLORER_FOLDER_ITEM[] PROGMEM = "<tr><td><a href=\"/?site=0&p1={{f.n}}\">{{f.n}}</a></td><td>{{f.s}} files</td><td>Delete</td></tr>";
const char HTML_UI_FILE_EXPLORER_FILE_ITEM[] PROGMEM = "<tr><td>{{f.n}}</td><td>{{f.s}}B</td><td><form action=\"/?site=0&cmd=1\" method=\"POST\"><input type=\"hidden\" name=\"f\" value=\"{{f.n}}\"><input type=\"hidden\" name=\"directory\" value=\"{{f.d}}\"><button class=\"pure-button pure-button-primary\" type=\"submit\">Delete</button></form></td></tr>";
const char HTML_UI_FILE_EXPLORER_FOOTER[] PROGMEM = "</tbody></table>";

const char HTML_UI_FILE_EXPLORER_UPLOAD_FORM[] PROGMEM =
    "<form action=\"/upload?site=0&cmd=0\" method=\"POST\" enctype=\"multipart/form-data\">"
    "<input type=\"file\" name=\"uploadedFile\">"
    "<input type=\"hidden\" name=\"directory\" value=\"{{f.d}}\">"
    "<button class=\"pure-button pure-button-primary\" type=\"submit\">Upload</button>"
    "</form>";
#endif