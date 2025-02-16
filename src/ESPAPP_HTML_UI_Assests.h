#ifndef _ESPAPP_HTML_UI_ASSETS_h
#define _ESPAPP_HTML_UI_ASSETS_h

/* Build on https://pure-css.github.io/start/*/

const char HTML_UI_EMPTY_STRING[] PROGMEM = "";

const char HTML_UI_SUBMITT_BUTTON_SAVE[] PROGMEM = "Save";

/** TAGS */
const char HTML_UI_TAG_TITLE[] PROGMEM = "{{m.t}}";
const char HTML_UI_TAG_ICON[] PROGMEM = "{{mi.i}}";

/** Icons */
const char HTML_UI_NO_ICON[] PROGMEM = "";
const char HTML_UI_ICON_RIGHTWARDS_ARROW[] PROGMEM = "&#10150; ";
const char HTML_UI_ICON_ARROW[] PROGMEM = "&#8227; ";

const char HTML_UI_SITE_SCELETON[] PROGMEM =
    "<!doctype html><html lang=\"{{s.lang}}\"><head><meta charset=\"utf-8\">{{s.refresh}}<title>{{s.title}}</title><link href=\"https://fonts.googleapis.com/css2?family=Titillium+Web&display=swap\" rel=\"stylesheet\"><style>{{s.css}}</style></head><body><div class=\"c\"><div class=\"l\"><img style=\"opacity:.4\" src=\"{{f.logo}}\"><br><small style=\"opacity:.3\">{{f.subtitle-1}}<br>{{f.subtitle-2}}</small>{{menu}}</div><div class=\"r\">{{body}}</div></div><div style=\"text-align: right;\"><a href=\"{{f.url}}\"><img style=\"opacity:.8;\" src=\"{{f.logo-url}}\" /></a></div></body></html>";

const char HTML_UI_SITE_HEADER_START[] PROGMEM =
    "<!doctype html><html lang=\"{{s.lang}}\"><head><meta charset=\"utf-8\">{{s.refresh}}<title>{{s.title}}</title><link href=\"https://fonts.googleapis.com/css2?family=Titillium+Web&display=swap\" rel=\"stylesheet\"><style>{{s.css}}</style></head><body><div class=\"c\"><div class=\"l\"><img style=\"opacity:.4\" src=\"{{f.logo}}\"><br><small style=\"opacity:.3\">{{f.subtitle-1}}<br>{{f.subtitle-2}}</small>";

const char HTML_UI_SITE_BODY_START[] PROGMEM = "</div><div class=\"r\">";

const char HTML_UI_SITE_END[] PROGMEM = "</div></div><div style=\"text-align: right;\"><a href=\"{{f.url}}\"><img style=\"opacity:.8;\" src=\"{{f.logo-url}}\" /></a></div></body></html>";

const char HTML_UI_CSS[] PROGMEM =
    ".l,.r{opacity:.93}.b,.itm a,.m,a{text-decoration:none}html{background:url(http://api.smartnydom.pl/images/background-3.8.0.webp) center center/cover no-repeat fixed;-webkit-background-size:cover;-moz-background-size:cover;-o-background-size:cover}.l,.r{margin:0;padding:1.2em 1.2em 2.2em;display:table-cell}.ltit,body{margin:auto}body{padding:2em;max-width:980px}.c{display:table;width:100%;margin-bottom:.5em}.m{padding-left:1.5em;color:#777;font-size:.9em}.l{width:280px;background:#000;color:#fff;border-top-left-radius:.8em;border-bottom-left-radius:.8em}.r{background:#eee;border-top-right-radius:.8em;border-bottom-right-radius:.8em}a,button,code,h1,h3,h4,input,label,li,p,small,span{font-family:'Titillium Web',sans-serif}h4{font-size:.8em;margin-bottom:1px}.lst{list-style:none;margin:0;padding:0}.itm a,.itm i{display:block;white-space:nowrap;padding:0 1em;color:#aaa;font-size:.8em}.ci h1,.cm{color:#999}.itm i{font-weight:700;padding-top:.5em}.cf label,.hint,input,select{display:inline-block;vertical-align:middle}.itm a:focus,.itm a:hover{background-color:#aaa;text-decoration:none;padding:0 1.5em;color:#000}.ci{margin-bottom:1em}.ci h1{border-bottom:1px solid #ddd;font-size:1.2em;font-weight:500;margin:0}.ci .cd{color:#aaa;font-size:80%;font-style:italic;margin-bottom:2em;margin-top:0}.cm{font-size:90%;margin:0 0 20px}.hint,.la{font-size:80%}.la{margin:0 .1em;padding:.3em 1em;color:#fff;background:#999}.cc label,.cf label{font-size:.875em}.be,.lr{background:#ca3c3c}.bs,.lg{background:#2fb548}fieldset{margin:0;padding:.35em 0 .75em;border:0}.cf{margin-bottom:.3em}.cc{margin:0 0 .1em 9.4em}.cf label{text-align:right;width:10em;margin:0 1em 0 0}input,select{padding:.4em .6em;border:1px solid #ccc;box-sizing:border-box}.hint{padding-left:.5em;color:#aaa}.b{font-size:100%;padding:.2em 1em;border:1px solid #999;border:transparent;color:#fff}.bw{background:#df7514}.bc{background:#42b8dd}.b:focus,.b:hover{background-image:-webkit-linear-gradient(transparent,rgba(0,0,0,.05) 40%,rgba(0,0,0,.1));background-image:linear-gradient(transparent,rgba(0,0,0,.05) 40%,rgba(0,0,0,.1))}";

/** Section */
const char HTML_UI_SECTION_START[] PROGMEM = "<div class=\"ci\"><h1>{{T}}</h1><p class=\"cd\">{{D}}</p><fieldset>";
const char HTML_UI_SECTION_END[] PROGMEM = "</fieldset></div>";

/** Menu */
const char HTML_UI_MENU_START[] PROGMEM = "<h4>{{mi.i}}{{m.t}}</h4><ul class=\"lst\">";
const char HTML_UI_MENU_END[] PROGMEM = "</ul>";
const char HTML_UI_MENU_SUBMENU_HEADER[] PROGMEM = "<li class=\"itm\"><i>{{m.t}}</i></li>";
const char HTML_UI_MENU_ITEM[] PROGMEM =
    "<li class=\"itm\"><a href=\"/?{{mi.s}}{{mi.c}}{{mi.a}}{{mi.o}}{{mi.p}}\">{{mi.i}}{{m.t}}</a></li>";
const char HTML_UI_MENU_ITEM_EXTERNAL[] PROGMEM =
    "<li class=\"itm\"><a href=\"{{s.u}}\">{{mi.i}}{{m.t}}</a></li>";

/** Forms */
const char HTML_UI_FORM_START[] PROGMEM = "<form method=\"post\" action=\"/?{{mi.s}}{{mi.c}}{{mi.a}}{{mi.o}}{{mi.p}}\">";
const char HTML_UI_FORM_END[] PROGMEM = "<input type=\"submit\" class=\"b bs\" value=\"{{b.t}}\"></form>";

/** Form items */
const char HTML_UI_ITEM_CHECKBOX[] PROGMEM =
    "<div class=\"cc\"><label><input name=\"{{i.n}}\" "
    "type=\"{{i.t}}\" "
    "value=\"{{i.v}}\"{{i.c}}{{i.d}}>{{i.l}}</"
    "label>{{i.h}}</div>";

const char HTML_UI_ITEM_SELECT_OPEN[] PROGMEM =
    "<div class=\"cf\"><label>{{i.l}}</label><select "
    "name=\"{{i.n}}\">";

const char HTML_UI_ITEM_SELECT_OPTION[] PROGMEM =
    "<option value=\"{{i.v}}\" "
    "{{i.s}}>{{i.l}}</option>";

const char HTML_UI_ITEM_SELECT_CLOSE[] PROGMEM = "</select></div>";


const char HTML_UI_ITEM_PARAGRAPH[] PROGMEM = "<br><p class=\"cm\">{{i.v}}</p>";

/** Lists */
const char HTML_UI_ITEM_LIST_START[] PROGMEM = "<fieldset><ul class=\"lst\">";
const char HTML_UI_ITEM_LIST_END[] PROGMEM = "</ul></fieldset>";
const char HTML_UI_ITEM_LIST_ITEM[] PROGMEM = "<li class=\"m\">{{i.v}}</li>";


/** Files explorer */

const char HTML_UI_FILE_EXPLORER_HEADER[] PROGMEM = "<table><tr><th>File name</th><th>Size</th><th>Acton</th></tr>";
const char HTML_UI_FILE_EXPLORER_FOLDER_ITEM[] PROGMEM = "<tr><td><a href=\"/?site=0&p1={{f.n}}\">{{f.n}}</a></td><td>{{f.s}} files</td><td>Delete</td></tr>";
const char HTML_UI_FILE_EXPLORER_FILE_ITEM[] PROGMEM = "<tr><td>{{f.n}}</td><td>{{f.s}}B</td><td>Download | Delete</td></tr>";
const char HTML_UI_FILE_EXPLORER_FOOTER[] PROGMEM = "</table>";

#endif