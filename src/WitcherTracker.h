#ifndef WITCHER_TRACKER_H
#define WITCHER_TRACKER_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <sstream>
#include <cctype>

using namespace std;

/**
 * @file WitcherTracker.h
 * @brief A comprehensive tracking system for managing witcher gameplay elements
 * 
 * This system handles inventory management, bestiary knowledge, alchemy formulas,
 * and command processing for a text-based witcher game.
 */

//========================================================================
// SYSTEM CONSTANTS
//========================================================================

// Maximum limits for various game elements to prevent memory overflow
constexpr int MAX_INPUT_LENGTH = 1024;          ///< Maximum characters in user input
constexpr int MAX_TOKEN_LENGTH = 1024;          ///< Maximum length of parsed tokens
constexpr int MAX_TOKENS = 1024;                ///< Maximum tokens per command
constexpr int MAX_ITEMS = 1024;                 ///< Maximum total items in inventory
constexpr int MAX_INGREDIENTS = 1024;           ///< Maximum ingredient types
constexpr int MAX_TROPHIES = 1024;              ///< Maximum trophy types
constexpr int MAX_POTIONS = 1024;               ///< Maximum potion types
constexpr int MAX_SIGNS = 1024;                 ///< Maximum magical signs
constexpr int MAX_BEASTS = 1024;                ///< Maximum beast entries
constexpr int MAX_EFFECTIVENESS = 1024;         ///< Maximum effectiveness entries
constexpr int MAX_POTION_INGREDIENTS = 1024;    ///< Maximum ingredients per potion

//========================================================================
// ENUMERATIONS
//========================================================================

/**
 * @enum CommandType
 * @brief Defines all supported command types for game actions and queries
 * 
 * Used by the command parser to categorize user input and route to
 * appropriate execution methods.
 */
enum class CommandType
{
    INVALID_COMMAND,           ///< Unrecognized or malformed command
    ACTION_LOOT,              ///< Collect items from environment
    ACTION_TRADE,             ///< Exchange items with NPCs
    ACTION_BREW,              ///< Create potions from ingredients
    KNOWLEDGE_EFFECTIVENESS,   ///< Learn beast weaknesses
    KNOWLEDGE_POTION_FORMULA, ///< Learn potion recipes
    ENCOUNTER,                ///< Meet beasts in the wild
    QUERY_SPECIFIC_INVENTORY, ///< Check specific item quantities
    QUERY_ALL_INVENTORY,      ///< View complete inventory
    QUERY_BESTIARY,           ///< Check beast information
    QUERY_ALCHEMY,            ///< View potion recipes
    EXIT_COMMAND              ///< Terminate program
};

/**
 * @enum ItemCategory
 * @brief Categories of items that can be stored in inventory
 * 
 * Provides type safety for item management operations.
 */
enum class ItemCategory
{
    INGREDIENT,    ///< Raw materials for brewing
    POTION,        ///< Completed magical brews
    TROPHY         ///< Beast parts as victory proof
};

//========================================================================
// FORWARD DECLARATIONS
//========================================================================

class Inventory;
class Bestiary;
class AlchemyKnowledge;
class CommandParser;

//========================================================================
// GAME ENTITY CLASSES
//========================================================================

/**
 * @class Ingredient
 * @brief Represents a brewing ingredient with quantity tracking
 * 
 * Simple data structure for storing ingredient name and current quantity
 * in the player's inventory.
 */
class Ingredient
{
public:
    string name;        ///< Unique ingredient identifier
    int quantity;       ///< Current amount owned

    /**
     * @brief Constructor with optional initialization
     * @param n Ingredient name (default: empty)
     * @param q Initial quantity (default: 0)
     */
    Ingredient(const string &n = "", int q = 0) : name(n), quantity(q) {}
};

/**
 * @class Potion
 * @brief Represents a magical potion with recipe and inventory data
 * 
 * Stores both the brewing recipe (if known) and current inventory quantity.
 * Recipes consist of ingredient names paired with required quantities.
 */
class Potion
{
public:
    string name;                        ///< Unique potion identifier
    vector<string> ingredientNames;     ///< Required ingredient types
    vector<int> ingredientQuantities;   ///< Required amounts per ingredient
    int quantity;                       ///< Current potions in inventory

    /**
     * @brief Constructor initializes potion with zero quantity
     * @param n Potion name (default: empty)
     */
    Potion(const string &n = "") : name(n), quantity(0) {}

    /**
     * @brief Adds an ingredient requirement to the recipe
     * @param ingredientName Name of required ingredient
     * @param ingredientQuantity Amount needed for brewing
     * 
     * Side effects: Modifies ingredient vectors to store recipe data
     */
    void addIngredient(const string &ingredientName, int ingredientQuantity);
    
    /**
     * @brief Checks if brewing recipe is known
     * @return true if recipe contains ingredients, false otherwise
     * 
     * Used to determine if player can attempt brewing this potion
     */
    bool hasFormula() const { return !ingredientNames.empty(); }
};

/**
 * @class Trophy
 * @brief Represents collectible beast trophies
 * 
 * Trophies serve as proof of victory and may have trade or quest value.
 */
class Trophy
{
public:
    string name;        ///< Trophy identifier (typically beast name)
    int quantity;       ///< Number collected

    /**
     * @brief Constructor with optional initialization
     * @param n Trophy name (default: empty)
     * @param q Initial quantity (default: 0)
     */
    Trophy(const string &n = "", int q = 0) : name(n), quantity(q) {}
};

/**
 * @class Sign
 * @brief Represents a magical sign ability
 * 
 * Signs are combat abilities that may be effective against certain beasts.
 */
class Sign
{
public:
    string name;        ///< Unique sign identifier

    /**
     * @brief Constructor with name initialization
     * @param n Sign name (default: empty)
     */
    Sign(const string &n = "") : name(n) {}
};

/**
 * @class Beast
 * @brief Represents a creature with known combat weaknesses
 * 
 * Stores tactical information about which signs and potions are
 * effective against this beast type.
 */
class Beast
{
public:
    string name;                    ///< Beast identifier
    vector<string> effectiveSigns;  ///< Signs that counter this beast
    vector<string> effectivePotions;///< Potions effective against this beast

    /**
     * @brief Constructor with name initialization
     * @param n Beast name (default: empty)
     */
    Beast(const string &n = "") : name(n) {}

    /**
     * @brief Records a sign as effective against this beast
     * @param signName Name of the effective sign
     * 
     * Side effects: Adds sign to effectiveness list if not already present
     */
    void addEffectiveSign(const string &signName);
    
    /**
     * @brief Records a potion as effective against this beast
     * @param potionName Name of the effective potion
     * 
     * Side effects: Adds potion to effectiveness list if not already present
     */
    void addEffectivePotion(const string &potionName);
};

//========================================================================
// MANAGEMENT SYSTEM CLASSES
//========================================================================

/**
 * @class Inventory
 * @brief Centralized storage system for all player items
 * 
 * Manages ingredients, potions, and trophies with quantity tracking,
 * addition/removal operations, and query capabilities.
 */
class Inventory
{
private:
    map<string, int> ingredients;   ///< Ingredient name -> quantity mapping
    map<string, int> potions;       ///< Potion name -> quantity mapping
    map<string, int> trophies;      ///< Trophy name -> quantity mapping

public:
    /**
     * @brief Adds ingredients to inventory
     * @param name Ingredient identifier
     * @param quantity Amount to add (must be positive)
     * 
     * Side effects: Creates new entry or increases existing quantity
     */
    void addIngredient(const string &name, int quantity);
    
    /**
     * @brief Adds potions to inventory
     * @param name Potion identifier  
     * @param quantity Amount to add (must be positive)
     * 
     * Side effects: Creates new entry or increases existing quantity
     */
    void addPotion(const string &name, int quantity);
    
    /**
     * @brief Adds trophies to inventory
     * @param name Trophy identifier
     * @param quantity Amount to add (must be positive)
     * 
     * Side effects: Creates new entry or increases existing quantity
     */
    void addTrophy(const string &name, int quantity);

    /**
     * @brief Attempts to remove ingredients from inventory
     * @param name Ingredient identifier
     * @param quantity Amount to remove
     * @return true if sufficient quantity available and removed, false otherwise
     * 
     * Side effects: Decreases quantity if successful, no change if insufficient
     */
    bool removeIngredient(const string &name, int quantity);
    
    /**
     * @brief Attempts to remove potions from inventory
     * @param name Potion identifier
     * @param quantity Amount to remove
     * @return true if sufficient quantity available and removed, false otherwise
     */
    bool removePotion(const string &name, int quantity);
    
    /**
     * @brief Attempts to remove trophies from inventory
     * @param name Trophy identifier
     * @param quantity Amount to remove
     * @return true if sufficient quantity available and removed, false otherwise
     */
    bool removeTrophy(const string &name, int quantity);

    /**
     * @brief Queries current ingredient quantity
     * @param name Ingredient identifier
     * @return Current quantity (0 if item not found)
     */
    int getIngredientQuantity(const string &name) const;
    
    /**
     * @brief Queries current potion quantity
     * @param name Potion identifier
     * @return Current quantity (0 if item not found)
     */
    int getPotionQuantity(const string &name) const;
    
    /**
     * @brief Queries current trophy quantity
     * @param name Trophy identifier
     * @return Current quantity (0 if item not found)
     */
    int getTrophyQuantity(const string &name) const;

    /**
     * @brief Generates formatted listing of all ingredients
     * @return String containing all ingredients with quantities
     */
    string getAllIngredients() const;
    
    /**
     * @brief Generates formatted listing of all potions
     * @return String containing all potions with quantities
     */
    string getAllPotions() const;
    
    /**
     * @brief Generates formatted listing of all trophies
     * @return String containing all trophies with quantities
     */
    string getAllTrophies() const;
};

/**
 * @class Bestiary
 * @brief Knowledge database for beast combat information
 * 
 * Stores and manages information about beast weaknesses, allowing
 * players to record and query effective combat strategies.
 */
class Bestiary
{
private:
    map<string, Beast> beasts;      ///< Beast name -> Beast data mapping

public:
    /**
     * @brief Creates new beast entry in bestiary
     * @param name Beast identifier
     * 
     * Side effects: Adds empty beast entry if not already present
     */
    void addBeast(const string &name);
    
    /**
     * @brief Records effectiveness data for a beast
     * @param beastName Target beast identifier
     * @param counter Name of effective sign or potion
     * @param isSign true for sign effectiveness, false for potion
     * 
     * Side effects: Creates beast entry if needed, adds counter to appropriate list
     */
    void addEffectiveness(const string &beastName, const string &counter, bool isSign);
    
    /**
     * @brief Retrieves beast data for modification
     * @param name Beast identifier
     * @return Pointer to Beast object or nullptr if not found
     */
    Beast *getBeast(const string &name);
    
    /**
     * @brief Generates formatted effectiveness information
     * @param beastName Beast to query
     * @return String listing effective signs and potions for this beast
     */
    string getEffectiveCounters(const string &beastName) const;
};

/**
 * @class AlchemyKnowledge
 * @brief Repository for potion recipes and magical sign knowledge
 * 
 * Manages learned potion formulas and available magical signs,
 * enabling brewing operations and combat planning.
 */
class AlchemyKnowledge
{
private:
    map<string, Potion> potions;    ///< Potion name -> recipe mapping
    map<string, Sign> signs;        ///< Sign name -> sign data mapping

public:
    /**
     * @brief Stores or updates a potion recipe
     * @param potionName Potion identifier
     * @param ingredients List of required ingredient names
     * @param quantities List of required amounts (parallel to ingredients)
     * 
     * Side effects: Creates/updates potion entry with complete recipe
     */
    void addPotionFormula(const string &potionName, const vector<string> &ingredients, const vector<int> &quantities);
    
    /**
     * @brief Adds a magical sign to knowledge base
     * @param signName Sign identifier
     * 
     * Side effects: Creates sign entry if not already present
     */
    void addSign(const string &signName);

    /**
     * @brief Retrieves potion data for modification
     * @param name Potion identifier
     * @return Pointer to Potion object or nullptr if not found
     */
    Potion *getPotion(const string &name);
    
    /**
     * @brief Checks if potion recipe is known
     * @param name Potion identifier
     * @return true if potion exists in knowledge base
     */
    bool hasPotion(const string &name) const;
    
    /**
     * @brief Checks if sign is available
     * @param name Sign identifier
     * @return true if sign exists in knowledge base
     */
    bool hasSign(const string &name) const;

    /**
     * @brief Generates formatted recipe information
     * @param potionName Potion to query
     * @return String listing required ingredients and quantities
     */
    string getPotionIngredients(const string &potionName) const;
};

//========================================================================
// COMMAND PROCESSING
//========================================================================

/**
 * @class CommandParser
 * @brief Static utility class for parsing and validating user commands
 * 
 * Provides text processing methods for tokenizing input, validating
 * command formats, and determining command types without state.
 */
class CommandParser
{
public:
    /**
     * @brief Splits input string into individual tokens
     * @param input Raw user input string
     * @return Vector of space-separated tokens
     * 
     * Handles whitespace normalization and comma separation
     */
    static vector<string> tokenizeInput(const string &input);
    
    /**
     * @brief Normalizes input by removing extra whitespace
     * @param input Raw input string
     * @return Cleaned string with normalized spacing
     */
    static string cleanInputLine(const string &input);
    
    /**
     * @brief Validates positive integer format
     * @param token String to validate
     * @return true if token represents valid positive integer
     */
    static bool isPositiveInteger(const string &token);
    
    /**
     * @brief Validates alphabetic-only content
     * @param token String to validate  
     * @return true if token contains only letters
     */
    static bool isAlphabeticOnly(const string &token);
    
    /**
     * @brief Detects comma spacing errors in token list
     * @param tokens Vector of parsed tokens
     * @return true if improper comma spacing detected
     */
    static bool hasCommaSpacingError(const vector<string> &tokens);

    // Command pattern validation methods - each checks specific command format
    
    /**
     * @brief Validates loot action command structure
     * @param input Command string to validate
     * @return true if matches expected loot format
     */
    static bool isLootAction(const string &input);
    
    /**
     * @brief Validates trade action command structure
     * @param input Command string to validate
     * @return true if matches expected trade format
     */
    static bool isTradeAction(const string &input);
    
    /**
     * @brief Validates brew action command structure
     * @param input Command string to validate
     * @return true if matches expected brew format
     */
    static bool isBrewAction(const string &input);
    
    /**
     * @brief Validates effectiveness knowledge command structure
     * @param input Command string to validate
     * @return true if matches expected effectiveness format
     */
    static bool isEffectivenessKnowledge(const string &input);
    
    /**
     * @brief Validates potion formula command structure
     * @param input Command string to validate
     * @return true if matches expected formula format
     */
    static bool isPotionFormulaKnowledge(const string &input);
    
    /**
     * @brief Validates encounter command structure
     * @param input Command string to validate
     * @return true if matches expected encounter format
     */
    static bool isEncounterSentence(const string &input);
    
    /**
     * @brief Validates inventory query command structure
     * @param input Command string to validate
     * @param isSpecific Output parameter indicating specific vs. general query
     * @return true if matches expected inventory query format
     */
    static bool isInventoryQuery(const string &input, bool &isSpecific);
    
    /**
     * @brief Validates bestiary query command structure
     * @param input Command string to validate
     * @return true if matches expected bestiary format
     */
    static bool isBestiaryQuery(const string &input);
    
    /**
     * @brief Validates alchemy query command structure
     * @param input Command string to validate
     * @return true if matches expected alchemy format
     */
    static bool isAlchemyQuery(const string &input);
    
    /**
     * @brief Validates exit command structure
     * @param input Command string to validate
     * @return true if matches expected exit format
     *      */
    static bool isExitCommand(const string &input);
    
    /**
     * @brief Determines command type from input string
     * @param input Command string to analyze
     * @param cmdType Output parameter for determined command type
     * @return true if command is valid and type successfully determined
     * 
     * Side effects: Sets cmdType to appropriate enum value if valid
     */
    static bool isValidCommand(const string &input, CommandType &cmdType);
    
    /**
     * @brief Validates potion name token format
     * @param token Token to validate as potion name
     * @return true if token meets potion naming requirements
     */
    static bool isValidPotionNameToken(const string &token);
};

//========================================================================
// MAIN APPLICATION CLASS
//========================================================================

/**
 * @class WitcherTracker
 * @brief Primary application controller coordinating all game systems
 * 
 * Serves as the main interface for processing user commands and managing
 * interactions between inventory, bestiary, and alchemy subsystems.
 * Handles command parsing, validation, and execution routing.
 */
class WitcherTracker
{
private:
    Inventory inventory;        ///< Player's item management system
    Bestiary bestiary;         ///< Beast knowledge database
    AlchemyKnowledge alchemy;  ///< Potion and sign knowledge repository

public:
    /**
     * @brief Processes a single line of user input
     * @param line Input command string to execute
     * @return Execution status code (0 for success, negative for errors)
     * 
     * Primary entry point for command processing. Handles command validation,
     * type determination, and routing to appropriate execution methods.
     */
    int executeLine(const string &line);

private:
    /**
     * @brief Routes validated commands to specific execution methods
     * @param input Validated command string
     * @param cmdType Determined command type from parsing
     * @return Execution status code
     * 
     * Internal dispatcher that calls appropriate execution method based on
     * command type. Assumes input has already been validated.
     */
    int executeCommand(const string &input, CommandType cmdType);

    //====================================================================
    // COMMAND EXECUTION METHODS
    //====================================================================

    /**
     * @brief Executes loot action to add items to inventory
     * @param input Loot command string
     * @return 0 on success, negative on error
     * 
     * Processes "loot" commands to add specified quantities of items
     * to the player's inventory from environmental sources.
     */
    int executeLootAction(const string &input);
    
    /**
     * @brief Executes trade action to exchange items
     * @param input Trade command string  
     * @return 0 on success, negative on error
     * 
     * Processes "trade" commands to remove items from inventory
     * in exchange for other items or services.
     */
    int executeTradeAction(const string &input);
    
    /**
     * @brief Executes brew action to create potions from ingredients
     * @param input Brew command string
     * @return 0 on success, negative on error
     * 
     * Processes "brew" commands to consume ingredients and create potions
     * according to known recipes. Validates ingredient availability.
     */
    int executeBrewAction(const string &input);
    
    /**
     * @brief Executes effectiveness knowledge acquisition
     * @param input Effectiveness command string
     * @return 0 on success, negative on error
     * 
     * Processes commands that teach the player which signs or potions
     * are effective against specific beast types.
     */
    int executeEffectivenessKnowledge(const string &input);
    
    /**
     * @brief Executes potion formula learning
     * @param input Formula command string
     * @return 0 on success, negative on error
     * 
     * Processes commands that teach potion recipes, storing ingredient
     * requirements for future brewing operations.
     */
    int executeFormulaKnowledge(const string &input);
    
    /**
     * @brief Executes beast encounter processing  
     * @param input Encounter command string
     * @return 0 on success, negative on error
     * 
     * Processes beast encounter events, potentially updating bestiary
     * information or triggering combat-related responses.
     */
    int executeEncounter(const string &input);
    
    /**
     * @brief Executes specific inventory item queries
     * @param input Specific inventory query string
     * @return 0 on success, negative on error
     * 
     * Processes queries for specific item quantities, returning current
     * inventory counts for requested items.
     */
    int executeSpecificInventoryQuery(const string &input);
    
    /**
     * @brief Executes complete inventory display
     * @param input All inventory query string
     * @return 0 on success, negative on error
     * 
     * Processes requests to display all inventory contents, including
     * all categories of items with their quantities.
     */
    int executeAllInventoryQuery(const string &input);
    
    /**
     * @brief Executes bestiary information queries
     * @param input Bestiary query string
     * @return 0 on success, negative on error
     * 
     * Processes requests for beast information, returning known
     * effectiveness data for specified creatures.
     */
    int executeBestiaryQuery(const string &input);
    
    /**
     * @brief Executes alchemy knowledge queries
     * @param input Alchemy query string
     * @return 0 on success, negative on error
     * 
     * Processes requests for potion recipe information, returning
     * ingredient requirements for specified potions.
     */
    int executeAlchemyQuery(const string &input);
};

#endif // WITCHER_TRACKER_H