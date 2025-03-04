/*
 * NotificationManager Teamspeak3 plugin
 * Copyright (c) EricZones
 */

#if defined(WIN32) || defined(__WIN32__) || defined(_WIN32)
#pragma warning(disable : 4100) /* Disable Unreferenced parameter warning */
#include <Windows.h>
#endif

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "teamspeak/public_definitions.h"
#include "teamspeak/public_errors.h"
#include "teamspeak/public_errors_rare.h"
#include "teamspeak/public_rare_definitions.h"
#include "ts3_functions.h"

#include "plugin.h"

static struct TS3Functions ts3Functions;

#ifdef _WIN32
#define _strcpy(dest, destSize, src) strcpy_s(dest, destSize, src)
#define snprintf sprintf_s
#else
#define _strcpy(dest, destSize, src)                                                                                                                                                                                                                           \
    {                                                                                                                                                                                                                                                          \
        strncpy(dest, src, destSize - 1);                                                                                                                                                                                                                      \
        (dest)[destSize - 1] = '\0';                                                                                                                                                                                                                           \
    }
#endif

#define PLUGIN_API_VERSION 26

#define PATH_BUFSIZE 512
#define COMMAND_BUFSIZE 128
#define INFODATA_BUFSIZE 128
#define SERVERINFO_BUFSIZE 256
#define CHANNELINFO_BUFSIZE 512
#define RETURNCODE_BUFSIZE 128

static char* pluginID = NULL;
static boolean privateChat = true, channelChat = true, serverChat = true, poke = true;

/*********************************** Required functions ************************************/

/* Plugin name */
const char* ts3plugin_name() {
    return "Notification Manager";
}

/* Plugin version */
const char* ts3plugin_version() {
    return "1.0";
}

/* Plugin API version */
int ts3plugin_apiVersion() {
    return PLUGIN_API_VERSION;
}

/* Plugin author */
const char* ts3plugin_author() {
    return "EricZones";
}

/* Plugin description */
const char* ts3plugin_description() {
    return "Keep control over incoming text messages and pokes on all servers\nToggle server, channel, client messages and pokes on the fly";
}

/* Set callback functions */
void ts3plugin_setFunctionPointers(const struct TS3Functions funcs) {
    ts3Functions = funcs;
}

/* Plugin loading function */
int ts3plugin_init() {
    char appPath[PATH_BUFSIZE];
    char resourcesPath[PATH_BUFSIZE];
    char configPath[PATH_BUFSIZE];
    char pluginPath[PATH_BUFSIZE];

    printf("[%s] Loading plugin...\n", ts3plugin_name());

    ts3Functions.getAppPath(appPath, PATH_BUFSIZE);
    ts3Functions.getResourcesPath(resourcesPath, PATH_BUFSIZE);
    ts3Functions.getConfigPath(configPath, PATH_BUFSIZE);
    ts3Functions.getPluginPath(pluginPath, PATH_BUFSIZE, pluginID);

    return 0;
}

/* Plugin unloading function */
void ts3plugin_shutdown() {
    printf("[%s] Unloading plugin...\n", ts3plugin_name());

    if (pluginID) {
        free(pluginID);
        pluginID = NULL;
    }
}

/*********************************** Optional functions ************************************/

/* Registering plugin id */
void ts3plugin_registerPluginID(const char* id) {
    const size_t sz = strlen(id) + 1;
    pluginID        = (char*)malloc(sz * sizeof(char));
    _strcpy(pluginID, sz, id);
}

/* Required to release memory allocated in ts3plugin_initMenus */
void ts3plugin_freeMemory(void* data) {
    free(data);
}

/* Static title in info frame */
const char* ts3plugin_infoTitle() {
    return "Notification Manager";
}

/* Dynamic content in info frame */
void ts3plugin_infoData(uint64 serverConnectionHandlerID, uint64 id, enum PluginItemType type, char** data) {
    char value[100];
    switch (type) {
        case PLUGIN_SERVER:
            if (serverChat)
                strcpy(value, "[b]Server Chats:[/b] [color=green]Enabled[/color]");
            else
                strcpy(value, "[b]Server Chats:[/b] [color=red]Disabled[/color]");
            break;
        case PLUGIN_CHANNEL:
            if (channelChat)
                strcpy(value, "[b]Channel Chats:[/b] [color=green]Enabled[/color]");
            else
                strcpy(value, "[b]Channel Chats:[/b] [color=red]Disabled[/color]");
            break;
        case PLUGIN_CLIENT:
            if (privateChat)
                strcpy(value, "[b]Private Chats:[/b] [color=green]Enabled[/color]");
            else
                strcpy(value, "[b]Private Chats:[/b] [color=red]Disabled[/color]");
            if (poke)
                strcat(value, "\n\n[b]Pokes:[/b] [color=green]Enabled[/color]");
            else
                strcat(value, "\n\n[b]Pokes:[/b] [color=red]Disabled[/color]");
            break;
        default:
            data = NULL;
            return;
    }
    *data = (char*)malloc(INFODATA_BUFSIZE * sizeof(char));
    snprintf(*data, INFODATA_BUFSIZE, "\n%s", value);
}

/* Menu item creation */
static struct PluginMenuItem* createMenuItem(enum PluginMenuType type, int id, const char* text, const char* icon) {
    struct PluginMenuItem* menuItem = (struct PluginMenuItem*)malloc(sizeof(struct PluginMenuItem));
    menuItem->type                  = type;
    menuItem->id                    = id;
    _strcpy(menuItem->text, PLUGIN_MENU_BUFSZ, text);
    _strcpy(menuItem->icon, PLUGIN_MENU_BUFSZ, icon);
    return menuItem;
}

/* Makros for creating menu items */
#define BEGIN_CREATE_MENUS(x)                                                                                                                                                                                                                                  \
    const size_t sz = x + 1;                                                                                                                                                                                                                                   \
    size_t       n  = 0;                                                                                                                                                                                                                                       \
    *menuItems      = (struct PluginMenuItem**)malloc(sizeof(struct PluginMenuItem*) * sz);
#define CREATE_MENU_ITEM(a, b, c, d) (*menuItems)[n++] = createMenuItem(a, b, c, d);
#define END_CREATE_MENUS                                                                                                                                                                                                                                       \
    (*menuItems)[n++] = NULL;                                                                                                                                                                                                                                  \
    assert(n == sz);

/* Menu IDs for menu items */
enum {
    MENU_ID_GLOBAL_1,
    MENU_ID_GLOBAL_2,
    MENU_ID_GLOBAL_3,
    MENU_ID_GLOBAL_4
};

/* Initialize plugin menus */
void ts3plugin_initMenus(struct PluginMenuItem*** menuItems, char** menuIcon) {
    BEGIN_CREATE_MENUS(4);
    CREATE_MENU_ITEM(PLUGIN_MENU_TYPE_GLOBAL, MENU_ID_GLOBAL_1, "Toggle Private Chats", "private.png");
    CREATE_MENU_ITEM(PLUGIN_MENU_TYPE_GLOBAL, MENU_ID_GLOBAL_2, "Toggle Channel Chats", "channel.png");
    CREATE_MENU_ITEM(PLUGIN_MENU_TYPE_GLOBAL, MENU_ID_GLOBAL_3, "Toggle Server Chats", "server.png");
    CREATE_MENU_ITEM(PLUGIN_MENU_TYPE_GLOBAL, MENU_ID_GLOBAL_4, "Toggle Pokes", "poke.png");
    END_CREATE_MENUS;

    /* Plugin menu icon */
    *menuIcon = (char*)malloc(PLUGIN_MENU_BUFSZ * sizeof(char));
    _strcpy(*menuIcon, PLUGIN_MENU_BUFSZ, "plugin.png");
}

/*********************************** TeamSpeak callbacks ************************************/

/* Client UI callback */
void ts3plugin_onMenuItemEvent(uint64 serverConnectionHandlerID, enum PluginMenuType type, int menuItemID, uint64 selectedItemID) {
    switch (type) {
        case PLUGIN_MENU_TYPE_GLOBAL:
            switch (menuItemID) {
                case MENU_ID_GLOBAL_1:
                    if (privateChat) {
                        privateChat = false;
                        ts3Functions.printMessageToCurrentTab("[color=black]<[b]Notification Manager[/b]> Disabled [color=red]Private Chats[/color]");
                    } else {
                        privateChat = true;
                        ts3Functions.printMessageToCurrentTab("[color=black]<[b]Notification Manager[/b]> Enabled [color=green]Private Chats[/color]");
                    }
                    break;
                case MENU_ID_GLOBAL_2:
                    if (channelChat) {
                        channelChat = false;
                        ts3Functions.printMessageToCurrentTab("[color=black]<[b]Notification Manager[/b]> Disabled [color=red]Channel Chats[/color]");
                    } else {
                        channelChat = true;
                        ts3Functions.printMessageToCurrentTab("[color=black]<[b]Notification Manager[/b]> Enabled [color=green]Channel Chats[/color]");
                    }
                    break;
                case MENU_ID_GLOBAL_3:
                    if (serverChat) {
                        serverChat = false;
                        ts3Functions.printMessageToCurrentTab("[color=black]<[b]Notification Manager[/b]> Disabled [color=red]Server Chats[/color]");
                    } else {
                        serverChat = true;
                        ts3Functions.printMessageToCurrentTab("[color=black]<[b]Notification Manager[/b]> Enabled [color=green]Server Chats[/color]");
                    }
                    break;
                case MENU_ID_GLOBAL_4:
                    if (poke) {
                        poke = false;
                        ts3Functions.printMessageToCurrentTab("[color=black]<[b]Notification Manager[/b]> Disabled [color=red]Pokes[/color]");
                    } else {
                        poke = true;
                        ts3Functions.printMessageToCurrentTab("[color=black]<[b]Notification Manager[/b]> Enabled [color=green]Pokes[/color]");
                    }
                    break;
                default:
                    break;
            }
        default:
            break;
    }
}

static char* pokeMessage = "This user [color=red]disabled[/color] pokes";
static char* chatMessage = "This user [color=red]disabled[/color] private chats";

int ts3plugin_onTextMessageEvent(uint64 serverConnectionHandlerID, anyID targetMode, anyID toID, anyID fromID, const char* fromName, const char* fromUniqueIdentifier, const char* message, int ffIgnored) {
    if (ffIgnored) return 0;
    anyID ownID;
    ts3Functions.getClientID(serverConnectionHandlerID, &ownID);
    if (!poke && fromID == ownID && strcmp(message, pokeMessage) == 0) return 1;
    if (!privateChat && fromID == ownID && strcmp(message, chatMessage) == 0) return 1;

    switch (targetMode) {
        case 1:
            if (!privateChat && fromID != ownID) {
                ts3Functions.requestSendPrivateTextMsg(serverConnectionHandlerID, chatMessage, fromID, NULL);
                return 1;
            }
            break;
        case 2:
            if (!channelChat && fromID != ownID) return 1;
            break;
        case 3:
            if (!serverChat && fromID != ownID) return 1;
            break;
    }
    return 0;
}

int ts3plugin_onClientPokeEvent(uint64 serverConnectionHandlerID, anyID fromClientID, const char* pokerName, const char* pokerUniqueIdentity, const char* message, int ffIgnored) {
    if (ffIgnored) return 0;
    if (poke) return 0;
    ts3Functions.requestSendPrivateTextMsg(serverConnectionHandlerID, pokeMessage, fromClientID, NULL);
    return 1;
}