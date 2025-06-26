#include "WitcherTracker.h"

using namespace std;

/**
 * @brief WitcherTracker class implementation - main system coordinator
 * 
 * This class serves as the central controller for the Witcher tracking system,
 * coordinating between command parsing, inventory management, alchemy knowledge,
 * and bestiary data. Handles all command execution and system interactions.
 */

/**
 * @brief Processes a single line of user input
 * @param line The raw input line from the user
 * @return 0 on successful execution, -1 on invalid input
 * 
 * Cleans input, validates command format, and delegates to appropriate execution method
 */
int WitcherTracker::executeLine(const string &line)
{
    // Clean input to remove extra whitespace and newlines
    string inputCopy = CommandParser::cleanInputLine(line);

    if (inputCopy.empty())
    {
        return -1;
    }

    CommandType cmdType;
    // Validate command format and determine type
    if (CommandParser::isValidCommand(inputCopy, cmdType))
    {
        return executeCommand(inputCopy, cmdType);
    }

    return -1;
}

/**
 * @brief Dispatches validated commands to appropriate execution methods
 * @param input The cleaned and validated input string
 * @param cmdType The determined command type from validation
 * @return 0 on successful execution, -1 on error
 * 
 * Central dispatcher that routes commands to specialized execution methods
 */
int WitcherTracker::executeCommand(const string &input, CommandType cmdType)
{
    switch (cmdType)
    {
    case CommandType::ACTION_LOOT:
        return executeLootAction(input);
    case CommandType::ACTION_TRADE:
        return executeTradeAction(input);
    case CommandType::ACTION_BREW:
        return executeBrewAction(input);
    case CommandType::KNOWLEDGE_EFFECTIVENESS:
        return executeEffectivenessKnowledge(input);
    case CommandType::KNOWLEDGE_POTION_FORMULA:
        return executeFormulaKnowledge(input);
    case CommandType::ENCOUNTER:
        return executeEncounter(input);
    case CommandType::QUERY_SPECIFIC_INVENTORY:
        return executeSpecificInventoryQuery(input);
    case CommandType::QUERY_ALL_INVENTORY:
        return executeAllInventoryQuery(input);
    case CommandType::QUERY_BESTIARY:
        return executeBestiaryQuery(input);
    case CommandType::QUERY_ALCHEMY:
        return executeAlchemyQuery(input);
    case CommandType::EXIT_COMMAND:
        return 0;
    default:
        return -1;
    }
}

/**
 * @brief Executes loot action commands
 * @param input The validated loot command string
 * @return 0 on successful execution
 * 
 * Parses ingredient quantities and names, adds them to inventory
 * Format: "Geralt loots quantity ingredient [, quantity ingredient]..."
 */
int WitcherTracker::executeLootAction(const string &input)
{
    auto tokens = CommandParser::tokenizeInput(input);

    // Parse ingredient-quantity pairs starting after "Geralt loots"
    size_t tokenIndex = 2;

    while (tokenIndex < tokens.size())
    {
        // Extract quantity and ingredient name
        int quantity = stoi(tokens[tokenIndex]);
        tokenIndex++;

        string ingredientName = tokens[tokenIndex];
        tokenIndex++;

        // Add to inventory
        inventory.addIngredient(ingredientName, quantity);

        // Skip comma separator if present
        if (tokenIndex < tokens.size() && tokens[tokenIndex] == ",")
        {
            tokenIndex++;
        }
    }

    cout << "Alchemy ingredients obtained\n";
    return 0;
}

/**
 * @brief Executes trade action commands
 * @param input The validated trade command string
 * @return 0 on successful execution
 * 
 * Parses trophy requirements and ingredient rewards, validates sufficient trophies,
 * and performs the exchange if possible
 * Format: "Geralt trades quantity monster [, quantity monster] trophy for quantity ingredient [, quantity ingredient]..."
 */
int WitcherTracker::executeTradeAction(const string &input)
{
    auto tokens = CommandParser::tokenizeInput(input);

    // Find "for" keyword to separate trophy and ingredient lists
    size_t forIndex = 0;
    for (size_t i = 2; i < tokens.size(); ++i)
    {
        if (tokens[i] == "for")
        {
            forIndex = i;
            break;
        }
    }

    // Parse required trophies (before "for")
    vector<pair<string, int>> requiredTrophies;
    size_t tokenIndex = 2;

    while (tokenIndex < forIndex)
    {
        int quantity = stoi(tokens[tokenIndex]);
        tokenIndex++;

        string trophyName = tokens[tokenIndex];
        tokenIndex++;

        requiredTrophies.emplace_back(trophyName, quantity);

        // Skip "trophy" keyword and comma if present
        if (tokenIndex < forIndex && tokens[tokenIndex] == "trophy")
        {
            tokenIndex++;
        }
        if (tokenIndex < forIndex && tokens[tokenIndex] == ",")
        {
            tokenIndex++;
        }
    }

    // Parse gained ingredients (after "for")
    vector<pair<string, int>> gainedIngredients;
    tokenIndex = forIndex + 1;

    while (tokenIndex < tokens.size())
    {
        int quantity = stoi(tokens[tokenIndex]);
        tokenIndex++;

        string ingredientName = tokens[tokenIndex];
        tokenIndex++;

        gainedIngredients.emplace_back(ingredientName, quantity);

        // Skip comma if present
        if (tokenIndex < tokens.size() && tokens[tokenIndex] == ",")
        {
            tokenIndex++;
        }
    }

    // Validate sufficient trophy quantities before trade
    bool hasEnoughTrophies = true;
    for (const auto &trophy : requiredTrophies)
    {
        if (inventory.getTrophyQuantity(trophy.first) < trophy.second)
        {
            hasEnoughTrophies = false;
            break;
        }
    }

    if (!hasEnoughTrophies)
    {
        cout << "Not enough trophies\n";
        return 0;
    }

    // Execute the trade: remove trophies and add ingredients
    for (const auto &trophy : requiredTrophies)
    {
        inventory.removeTrophy(trophy.first, trophy.second);
    }

    for (const auto &ingredient : gainedIngredients)
    {
        inventory.addIngredient(ingredient.first, ingredient.second);
    }

    cout << "Trade successful\n";
    return 0;
}

/**
 * @brief Executes brew action commands
 * @param input The validated brew command string
 * @return 0 on successful execution
 * 
 * Checks for known formula and sufficient ingredients, consumes ingredients
 * and creates potion if conditions are met
 * Format: "Geralt brews <potion_name>"
 */
int WitcherTracker::executeBrewAction(const string &input)
{
    auto tokens = CommandParser::tokenizeInput(input);

    // Extract potion name (everything after "Geralt brews")
    string potionName;
    for (size_t i = 2; i < tokens.size(); ++i)
    {
        if (i > 2)
            potionName += " ";
        potionName += tokens[i];
    }

    // Check if formula is known
    Potion *potion = alchemy.getPotion(potionName);
    if (!potion || !potion->hasFormula())
    {
        cout << "No formula for " << potionName << "\n";
        return 0;
    }

    // Validate sufficient ingredients for brewing
    bool hasEnoughIngredients = true;
    for (size_t i = 0; i < potion->ingredientNames.size(); ++i)
    {
        if (inventory.getIngredientQuantity(potion->ingredientNames[i]) < potion->ingredientQuantities[i])
        {
            hasEnoughIngredients = false;
            break;
        }
    }

    if (!hasEnoughIngredients)
    {
        cout << "Not enough ingredients\n";
        return 0;
    }

    // Consume ingredients and create potion
    for (size_t i = 0; i < potion->ingredientNames.size(); ++i)
    {
        inventory.removeIngredient(potion->ingredientNames[i], potion->ingredientQuantities[i]);
    }

    inventory.addPotion(potionName, 1);

    cout << "Alchemy item created: " << potionName << "\n";
    return 0;
}

/**
 * @brief Executes effectiveness knowledge commands
 * @param input The validated effectiveness knowledge string
 * @return 0 on successful execution
 * 
 * Parses item effectiveness information, updates bestiary, and adds items to alchemy knowledge
 * Format: "Geralt learns <item> potion/sign is effective against <monster>"
 */
int WitcherTracker::executeEffectivenessKnowledge(const string &input)
{
    auto tokens = CommandParser::tokenizeInput(input);

    // Find key indices for parsing
    size_t isIndex = 0, againstIndex = 0;
    bool isSign = false;

    for (size_t i = 2; i < tokens.size(); ++i)
    {
        if (tokens[i] == "is")
        {
            isIndex = i;
        }
        else if (tokens[i] == "against")
        {
            againstIndex = i;
            break;
        }
        else if (tokens[i] == "sign")
        {
            isSign = true;
        }
    }

    // Extract counter name (before "sign/potion")
    string counterName;
    for (size_t i = 2; i < isIndex; ++i)
    {
        if (tokens[i] != "sign" && tokens[i] != "potion")
        {
            if (!counterName.empty())
                counterName += " ";
            counterName += tokens[i];
        }
    }

    // Extract monster name (after "against")
    string monsterName;
    for (size_t i = againstIndex + 1; i < tokens.size(); ++i)
    {
        if (i > againstIndex + 1)
            monsterName += " ";
        monsterName += tokens[i];
    }

    // Check for existing beast and knowledge
    Beast *existingBeast = bestiary.getBeast(monsterName);
    bool beastExists = (existingBeast != nullptr);

    // Check if effectiveness is already known
    bool alreadyKnown = false;
    if (beastExists)
    {
        if (isSign)
        {
            const auto &signs = existingBeast->effectiveSigns;
            alreadyKnown = find(signs.begin(), signs.end(), counterName) != signs.end();
        }
        else
        {
            const auto &potions = existingBeast->effectivePotions;
            alreadyKnown = find(potions.begin(), potions.end(), counterName) != potions.end();
        }
    }

    if (alreadyKnown)
    {
        cout << "Already known effectiveness\n";
    }
    else
    {
        // Add to bestiary and alchemy knowledge
        bestiary.addEffectiveness(monsterName, counterName, isSign);

        // Add sign to alchemy knowledge if not already known
        if (isSign)
        {
            alchemy.addSign(counterName);
        }

        // Output appropriate message based on beast existence
        if (beastExists)
        {
            cout << "Bestiary entry updated: " << monsterName << "\n";
        }
        else
        {
            cout << "New bestiary entry added: " << monsterName << "\n";
        }
    }

    return 0;
}

/**
 * @brief Executes potion formula knowledge commands
 * @param input The validated formula knowledge string
 * @return 0 on successful execution
 * 
 * Parses potion formula ingredients and quantities, adds to alchemy knowledge if new
 * Format: "Geralt learns <potion> potion consists of quantity ingredient [, quantity ingredient]..."
 */
int WitcherTracker::executeFormulaKnowledge(const string &input)
{
    auto tokens = CommandParser::tokenizeInput(input);

    // Find key indices for parsing
    size_t potionIndex = 0, ofIndex = 0;

    for (size_t i = 2; i < tokens.size(); ++i)
    {
        if (tokens[i] == "potion")
        {
            potionIndex = i;
        }
        else if (tokens[i] == "of")
        {
            ofIndex = i;
            break;
        }
    }

    // Extract potion name (before "potion")
    string potionName;
    for (size_t i = 2; i < potionIndex; ++i)
    {
        if (i > 2)
            potionName += " ";
        potionName += tokens[i];
    }

    // Check if formula already known
    if (alchemy.hasPotion(potionName))
    {
        cout << "Already known formula\n";
        return 0;
    }

    // Parse ingredient list (after "of")
    vector<string> ingredients;
    vector<int> quantities;

    size_t tokenIndex = ofIndex + 1;
    while (tokenIndex < tokens.size())
    {
        int quantity = stoi(tokens[tokenIndex]);
        tokenIndex++;

        string ingredientName = tokens[tokenIndex];
        tokenIndex++;

        ingredients.push_back(ingredientName);
        quantities.push_back(quantity);

        // Skip comma if present
        if (tokenIndex < tokens.size() && tokens[tokenIndex] == ",")
        {
            tokenIndex++;
        }
    }

    // Add formula to alchemy knowledge
    alchemy.addPotionFormula(potionName, ingredients, quantities);

    cout << "New alchemy formula obtained: " << potionName << "\n";
    return 0;
}

/**
 * @brief Executes encounter commands
 * @param input The validated encounter string
 * @return 0 on successful execution
 * 
 * Handles monster encounters, checks for effective counters, consumes potions,
 * and awards trophies or reports failure
 * Format: "Geralt encounters a <monster>"
 */
int WitcherTracker::executeEncounter(const string &input)
{
    auto tokens = CommandParser::tokenizeInput(input);

    // Extract monster name (after "a")
    string monsterName;
    for (size_t i = 3; i < tokens.size(); ++i)
    {
        if (i > 3)
            monsterName += " ";
        monsterName += tokens[i];
    }

    // Check if beast is known
    Beast *beast = bestiary.getBeast(monsterName);
    if (!beast)
    {
        cout << "Geralt is unprepared and barely escapes with his life\n";
        return 0;
    }

    // Check for effective counters in inventory or signs
    bool hasEffectiveCounter = false;

    // Check for effective potions in inventory
    for (const auto &potionName : beast->effectivePotions)
    {
        if (inventory.getPotionQuantity(potionName) > 0)
        {
            hasEffectiveCounter = true;
            break;
        }
    }

    // Check for effective signs (always available if known)
    if (!hasEffectiveCounter && !beast->effectiveSigns.empty())
    {
        hasEffectiveCounter = true;
    }

    if (hasEffectiveCounter)
    {
        // Consume one of each effective potion in inventory
        for (const auto &potionName : beast->effectivePotions)
        {
            if (inventory.getPotionQuantity(potionName) > 0)
            {
                inventory.removePotion(potionName, 1);
            }
        }

        // Award trophy for successful encounter
        inventory.addTrophy(monsterName, 1);
        cout << "Geralt defeats " << monsterName << "\n";
    }
    else
    {
        cout << "Geralt is unprepared and barely escapes with his life\n";
    }

    return 0;
}

/**
 * @brief Executes specific inventory queries
 * @param input The validated specific inventory query string
 * @return 0 on successful execution
 * 
 * Queries inventory for specific item quantity and outputs the result
 * Format: "Total <category> <item> ?"
 */
int WitcherTracker::executeSpecificInventoryQuery(const string &input)
{
    auto tokens = CommandParser::tokenizeInput(input);

    string category = tokens[1];

    // Extract item name (everything between category and "?")
    string itemName;
    for (size_t i = 2; i < tokens.size() - 1; ++i)
    {
        if (i > 2)
            itemName += " ";
        itemName += tokens[i];
    }

    // Query appropriate inventory category
    int quantity = 0;
    if (category == "ingredient")
    {
        quantity = inventory.getIngredientQuantity(itemName);
    }
    else if (category == "potion")
    {
        quantity = inventory.getPotionQuantity(itemName);
    }
    else if (category == "trophy")
    {
        quantity = inventory.getTrophyQuantity(itemName);
    }

    cout << quantity << "\n";
    return 0;
}

/**
 * @brief Executes general inventory queries
 * @param input The validated general inventory query string
 * @return 0 on successful execution
 * 
 * Outputs all items in specified category or "None" if empty
 * Format: "Total <category> ?"
 */
int WitcherTracker::executeAllInventoryQuery(const string &input)
{
    auto tokens = CommandParser::tokenizeInput(input);

    string category = tokens[1];
    string result;

    // Get all items from appropriate category
    if (category == "ingredient")
    {
        result = inventory.getAllIngredients();
    }
    else if (category == "potion")
    {
        result = inventory.getAllPotions();
    }
    else if (category == "trophy")
    {
        result = inventory.getAllTrophies();
    }

    // Output result or "None" if empty
    if (result.empty())
    {
        cout << "None\n";
    }
    else
    {
        cout << result << "\n";
    }

    return 0;
}

/**
 * @brief Executes bestiary queries
 * @param input The validated bestiary query string
 * @return 0 on successful execution
 * 
 * Outputs effective counters for specified monster or reports no knowledge
 * Format: "What is effective against <monster> ?"
 */
int WitcherTracker::executeBestiaryQuery(const string &input)
{
    auto tokens = CommandParser::tokenizeInput(input);

    // Extract monster name (between "against" and "?")
    string monsterName;
    size_t startIndex = 4; // After "What is effective against"

        for (size_t i = startIndex; i < tokens.size() - 1; ++i)
    {
        if (i > startIndex)
            monsterName += " ";
        monsterName += tokens[i];
    }

    string result = bestiary.getEffectiveCounters(monsterName);

    // Output effective counters or report no knowledge
    if (result.empty())
    {
        cout << "No knowledge of " << monsterName << "\n";
    }
    else
    {
        cout << result << "\n";
    }

    return 0;
}

/**
 * @brief Executes alchemy queries
 * @param input The validated alchemy query string
 * @return 0 on successful execution
 * 
 * Outputs potion formula ingredients or reports no formula knowledge
 * Format: "What is in <potion> ?"
 */
int WitcherTracker::executeAlchemyQuery(const string &input)
{
    auto tokens = CommandParser::tokenizeInput(input);

    // Extract potion name (between "in" and "?")
    string potionName;
    for (size_t i = 3; i < tokens.size() - 1; ++i)
    {
        if (i > 3)
            potionName += " ";
        potionName += tokens[i];
    }

    string result = alchemy.getPotionIngredients(potionName);

    // Output formula ingredients or report no formula
    if (result.empty())
    {
        cout << "No formula for " << potionName << "\n";
    }
    else
    {
        cout << result << "\n";
    }

    return 0;
}