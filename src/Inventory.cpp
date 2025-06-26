#include "WitcherTracker.h"

using namespace std;

/**
 * @brief Inventory class implementation - manages Geralt's collection of items
 * 
 * This class handles the storage and management of ingredients, potions, and trophies
 * with functionality for adding, removing, querying quantities, and generating
 * formatted inventory lists sorted alphabetically.
 */

/**
 * @brief Adds ingredients to the inventory
 * @param name The name of the ingredient to add
 * @param quantity The amount of ingredient to add
 * @return void
 * @side_effects Increases the ingredient quantity in the ingredients map
 */
void Inventory::addIngredient(const string &name, int quantity)
{
    // Accumulate ingredient quantity using map's default initialization
    ingredients[name] += quantity;
}

/**
 * @brief Adds potions to the inventory
 * @param name The name of the potion to add
 * @param quantity The amount of potion to add
 * @return void
 * @side_effects Increases the potion quantity in the potions map
 */
void Inventory::addPotion(const string &name, int quantity)
{
    // Accumulate potion quantity using map's default initialization
    potions[name] += quantity;
}

/**
 * @brief Adds trophies to the inventory
 * @param name The name of the trophy to add
 * @param quantity The amount of trophy to add
 * @return void
 * @side_effects Increases the trophy quantity in the trophies map
 */
void Inventory::addTrophy(const string &name, int quantity)
{
    // Accumulate trophy quantity using map's default initialization
    trophies[name] += quantity;
}

/**
 * @brief Removes ingredients from the inventory if sufficient quantity exists
 * @param name The name of the ingredient to remove
 * @param quantity The amount of ingredient to remove
 * @return true if removal successful, false if insufficient quantity
 * @side_effects Decreases ingredient quantity if removal is possible
 */
bool Inventory::removeIngredient(const string &name, int quantity)
{
    // Check if sufficient quantity exists before removal
    if (ingredients[name] >= quantity)
    {
        ingredients[name] -= quantity;
        return true;
    }
    return false;
}

/**
 * @brief Removes potions from the inventory if sufficient quantity exists
 * @param name The name of the potion to remove
 * @param quantity The amount of potion to remove
 * @return true if removal successful, false if insufficient quantity
 * @side_effects Decreases potion quantity if removal is possible
 */
bool Inventory::removePotion(const string &name, int quantity)
{
    // Check if sufficient quantity exists before removal
    if (potions[name] >= quantity)
    {
        potions[name] -= quantity;
        return true;
    }
    return false;
}

/**
 * @brief Removes trophies from the inventory if sufficient quantity exists
 * @param name The name of the trophy to remove
 * @param quantity The amount of trophy to remove
 * @return true if removal successful, false if insufficient quantity
 * @side_effects Decreases trophy quantity if removal is possible
 */
bool Inventory::removeTrophy(const string &name, int quantity)
{
    // Check if sufficient quantity exists before removal
    if (trophies[name] >= quantity)
    {
        trophies[name] -= quantity;
        return true;
    }
    return false;
}

/**
 * @brief Retrieves the quantity of a specific ingredient
 * @param name The name of the ingredient to query
 * @return The quantity of the ingredient, 0 if not found
 */
int Inventory::getIngredientQuantity(const string &name) const
{
    auto it = ingredients.find(name);
    // Return quantity if found, otherwise return 0 for non-existent items
    return (it != ingredients.end()) ? it->second : 0;
}

/**
 * @brief Retrieves the quantity of a specific potion
 * @param name The name of the potion to query
 * @return The quantity of the potion, 0 if not found
 */
int Inventory::getPotionQuantity(const string &name) const
{
    auto it = potions.find(name);
    // Return quantity if found, otherwise return 0 for non-existent items
    return (it != potions.end()) ? it->second : 0;
}

/**
 * @brief Retrieves the quantity of a specific trophy
 * @param name The name of the trophy to query
 * @return The quantity of the trophy, 0 if not found
 */
int Inventory::getTrophyQuantity(const string &name) const
{
    auto it = trophies.find(name);
    // Return quantity if found, otherwise return 0 for non-existent items
    return (it != trophies.end()) ? it->second : 0;
}

/**
 * @brief Generates formatted string of all ingredients in inventory
 * @return Comma-separated string of ingredients with quantities, sorted alphabetically
 *         Format: "quantity ingredient, quantity ingredient, ..."
 *         Returns empty string if no ingredients
 */
string Inventory::getAllIngredients() const
{
    vector<pair<string, int>> sortedIngredients;
    
    // Collect only ingredients with positive quantities
    for (const auto &pair : ingredients)
    {
        if (pair.second > 0)
        {
            sortedIngredients.emplace_back(pair);
        }
    }

    // Sort alphabetically by ingredient name for consistent output
    sort(sortedIngredients.begin(), sortedIngredients.end());

    // Build formatted result string
    string result;
    for (size_t i = 0; i < sortedIngredients.size(); ++i)
    {
        if (i > 0)
            result += ", "; // Add comma separator between items
        result += to_string(sortedIngredients[i].second) + " " + sortedIngredients[i].first;
    }
    return result;
}

/**
 * @brief Generates formatted string of all potions in inventory
 * @return Comma-separated string of potions with quantities, sorted alphabetically
 *         Format: "quantity potion, quantity potion, ..."
 *         Returns empty string if no potions
 */
string Inventory::getAllPotions() const
{
    vector<pair<string, int>> sortedPotions;
    
    // Collect only potions with positive quantities
    for (const auto &pair : potions)
    {
        if (pair.second > 0)
        {
            sortedPotions.emplace_back(pair);
        }
    }

    // Sort alphabetically by potion name for consistent output
    sort(sortedPotions.begin(), sortedPotions.end());

    // Build formatted result string
    string result;
    for (size_t i = 0; i < sortedPotions.size(); ++i)
    {
        if (i > 0)
            result += ", "; // Add comma separator between items
        result += to_string(sortedPotions[i].second) + " " + sortedPotions[i].first;
    }
    return result;
}

/**
 * @brief Generates formatted string of all trophies in inventory
 * @return Comma-separated string of trophies with quantities, sorted alphabetically
 *         Format: "quantity trophy, quantity trophy, ..."
 *         Returns empty string if no trophies
 */
string Inventory::getAllTrophies() const
{
    vector<pair<string, int>> sortedTrophies;
    
    // Collect only trophies with positive quantities
    for (const auto &pair : trophies)
    {
        if (pair.second > 0)
        {
            sortedTrophies.emplace_back(pair);
        }
    }

    // Sort alphabetically by trophy name for consistent output
    sort(sortedTrophies.begin(), sortedTrophies.end());

    // Build formatted result string
    string result;
    for (size_t i = 0; i < sortedTrophies.size(); ++i)
    {
        if (i > 0)
            result += ", "; // Add comma separator between items
        result += to_string(sortedTrophies[i].second) + " " + sortedTrophies[i].first;
    }
    return result;
}