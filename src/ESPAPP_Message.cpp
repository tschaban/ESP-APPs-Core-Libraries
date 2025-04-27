#include "ESPAPP_Message.h"

#ifdef DEBUG
ESPAPP_Message::ESPAPP_Message(ESPAPP_API_Flash *_Flash, ESPAPP_SerialDebugger *_Debugger)
{
    this->Flash = _Flash;
    this->Debugger = _Debugger;
}
#else
ESPAPP_Message::ESPAPP_Message(ESPAPP_API_Flash *_Flash)
{
    this->Flash = _Flash;
}
#endif

bool ESPAPP_Message::addMessage(const char *message)
{
#ifdef DEBUG
    this->Debugger->printInformation(F("Adding message"), F("MSG"));
    this->Debugger->printBulletPoint(F("Content: "));
    this->Debugger->printValue(message);
#endif

    // Open the file in append mode
    File file;
    bool success = this->Flash->openFile(file, ESPAPP_OPEN_FILE_APPEND, F(ESPAPP_SYS_FILE_MESSAGES), ESPAPP_NONE, true);

    if (success)
    {
        file.println(message); // Append the message as a new line
        file.close();

#ifdef DEBUG
        this->Debugger->printBulletPoint(F("Message added successfully"));
#endif
    }
    else
    {
#ifdef DEBUG
        this->Debugger->printError(F("Failed to add message"), F("MSG"));
#endif
    }

    return success;
}

bool ESPAPP_Message::addMessage(const __FlashStringHelper *message)
{
    // Convert __FlashStringHelper to char array
    char buffer[256]; // Adjust size as needed
    strncpy_P(buffer, (PGM_P)message, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0'; // Ensure null termination

    return addMessage(buffer);
}

bool ESPAPP_Message::clearMessages()
{
#ifdef DEBUG
    this->Debugger->printInformation(F("Clearing all messages"), F("MSG"));
#endif

    // Overwrite the file with an empty file
    File file;
    bool success = this->Flash->openFile(file, ESPAPP_OPEN_FILE_WRITING, F(ESPAPP_SYS_FILE_MESSAGES), ESPAPP_NONE, true);

    if (success)
    {
        file.close();


#ifdef DEBUG
        this->Debugger->printBulletPoint(F("Messages cleared successfully"));
#endif
    }
    else
    {
#ifdef DEBUG
        this->Debugger->printError(F("Failed to clear messages"), F("MSG"));
#endif
    }

    return success;
}

bool ESPAPP_Message::getMessages(char **&messageArray, uint8_t &count)
{
#ifdef DEBUG
    this->Debugger->printInformation(F("Retrieving all messages"), F("MSG"));
#endif

    File file;
    bool success = this->Flash->openFile(file, ESPAPP_OPEN_FILE_READING, F(ESPAPP_SYS_FILE_MESSAGES), ESPAPP_NONE, false);

    if (!success)
    {
#ifdef DEBUG
        this->Debugger->printBulletPoint(F("No messages file found"));
#endif
        messageArray = nullptr;
        count = 0;
        return true; // Success, but no messages
    }

    // Read all lines into memory
    std::vector<String> messages;
    while (file.available())
    {
        messages.push_back(file.readStringUntil(ESPAPP_NEW_LINE));
    }
    file.close();

    count = messages.size();
    if (count == 0)
    {
#ifdef DEBUG
        this->Debugger->printBulletPoint(F("No messages found"));
#endif
        messageArray = nullptr;
        return true; // Success, but empty
    }

    // Allocate memory for array of char pointers
    messageArray = new char *[count];
    for (int i = 0; i < count; i++)
    {
        size_t len = messages[i].length() + 1;
        messageArray[i] = new char[len];
        strncpy(messageArray[i], messages[i].c_str(), len);
    }

#ifdef DEBUG
    this->Debugger->printBulletPoint(F("Retrieved messages count: "));
    this->Debugger->printValue(count);
#endif

    this->clearMessages(); // Clear messages after retrieval

    return true;
}

bool ESPAPP_Message::newMessageAvailable(void)
{
    File file;
    bool success = this->Flash->openFile(file, ESPAPP_OPEN_FILE_READING, F(ESPAPP_SYS_FILE_MESSAGES), ESPAPP_NONE, false);

    if (!success)
    {
        return false; // No messages file found
    }

    bool hasNewMessages = file.available() && file.readStringUntil(ESPAPP_NEW_LINE).length() > 0;
    file.close();
    return hasNewMessages;
}   