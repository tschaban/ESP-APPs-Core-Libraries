#ifndef _ESPAPP_HTML_UI_ASSETS_h
#define _ESPAPP_HTML_UI_ASSETS_h

/* Build on https://pure-css.github.io/start/*/

/* Form constants */
const char HTML_UI_EMPTY_STRING[] PROGMEM = "";
const char HTML_UI_SLASH_CHAR[] PROGMEM = "/";

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

/** Form items: name tags */
const char HTML_UI_FORM_INPUT_COMMON_0[] PROGMEM = "c0";
const char HTML_UI_FORM_INPUT_COMMON_1[] PROGMEM = "c1";
const char HTML_UI_FORM_INPUT_COMMON_2[] PROGMEM = "c2";
const char HTML_UI_FORM_INPUT_COMMON_3[] PROGMEM = "c3";
const char HTML_UI_FORM_INPUT_COMMON_4[] PROGMEM = "c4";
const char HTML_UI_FORM_INPUT_COMMON_5[] PROGMEM = "c5";
const char HTML_UI_FORM_INPUT_COMMON_6[] PROGMEM = "c6";
const char HTML_UI_FORM_INPUT_COMMON_7[] PROGMEM = "c7";
const char HTML_UI_FORM_INPUT_COMMON_8[] PROGMEM = "c8";
const char HTML_UI_FORM_INPUT_COMMON_9[] PROGMEM = "c9";

/** TAGS */
const char HTML_UI_TAG_TYPE[] PROGMEM = "{{it}}";
const char HTML_UI_TAG_TITLE[] PROGMEM = "{{t}}";
const char HTML_UI_TAG_ICON[] PROGMEM = "{{i}}";
const char HTML_UI_TAG_VALUE[] PROGMEM = "{{v}}";
const char HTML_UI_TAG_NAME[] PROGMEM = "{{n}}";
const char HTML_UI_TAG_HINT[] PROGMEM = "{{h}}";

/** HTML TAGS templates */
const char HTML_UI_SITE_CSS_FILE_TAG[] PROGMEM = "<link rel=\"stylesheet\" href=\"{{v}}\">";
const char HTML_UI_SITE_JS_FILE_TAG[] PROGMEM = "<script src=\"{{v}}\"></script>";
const char HTML_UI_SITE_URL_TAG[] PROGMEM = "<a href=\"{{v}}\">{{t}}</a>";
const char HTML_UI_SITE_REFRESH_TAG[] PROGMEM = "<meta http-equiv=\"refresh\" content=\"{{v}};URL='{{t}}'\">";

/** Icons */
const char HTML_UI_NO_ICON[] PROGMEM = "";
const char HTML_UI_ICON_RIGHTWARDS_ARROW[] PROGMEM = "&#10150; ";
const char HTML_UI_ICON_ARROW[] PROGMEM = "&#8227; ";
const char HTML_UI_ICON_DISK[] PROGMEM ="&#128190; ";

/* Sites structure */

const char HTML_UI_SITE_HEADER_LIGHT[] PROGMEM =
    "<!DOCTYPE html><html><head><title>{{s.title}}</title><style>body{display:flex;justify-content:center;align-items:center;min-height:100vh;margin:0;font-family:sans-serif;background-color:#777}form{border:1px solid #000;background-color:#fefefe;padding:2em;text-align:center}select,input,button{width:300px;margin-bottom:.8em;padding:.4em}form *{box-sizing:border-box}label{display:block;text-align:left;width:auto}</style></head><body><div><div>";

const char HTML_UI_SITE_HEADER[] PROGMEM =
    "<!doctype html><html lang=\"{{s.lang}}\"><head><meta charset=\"utf-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><link href=\"https://fonts.googleapis.com/css2?family=Titillium+Web&display=swap\" rel=\"stylesheet\"><title>{{s.title}}</title>{{s.refresh}}{{s.css}}{{s.js}}</head><body><div id=\"layout\">";

const char HTML_UI_SITE_BODY_START[] PROGMEM = "<div id=\"main\" class=\"c\">";


const char HTML_UI_SITE_FOOTER[] PROGMEM = "</div></div></body></html>";


/** Navigation Block */
const char HTML_UI_SITE_MENU_BLOCK_START[] PROGMEM = "<a href=\"#menu\" id=\"menuLink\" class=\"menu-link\"><span></span></a><div id=\"menu\"><header><img src=\"{{f.logo}}\" alt=\"{{s.title}}\"><small>{{f.subtitle-1}}<br>{{f.subtitle-2}}<br>{{f.version}}</small></header>";
const char HTML_UI_SITE_MENU_BLOCK_END[] PROGMEM = "</div>";

const char HTML_UI_SITE_MENU_SECTION_START[] PROGMEM = "<div class=\"pure-menu\"><a class=\"pure-menu-heading\" href=\"#{{t}}\">{{i}}{{t}}</a><ul class=\"pure-menu-list\">";
const char HTML_UI_SITE_MENU_SECTION_END[] PROGMEM = "</ul></div>";

const char HTML_UI_SITE_MENU_SECTION_ITEM[] PROGMEM = "<li class=\"pure-menu-item\"><a href=\"/?{{mi.s}}{{mi.c}}{{mi.a}}{{mi.o}}{{mi.p}}\" class=\"pure-menu-link\">{{i}}{{t}}</a></li>";
const char HTML_UI_SITE_MENU_SECTION_ITEM_EXTERNAL[] PROGMEM = "<li class=\"pure-menu-item\"><a href=\"{{v}}\" class=\"pure-menu-link\">{{i}}{{t}}</a></li>";

/** Section */
const char HTML_UI_SECTION_START[] PROGMEM = "<div><h1 class=\"cs\">{{t}}</h1><section class=\"s\"><header><h2 class=\"st\">{{v}}</h2></header><div class=\"sc\">";
const char HTML_UI_SECTION_END[] PROGMEM = "</div></section></div>";

/** Forms */
const char HTML_UI_FORM_START[] PROGMEM = "<form class=\"pure-form pure-form-aligned\" method=\"post\" action=\"/?{{mi.s}}{{mi.c}}{{mi.a}}{{mi.o}}{{mi.p}}\">";
const char HTML_UI_FORM_END[] PROGMEM = "<div class=\"pure-controls\"><button type=\"submit\" class=\"button-green pure-button\">{{t}}</button></div></form>";

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
    "<div class=\"pure-control-group\"><label></label><input name=\"{{n}}\" type=\"{{it}}\" value=\"{{v}}\"{{i.c}}{{i.d}}> {{t}}{{h}}</div>";

/** Form: Paragraph */
const char HTML_UI_ITEM_PARAGRAPH[] PROGMEM = "<p>{{v}}</p>";
const char HTML_UI_FORM_ITEM_PARAGRAPH[] PROGMEM = "<div class=\"pure-control-group\"><label></label>{{v}}</div>";

/** Form: Select */    
const char HTML_UI_ITEM_SELECT_OPEN[] PROGMEM =
        "<div class=\"pure-control-group\"><label>{{t}}</label><select name=\"{{v}}\">";
const char HTML_UI_ITEM_SELECT_OPTION[] PROGMEM = "<option value=\"{{v}}\" {{i.s}}>{{t}}</option>";
const char HTML_UI_ITEM_SELECT_CLOSE[] PROGMEM = "</select>{{v}}</div>";

/** Lists */
const char HTML_UI_ITEM_LIST_START[] PROGMEM = "<fieldset><ul class=\"lst\">";
const char HTML_UI_ITEM_LIST_END[] PROGMEM = "</ul></fieldset>";
const char HTML_UI_ITEM_LIST_ITEM[] PROGMEM = "<li class=\"m\">{{v}}</li>";

/** Files explorer
 * site must be in sync with ESPAPP_Parameters_HTTP_Server.h
 */
const char HTML_UI_FILE_EXPLORER_HEADER[] PROGMEM = "<table class=\"pure-table pure-table-horizontal\"><thead><tr><th>File name</th><th>Size</th><th>Action</th></tr></thead><tbody>";
const char HTML_UI_FILE_EXPLORER_FOLDER_ITEM[] PROGMEM = "<tr><td><a class=\"folder\" href=\"/?site=110&p1={{v}}\">{{v}}</a></td><td>{{t}} files</td><td><form action=\"/?site=110\" method=\"POST\"><input type=\"hidden\" name=\"d\" value=\"{{v}}\"><button {{h}} name=\"a\" value=\"4\" class=\"button-red pure-button\" type=\"submit\">Delete</button></form></td></tr>";
const char HTML_UI_FILE_EXPLORER_FILE_ITEM[] PROGMEM = "<tr><td>{{t}}</td><td>{{f.s}} B</td><td><form action=\"/?site=110\" method=\"POST\"><input type=\"hidden\" name=\"f\" value=\"{{t}}\"><input type=\"hidden\" name=\"d\" value=\"{{v}}\"><button name=\"a\" value=\"1\" class=\"button-blue pure-button\" type=\"submit\">View</button> <button name=\"a\" value=\"2\" class=\"button-blue pure-button\" type=\"submit\">Download</button> <button name=\"a\" value=\"3\" class=\"button-red pure-button\" type=\"submit\">Delete</button></form></td></tr>";
const char HTML_UI_FILE_EXPLORER_FOOTER[] PROGMEM = "</tbody></table>";

const char HTML_UI_FILE_EXPLORER_UPLOAD_FORM[] PROGMEM =
    "<form action=\"/upload?site=110\" method=\"POST\" enctype=\"multipart/form-data\">"
    "<input type=\"file\" name=\"f\">"
    "<input type=\"hidden\" name=\"d\" value=\"{{v}}\">"
    "<button name=\"a\" value=\"0\" class=\"button-green pure-button\" type=\"submit\">Upload</button>"
    "</form>";

#endif