#include "WitcherTracker.h"

using namespace std;

/**
 * @brief CommandParser class implementation - parses and validates user input commands
 * 
 * This class handles the complex parsing of various command types including actions,
 * knowledge statements, queries, and encounters. Provides comprehensive input validation
 * and tokenization for the Witcher tracking system.
 */

/**
 * @brief Tokenizes input string into structured command components
 * @param input The raw input string to tokenize
 * @return Vector of string tokens representing parsed command elements
 * 
 * Handles complex parsing for different command patterns including questions,
 * total queries, and Geralt commands with proper whitespace and punctuation handling.
 */
vector<string> CommandParser::tokenizeInput(const string &input)
{
    vector<string> tokens;
    int inputLen = input.length();
    int i = 0;

    // Skip leading whitespace for consistent parsing
    while (i < inputLen && isspace(input[i]))
        i++;

    // Handle question queries starting with "What"
    if (i < inputLen && input.substr(i, 4) == "What" &&
        (i + 4 >= inputLen || isspace(input[i + 4])))
    {
        tokens.push_back("What");
        i += 4;
        while (i < inputLen && isspace(input[i]))
            i++;

        // Check for "is in" (alchemy query) or "is effective against" (bestiary query)
        if (i < inputLen && input.substr(i, 2) == "is" &&
            (i + 2 >= inputLen || isspace(input[i + 2])))
        {
            tokens.push_back("is");
            i += 2;
            while (i < inputLen && isspace(input[i]))
                i++;

            // Parse alchemy query pattern: "What is in <potion>?"
            if (i < inputLen && input.substr(i, 2) == "in" &&
                (i + 2 >= inputLen || isspace(input[i + 2])))
            {
                tokens.push_back("in");
                i += 2;
                while (i < inputLen && isspace(input[i]))
                    i++;

                // Extract potion name up to question mark
                int potionStart = i;
                int potionEnd = -1;

                for (int j = i; j < inputLen; j++)
                {
                    if (input[j] == '?')
                    {
                        potionEnd = j;
                        break;
                    }
                }

                if (potionEnd == -1)
                    potionEnd = inputLen;

                // Trim trailing whitespace from potion name
                while (potionEnd > potionStart && isspace(input[potionEnd - 1]))
                    potionEnd--;

                int potionLen = potionEnd - potionStart;
                if (potionLen > 0)
                {
                    tokens.push_back(input.substr(potionStart, potionLen));
                }

                // Process question mark and any remaining tokens
                while (i < inputLen && input[i] != '?')
                    i++;
                if (i < inputLen && input[i] == '?')
                {
                    tokens.push_back("?");
                    i++;

                    // Continue parsing after question mark for additional parameters
                    while (i < inputLen)
                    {
                        while (i < inputLen && isspace(input[i]))
                            i++;
                        if (i >= inputLen)
                            break;

                        if (input[i] == ',')
                        {
                            tokens.push_back(",");
                            i++;
                            continue;
                        }

                        int tokenStart = i;
                        while (i < inputLen && !isspace(input[i]) && input[i] != ',')
                            i++;
                        int tokenLen = i - tokenStart;
                        if (tokenLen > 0)
                        {
                            tokens.push_back(input.substr(tokenStart, tokenLen));
                        }
                    }
                }
                return tokens;
            }
            // Parse bestiary query pattern: "What is effective against <monster>?"
            else if (i < inputLen && input.substr(i, 9) == "effective" &&
                     (i + 9 >= inputLen || isspace(input[i + 9])))
            {
                tokens.push_back("effective");
                i += 9;
                while (i < inputLen && isspace(input[i]))
                    i++;

                if (i < inputLen && input.substr(i, 7) == "against" &&
                    (i + 7 >= inputLen || isspace(input[i + 7])))
                {
                    tokens.push_back("against");
                    i += 7;
                    while (i < inputLen && isspace(input[i]))
                        i++;

                    // Extract monster name up to question mark
                    int monsterStart = i;
                    int monsterEnd = -1;

                    for (int j = i; j < inputLen; j++)
                    {
                        if (input[j] == '?')
                        {
                            monsterEnd = j;
                            break;
                        }
                    }

                    if (monsterEnd == -1)
                        monsterEnd = inputLen;

                    // Trim trailing whitespace from monster name
                    while (monsterEnd > monsterStart && isspace(input[monsterEnd - 1]))
                        monsterEnd--;

                    int monsterLen = monsterEnd - monsterStart;
                    if (monsterLen > 0)
                    {
                        tokens.push_back(input.substr(monsterStart, monsterLen));
                    }

                    // Process question mark and remaining tokens
                    while (i < inputLen && input[i] != '?')
                        i++;
                    if (i < inputLen && input[i] == '?')
                    {
                        tokens.push_back("?");
                        i++;

                        // Continue parsing after question mark
                        while (i < inputLen)
                        {
                            while (i < inputLen && isspace(input[i]))
                                i++;
                            if (i >= inputLen)
                                break;

                            if (input[i] == ',')
                            {
                                tokens.push_back(",");
                                i++;
                                continue;
                            }

                            int tokenStart = i;
                            while (i < inputLen && !isspace(input[i]) && input[i] != ',')
                                i++;
                            int tokenLen = i - tokenStart;
                            if (tokenLen > 0)
                            {
                                tokens.push_back(input.substr(tokenStart, tokenLen));
                            }
                        }
                    }
                    return tokens;
                }
            }
        }
    }

    // Handle inventory total queries starting with "Total"
    if (i < inputLen && input.substr(i, 5) == "Total" &&
        (i + 5 >= inputLen || isspace(input[i + 5])))
    {
        tokens.push_back("Total");
        i += 5;
        while (i < inputLen && isspace(input[i]))
            i++;

        // Extract category (potion/ingredient/trophy)
        int catStart = i;
        while (i < inputLen && !isspace(input[i]) && input[i] != '?')
            i++;
        int catLen = i - catStart;
        if (catLen > 0)
        {
            tokens.push_back(input.substr(catStart, catLen));
        }

        while (i < inputLen && isspace(input[i]))
            i++;

        // Handle immediate question mark (general query)
        if (i < inputLen && input[i] == '?')
        {
            tokens.push_back("?");
            i++;

            // Continue parsing after question mark
            while (i < inputLen)
            {
                while (i < inputLen && isspace(input[i]))
                    i++;
                if (i >= inputLen)
                    break;

                if (input[i] == ',')
                {
                    tokens.push_back(",");
                    i++;
                    continue;
                }

                int tokenStart = i;
                while (i < inputLen && !isspace(input[i]) && input[i] != ',')
                    i++;
                int tokenLen = i - tokenStart;
                if (tokenLen > 0)
                {
                    tokens.push_back(input.substr(tokenStart, tokenLen));
                }
            }
            return tokens;
        }

        // Return early if no more input to process
        if (i >= inputLen)
            return tokens;

        while (i < inputLen && isspace(input[i]))
            i++;

        // Extract specific item name for targeted queries
        int itemStart = i;
        int itemEnd = -1;
        for (int j = i; j < inputLen; j++)
        {
            if (input[j] == '?')
            {
                itemEnd = j;
                break;
            }
        }
        if (itemEnd == -1)
            itemEnd = inputLen;

        // Trim trailing spaces from item name
        while (itemEnd > itemStart && isspace(input[itemEnd - 1]))
            itemEnd--;

        int itemLen = itemEnd - itemStart;
        if (itemLen > 0)
        {
            tokens.push_back(input.substr(itemStart, itemLen));
        }

        // Process question mark and remaining content
        while (i < inputLen && input[i] != '?')
            i++;
        if (i < inputLen && input[i] == '?')
        {
            tokens.push_back("?");
            i++;

            // Continue parsing after question mark
            while (i < inputLen)
            {
                while (i < inputLen && isspace(input[i]))
                    i++;
                if (i >= inputLen)
                    break;

                if (input[i] == ',')
                {
                    tokens.push_back(",");
                    i++;
                    continue;
                }

                int tokenStart = i;
                while (i < inputLen && !isspace(input[i]) && input[i] != ',')
                    i++;
                int tokenLen = i - tokenStart;
                if (tokenLen > 0)
                {
                    tokens.push_back(input.substr(tokenStart, tokenLen));
                }
            }
        }
        return tokens;
    }

    // Reset parser position for Geralt commands
    i = 0;
    while (i < inputLen && isspace(input[i]))
        i++;

    // Handle Geralt action commands
    if (i < inputLen && input.substr(i, 6) == "Geralt" &&
        (i + 6 >= inputLen || isspace(input[i + 6])))
    {
        tokens.push_back("Geralt");
        i += 6;

        // Skip whitespace after "Geralt"
        while (i < inputLen && isspace(input[i]))
            i++;

        // Parse "brews" command - simple pattern with potion name
        if (i < inputLen && input.substr(i, 5) == "brews" &&
            (i + 5 >= inputLen || isspace(input[i + 5])))
        {
            tokens.push_back("brews");
            i += 5;

            // Skip whitespace and capture entire potion name
            while (i < inputLen && isspace(input[i]))
                i++;

            // Capture potion name as single token (may contain spaces)
            if (i < inputLen)
            {
                tokens.push_back(input.substr(i));
            }
            return tokens;
        }
        // Parse "learns" command - complex pattern for knowledge statements
        else if (i < inputLen && input.substr(i, 6) == "learns" &&
                 (i + 6 >= inputLen || isspace(input[i + 6])))
        {
            tokens.push_back("learns");
            i += 6;

            // Skip whitespace
            while (i < inputLen && isspace(input[i]))
                i++;

            // Store position for backtracking during complex parsing
            int learnsStartPos = i;

            // Look for "sign" or "potion" keywords to determine parse strategy
            vector<string> tempTokens;
            while (i < inputLen)
            {
                while (i < inputLen && isspace(input[i]))
                    i++;
                if (i >= inputLen)
                    break;

                int wordStart = i;
                while (i < inputLen && !isspace(input[i]))
                    i++;
                int wordEnd = i;

                int wordLen = wordEnd - wordStart;
                if (wordLen <= 0)
                    break;

                string word = input.substr(wordStart, wordLen);
                tempTokens.push_back(word);

                // Handle effectiveness knowledge: <item> sign/potion is effective against <monster>
                if (word == "sign" || word == "potion")
                {
                    bool isPotion = (word == "potion");

                    if (tempTokens.size() >= 2)
                    {
                        // Extract item name before "sign"/"potion" keyword
                        int rawStart = learnsStartPos;
                        int rawEnd = wordStart;

                        while (rawStart < rawEnd && isspace(input[rawStart]))
                            rawStart++;
                        while (rawEnd > rawStart && isspace(input[rawEnd - 1]))
                            rawEnd--;

                        if (rawEnd > rawStart)
                        {
                            tokens.push_back(input.substr(rawStart, rawEnd - rawStart));
                            tokens.push_back(word); // "sign" or "potion"

                            while (i < inputLen && isspace(input[i]))
                                i++;

                            // Parse effectiveness pattern: "is effective against"
                            if (i < inputLen && input.substr(i, 2) == "is" &&
                                (i + 2 >= inputLen || isspace(input[i + 2])))
                            {
                                tokens.push_back("is");
                                i += 2;

                                while (i < inputLen && isspace(input[i]))
                                    i++;

                                if (i < inputLen && input.substr(i, 9) == "effective" &&
                                    (i + 9 >= inputLen || isspace(input[i + 9])))
                                {
                                    tokens.push_back("effective");
                                    i += 9;

                                    while (i < inputLen && isspace(input[i]))
                                        i++;

                                    if (i < inputLen && input.substr(i, 7) == "against" &&
                                        (i + 7 >= inputLen || isspace(input[i + 7])))
                                    {
                                        tokens.push_back("against");
                                        i += 7;

                                        while (i < inputLen && isspace(input[i]))
                                            i++;

                                        // Capture monster name (remainder of string)
                                        if (i < inputLen)
                                        {
                                            tokens.push_back(input.substr(i));
                                        }
                                        return tokens;
                                    }
                                }
                            }

                            // Parse potion formula pattern: "consists of"
                            i = wordEnd; // Reset position to after sign/potion
                            while (i < inputLen && isspace(input[i]))
                                i++;

                            if (i < inputLen && input.substr(i, 8) == "consists" &&
                                (i + 8 >= inputLen || isspace(input[i + 8])))
                            {
                                tokens.push_back("consists");
                                i += 8;

                                while (i < inputLen && isspace(input[i]))
                                    i++;

                                if (i < inputLen && input.substr(i, 2) == "of" &&
                                    (i + 2 >= inputLen || isspace(input[i + 2])))
                                {
                                    tokens.push_back("of");
                                    i += 2;

                                    // Parse ingredient list with quantities
                                    while (i < inputLen)
                                    {
                                        while (i < inputLen && isspace(input[i]))
                                            i++;
                                        if (i >= inputLen)
                                            break;

                                        // Handle quantity-ingredient pairs
                                        if (isdigit(input[i]))
                                        {
                                            // Extract numeric quantity
                                            int numStart = i;
                                            while (i < inputLen && isdigit(input[i]))
                                                i++;
                                            int numLen = i - numStart;

                                            if (numLen > 0)
                                            {
                                                tokens.push_back(input.substr(numStart, numLen));
                                            }

                                            // Skip spaces between quantity and ingredient
                                            while (i < inputLen && isspace(input[i]))
                                                i++;

                                            // Extract ingredient name
                                            int nameStart = i;
                                            while (i < inputLen && !isspace(input[i]) && input[i] != ',')
                                                i++;
                                            int nameLen = i - nameStart;

                                            if (nameLen > 0)
                                            {
                                                tokens.push_back(input.substr(nameStart, nameLen));
                                            }

                                            // Skip spaces after ingredient
                                            while (i < inputLen && isspace(input[i]))
                                                i++;

                                            // Handle comma separators
                                            if (i < inputLen && input[i] == ',')
                                            {
                                                tokens.push_back(",");
                                                i++;
                                            }
                                        }
                                        else
                                        {
                                            // Handle non-numeric tokens
                                            int wordStart = i;
                                            while (i < inputLen && !isspace(input[i]) && input[i] != ',')
                                                i++;
                                            int wordLen = i - wordStart;

                                            if (wordLen > 0)
                                            {
                                                tokens.push_back(input.substr(wordStart, wordLen));
                                            }

                                            // Skip spaces
                                            while (i < inputLen && isspace(input[i]))
                                                i++;

                                            // Handle comma if present
                                            if (i < inputLen && input[i] == ',')
                                            {
                                                tokens.push_back(",");
                                                i++;
                                            }
                                        }
                                    }
                                    return tokens;
                                }
                            }
                        }
                    }
                    return tokens; // Fallback if parsing fails
                }
            }
            return tokens; // Fallback if neither potion nor sign found
        }
        // Parse "trades" command - handles trophy-for-ingredient exchanges
        else if (i < inputLen && input.substr(i, 6) == "trades" &&
                 (i + 6 >= inputLen || isspace(input[i + 6])))
        {
            tokens.push_back("trades");
            i += 6;

            // Process remaining tokens with proper comma handling
            while (i < inputLen)
            {
                // Skip spaces
                while (i < inputLen && isspace(input[i]))
                    i++;
                if (i >= inputLen)
                    break;

                // Handle comma as separate token for structured parsing
                if (input[i] == ',')
                {
                    tokens.push_back(",");
                    i++;
                    continue;
                }

                // Handle regular token (words/numbers)
                int tokenStart = i;
                while (i < inputLen && input[i] != ',' && !isspace(input[i]))
                    i++;

                int tokenLen = i - tokenStart;
                if (tokenLen > 0)
                {
                    tokens.push_back(input.substr(tokenStart, tokenLen));
                }
            }
            return tokens;
        }
        // Note: "loots" and "encounters" commands fall through to generic parsing
    }

    // Generic fallback parsing for unrecognized patterns
    i = 0;
    tokens.clear();

    while (i < inputLen)
    {
        // Skip spaces between tokens
        while (i < inputLen && isspace(input[i]))
            i++;
        if (i >= inputLen)
            break;

        // Handle comma as separate token
        if (input[i] == ',')
        {
            tokens.push_back(",");
            i++;
            continue;
        }

        // Extract regular token until space or comma
        int tokenStart = i;
        while (i < inputLen && input[i] != ',' && !isspace(input[i]))
            i++;

                    int tokenLen = i - tokenStart;
        if (tokenLen > 0)
        {
            tokens.push_back(input.substr(tokenStart, tokenLen));
        }
    }

    return tokens;
}

/**
 * @brief Cleans input string by removing trailing newlines and trimming whitespace
 * @param input The raw input string to clean
 * @return Cleaned string with trimmed whitespace and no trailing newlines
 */
string CommandParser::cleanInputLine(const string &input)
{
    string cleaned = input;

    // Remove trailing newline if present for consistent processing
    if (!cleaned.empty() && cleaned.back() == '\n')
    {
        cleaned.pop_back();
    }

    // Trim leading spaces for uniform input format
    size_t start = 0;
    while (start < cleaned.length() && isspace(cleaned[start]))
    {
        start++;
    }

    if (start > 0)
    {
        cleaned = cleaned.substr(start);
    }

    // Trim trailing spaces for clean input
    while (!cleaned.empty() && isspace(cleaned.back()))
    {
        cleaned.pop_back();
    }

    return cleaned;
}

/**
 * @brief Validates if a token represents a positive integer
 * @param token The string token to validate
 * @return true if token is a positive integer, false otherwise
 * 
 * Rejects leading zeros (except single "0") and non-digit characters
 */
bool CommandParser::isPositiveInteger(const string &token)
{
    if (token.empty())
        return false;

    // Reject leading zeros except for single "0" - invalid format
    if (token[0] == '0' && token.length() > 1)
        return false;

    // Ensure all characters are digits
    for (char c : token)
    {
        if (!isdigit(c))
            return false;
    }

    int value = stoi(token);
    return value > 0;
}

/**
 * @brief Validates if a token contains only alphabetic characters
 * @param token The string token to validate
 * @return true if token contains only letters, false otherwise
 */
bool CommandParser::isAlphabeticOnly(const string &token)
{
    if (token.empty())
        return false;

    for (char c : token)
    {
        if (!isalpha(c))
            return false;
    }

    return true;
}

/**
 * @brief Checks for comma spacing errors in tokenized input
 * @param tokens Vector of tokens to validate
 * @return true if comma spacing errors found, false otherwise
 * 
 * Detects leading/trailing commas and consecutive commas which indicate malformed input
 */
bool CommandParser::hasCommaSpacingError(const vector<string> &tokens)
{
    for (size_t i = 0; i < tokens.size(); ++i)
    {
        if (tokens[i] == ",")
        {
            // Check if comma is at the beginning or end - invalid placement
            if (i == 0 || i == tokens.size() - 1)
            {
                return true;
            }
            // Check for consecutive commas - malformed list
            if (i > 0 && tokens[i - 1] == ",")
            {
                return true;
            }
            if (i < tokens.size() - 1 && tokens[i + 1] == ",")
            {
                return true;
            }
        }
    }
    return false;
}

/**
 * @brief Validates potion name format allowing letters and single spaces
 * @param token The token to validate as a potion name
 * @return true if valid potion name format, false otherwise
 * 
 * Allows alphabetic characters and single spaces, rejects consecutive spaces
 */
bool CommandParser::isValidPotionNameToken(const string &token)
{
    if (token.empty())
        return false;

    bool lastWasSpace = false;

    for (char c : token)
    {
        if (c == ' ')
        {
            if (lastWasSpace) // Reject consecutive spaces
                return false;
            lastWasSpace = true;
        }
        else if (!isalpha(c))
        {
            return false;
        }
        else
        {
            lastWasSpace = false;
        }
    }
    return true;
}

/**
 * @brief Validates loot action command format
 * @param input The input string to validate
 * @return true if valid loot action, false otherwise
 * 
 * Expected format: "Geralt loots quantity ingredient [, quantity ingredient]..."
 */
bool CommandParser::isLootAction(const string &input)
{
    auto tokens = tokenizeInput(input);
    
    if (hasCommaSpacingError(tokens))
    {
        return false;
    }

    if (tokens.size() < 4)
        return false;

    if (tokens[0] != "Geralt" || tokens[1] != "loots")
        return false;

    // Validate pattern: quantity, ingredient [, quantity, ingredient]...
    size_t tokenIndex = 2;
    while (tokenIndex < tokens.size())
    {
        // Expect positive integer quantity
        if (!isPositiveInteger(tokens[tokenIndex]))
            return false;
        tokenIndex++;

        // Expect alphabetic ingredient name
        if (tokenIndex >= tokens.size() || !isAlphabeticOnly(tokens[tokenIndex]))
            return false;
        tokenIndex++;

        // Check for comma separator or end of input
        if (tokenIndex < tokens.size())
        {
            if (tokens[tokenIndex] == ",")
            {
                tokenIndex++;
                if (tokenIndex >= tokens.size())
                    return false; // Trailing comma is invalid
            }
        }
    }

    return true;
}

/**
 * @brief Validates trade action command format
 * @param input The input string to validate
 * @return true if valid trade action, false otherwise
 * 
 * Expected format: "Geralt trades quantity monster [, quantity monster] trophy for quantity ingredient [, quantity ingredient]..."
 */
bool CommandParser::isTradeAction(const string &input)
{
    auto tokens = tokenizeInput(input);

    if (hasCommaSpacingError(tokens))
        return false;

    // Check command start
    if (tokens.size() < 2 || tokens[0] != "Geralt" || tokens[1] != "trades")
        return false;

    // Find "for" keyword to separate trophy and ingredient lists
    int forIndex = -1;
    for (size_t i = 2; i < tokens.size(); i++)
    {
        if (tokens[i] == "for")
        {
            forIndex = static_cast<int>(i);
            break;
        }
    }

    if (forIndex == -1 || forIndex <= 4 || forIndex >= static_cast<int>(tokens.size()) - 2)
        return false;

    // Validate trophy list: from index 2 to forIndex - 1
    size_t i = 2;
    bool expectingQuantity = true;
    bool lastTrophyHasKeyword = false;

    while (i < static_cast<size_t>(forIndex))
    {
        // Parse quantity-monster-trophy pattern
        if (expectingQuantity)
        {
            if (!isPositiveInteger(tokens[i]))
                return false;
            i++;
            expectingQuantity = false;
        }
        // Monster name validation
        else if (!isAlphabeticOnly(tokens[i]))
        {
            return false;
        }
        else
        {
            i++;
            // Check for "trophy" keyword at end of trophy list
            if (i < static_cast<size_t>(forIndex) && tokens[i] == "trophy" && i + 1 == static_cast<size_t>(forIndex))
            {
                lastTrophyHasKeyword = true;
                i++;
            }
            else if (i < static_cast<size_t>(forIndex) && tokens[i] == ",")
            {
                i++;
                expectingQuantity = true;
            }
            else
            {
                // Must be either 'trophy' at end or comma between entries
                return false;
            }
        }
    }

    if (!lastTrophyHasKeyword)
        return false;

    // Validate ingredient list after "for"
    i = forIndex + 1;
    expectingQuantity = true;

    while (i < tokens.size())
    {
        // Validate quantity
        if (expectingQuantity)
        {
            if (!isPositiveInteger(tokens[i]))
                return false;
            i++;
            expectingQuantity = false;

            // Invalid if quantity is last token
            if (i == tokens.size())
                return false;
        }
        // Validate ingredient name
        else if (!isAlphabeticOnly(tokens[i]))
        {
            return false;
        }
        else
        {
            i++;
            // Complete quantity-ingredient pair processed

            // End of input is valid
            if (i == tokens.size())
            {
                break;
            }

            if (tokens[i] == ",")
            {
                i++;
                expectingQuantity = true;

                // Trailing comma is invalid
                if (i == tokens.size())
                {
                    return false;
                }
            }
            else
            {
                return false;
            }
        }
    }

    // Ensure we're not expecting another quantity-ingredient pair
    if (expectingQuantity)
    {
        return false;
    }

    return true;
}

/**
 * @brief Validates brew action command format
 * @param input The input string to validate
 * @return true if valid brew action, false otherwise
 * 
 * Expected format: "Geralt brews <potion_name>"
 */
bool CommandParser::isBrewAction(const string &input)
{
    auto tokens = tokenizeInput(input);

    // Minimum required tokens
    if (tokens.size() < 3)
        return false;

    // Validate command structure
    if (tokens[0] != "Geralt" || tokens[1] != "brews")
        return false;

    // Validate potion name format (alphabetic with single spaces allowed)
    const string &potionName = tokens[2];
    bool lastWasSpace = false;

    for (char c : potionName)
    {
        if (c == ' ')
        {
            if (lastWasSpace) // Reject consecutive spaces
                return false;
            lastWasSpace = true;
        }
        else if (!isalpha(c))
        {
            return false;
        }
        else
        {
            lastWasSpace = false;
        }
    }

    return true;
}

/**
 * @brief Validates effectiveness knowledge statement format
 * @param input The input string to validate
 * @return true if valid effectiveness knowledge, false otherwise
 * 
 * Expected format: "Geralt learns <item> potion/sign is effective against <monster>"
 */
bool CommandParser::isEffectivenessKnowledge(const string &input)
{
    auto tokens = tokenizeInput(input);

    // Must start with "Geralt learns"
    if (tokens.size() < 5 || tokens[0] != "Geralt" || tokens[1] != "learns")
        return false;

    // Validate pattern structure
    const string &type = tokens[3]; // "potion" or "sign"
    const string &itemName = tokens[2];

    if (!(type == "potion" || type == "sign"))
        return false;

    if (tokens[4] != "is" ||
        tokens[5] != "effective" ||
        tokens[6] != "against")
        return false;

    // Monster name validation
    if (tokens.size() != 8 || !isAlphabeticOnly(tokens[7]))
        return false;

    // Validate item name based on type
    if (type == "sign")
    {
        // Sign must be single word, alphabetic only
        return isAlphabeticOnly(itemName);
    }
    else
    {
        // Potion name may include spaces
        return isValidPotionNameToken(itemName);
    }
}

/**
 * @brief Validates potion formula knowledge statement format
 * @param input The input string to validate
 * @return true if valid potion formula knowledge, false otherwise
 * 
 * Expected format: "Geralt learns <potion> potion consists of quantity ingredient [, quantity ingredient]..."
 */
bool CommandParser::isPotionFormulaKnowledge(const string &input)
{
    auto tokens = tokenizeInput(input);

    if (tokens.size() < 7)
        return false;

    // Check for comma formatting issues
    if (hasCommaSpacingError(tokens))
        return false;

    // Reject trailing comma
    if (!tokens.empty() && tokens[tokens.size() - 1] == ",")
    {
        return false;
    }

    // Validate command start
    if (tokens[0] != "Geralt" || tokens[1] != "learns")
        return false;

    // Find required keywords and validate their order
    int potionIndex = -1, consistsIndex = -1, ofIndex = -1;
    for (size_t i = 2; i < tokens.size(); i++)
    {
        if (potionIndex == -1 && tokens[i] == "potion")
            potionIndex = static_cast<int>(i);
        else if (consistsIndex == -1 && tokens[i] == "consists")
            consistsIndex = static_cast<int>(i);
        else if (ofIndex == -1 && tokens[i] == "of")
            ofIndex = static_cast<int>(i);
    }

    // Ensure all keywords found in correct order
    if (potionIndex == -1 || consistsIndex == -1 || ofIndex == -1)
        return false;

    if (!(1 < potionIndex && potionIndex < consistsIndex && consistsIndex < ofIndex))
        return false;

    // Validate potion name between "learns" and "potion"
    for (int i = 2; i < potionIndex; i++)
    {
        if (!isValidPotionNameToken(tokens[i]))
            return false;
    }

    // Ensure proper keyword sequence: "potion consists of"
    if (!(potionIndex + 1 == consistsIndex && consistsIndex + 1 == ofIndex))
        return false;

    // Validate ingredient list after "of"
    size_t i = ofIndex + 1;

    while (i < tokens.size())
    {
        // Validate quantity
        if (i >= tokens.size() || !isPositiveInteger(tokens[i]))
            return false;
        i++;

        // Validate ingredient name
        if (i >= tokens.size() || !isAlphabeticOnly(tokens[i]))
            return false;
        i++;

        // Handle optional comma separator
        if (i < tokens.size())
        {
            if (tokens[i] == ",")
            {
                i++;
                // Trailing comma is invalid
                if (i == tokens.size())
                    return false;
            }
            else
            {
                // No comma means this should be the last item
                if ((tokens.size() - i) >= 1)
                    return false;
            }
        }
    }

    return true;
}

/**
 * @brief Validates encounter sentence format
 * @param input The input string to validate
 * @return true if valid encounter sentence, false otherwise
 * 
 * Expected format: "Geralt encounters a <monster>"
 */
bool CommandParser::isEncounterSentence(const string &input)
{
    auto tokens = tokenizeInput(input);

    // Exact pattern required
    if (tokens.size() != 4)
        return false;

    if (tokens[0] != "Geralt" || tokens[1] != "encounters" || tokens[2] != "a")
        return false;

    // Monster name must be alphabetic
    if (!isAlphabeticOnly(tokens[3]))
        return false;

    return true;
}

/**
 * @brief Validates inventory query format
 * @param input The input string to validate
 * @param isSpecific Reference to boolean indicating if query is for specific item
 * @return true if valid inventory query, false otherwise
 * 
 * Expected formats: "Total <category> ?" or "Total <category> <item> ?"
 */
bool CommandParser::isInventoryQuery(const string &input, bool &isSpecific)
{
    auto tokens = tokenizeInput(input);

    if (tokens.size() < 3 || tokens.size() > 4)
        return false;

    // Must end with question mark
    if (tokens[tokens.size() - 1] != "?")
        return false;

    // Must start with "Total"
    if (tokens[0] != "Total")
        return false;

    // Validate category
    if (tokens[1] != "ingredient" &&
        tokens[1] != "potion" &&
        tokens[1] != "trophy")
        return false;

    // Determine if specific item query
    isSpecific = (tokens.size() == 4); // Total + category + name + '?'

    if (isSpecific)
    {
        // Validate item name based on category
        if (tokens[1] == "ingredient" || tokens[1] == "trophy")
        {
            if (!isAlphabeticOnly(tokens[2]))
            {
                return false;
            }
        }
        else if (tokens[1] == "potion")
        {
            if (!isValidPotionNameToken(tokens[2]))
            {
                return false;
            }
        }
    }

    return true;
}

/**
 * @brief Validates bestiary query format
 * @param input The input string to validate
 * @return true if valid bestiary query, false otherwise
 * 
 * Expected format: "What is effective against <monster> ?"
 */
bool CommandParser::isBestiaryQuery(const string &input)
{
    auto tokens = tokenizeInput(input);

    // Exact pattern required
    if (tokens.size() != 6)
        return false;

    if (tokens[0] != "What" || tokens[1] != "is" || tokens[2] != "effective" || tokens[3] != "against")
        return false;

    if (!isAlphabeticOnly(tokens[4]))
        return false;

    if (tokens[5] != "?")
        return false;

    return true;
}

/**
 * @brief Validates alchemy query format
 * @param input The input string to validate
 * @return true if valid alchemy query, false otherwise
 * 
 * Expected format: "What is in <potion> ?"
 */
bool CommandParser::isAlchemyQuery(const string &input)
{
    auto tokens = tokenizeInput(input);

    // Minimum required tokens
    if (tokens.size() < 5)
        return false;

        if (tokens[0] != "What" || tokens[1] != "is" || tokens[2] != "in")
        return false;

    if (tokens[tokens.size() - 1] != "?")
        return false;

    // Validate all tokens between "in" and "?" are valid potion name components
    for (size_t i = 3; i < tokens.size() - 1; i++)
    {
        if (!isValidPotionNameToken(tokens[i]))
            return false;
    }

    return true;
}

/**
 * @brief Checks if input is the exit command
 * @param input The input string to check
 * @return true if input is "Exit", false otherwise
 */
bool CommandParser::isExitCommand(const string &input)
{
    return input == "Exit";
}

/**
 * @brief Validates input command and determines its type
 * @param input The input string to validate
 * @param cmdType Reference to CommandType enum to store the determined command type
 * @return true if input is a valid command, false otherwise
 * @side_effects Sets cmdType to the appropriate CommandType enum value
 * 
 * Processes input through all validation methods to determine command type.
 * Order of validation is important as some patterns may overlap.
 */
bool CommandParser::isValidCommand(const string &input, CommandType &cmdType)
{
    // Check action commands first
    if (isLootAction(input))
    {
        cmdType = CommandType::ACTION_LOOT;
        return true;
    }
    else if (isTradeAction(input))
    {
        cmdType = CommandType::ACTION_TRADE;
        return true;
    }
    else if (isBrewAction(input))
    {
        cmdType = CommandType::ACTION_BREW;
        return true;
    }
    // Check knowledge commands
    else if (isEffectivenessKnowledge(input))
    {
        cmdType = CommandType::KNOWLEDGE_EFFECTIVENESS;
        return true;
    }
    else if (isPotionFormulaKnowledge(input))
    {
        cmdType = CommandType::KNOWLEDGE_POTION_FORMULA;
        return true;
    }
    // Check encounter command
    else if (isEncounterSentence(input))
    {
        cmdType = CommandType::ENCOUNTER;
        return true;
    }
    // Check query commands
    else
    {
        bool isSpecific = false;
        if (isInventoryQuery(input, isSpecific))
        {
            // Set appropriate inventory query type based on specificity
            cmdType = isSpecific ? CommandType::QUERY_SPECIFIC_INVENTORY : CommandType::QUERY_ALL_INVENTORY;
            return true;
        }
        else if (isBestiaryQuery(input))
        {
            cmdType = CommandType::QUERY_BESTIARY;
            return true;
        }
        else if (isAlchemyQuery(input))
        {
            cmdType = CommandType::QUERY_ALCHEMY;
            return true;
        }
        else if (isExitCommand(input))
        {
            cmdType = CommandType::EXIT_COMMAND;
            return true;
        }
    }

    // If no valid command pattern matched, mark as invalid
    cmdType = CommandType::INVALID_COMMAND;
    return false;
}