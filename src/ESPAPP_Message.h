#ifndef _ESPAPP_MESSAGE_h
#define _ESPAPP_MESSAGE_h

#include <vector>
#include <Arduino.h>
#include <ESPAPP_Parameters.h>
#include <ESPAPP_API_Flash.h>

#ifdef DEBUG
#include <ESPAPP_SerialMessages.h>
#endif



class ESPAPP_Message {
private:
    ESPAPP_API_Flash *Flash;

#ifdef DEBUG
    ESPAPP_SerialMessages *Msg;
#endif

public:
#ifdef DEBUG
    ESPAPP_Message(ESPAPP_API_Flash *_Flash, ESPAPP_SerialMessages *_Msg);
#else
    ESPAPP_Message(ESPAPP_API_Flash *_Flash);
#endif

    // Add a new message to the messages.txt file
    bool addMessage(const char *message);
    bool addMessage(const __FlashStringHelper *message);

    // Clear all messages
    bool clearMessages();

    // Retrieve all messages
    bool getMessages(char **&messageArray, uint8_t &count);

    bool newMessageAvailable(void);
};

#endif // _ESPAPP_MESSAGE_h