#include "WitcherTracker.h"

using namespace std;

/**
 * @brief AlchemyKnowledge class implementation - manages potion formulas and witcher signs
 * 
 * This class handles the storage and retrieval of alchemical knowledge including
 * potion recipes with ingredients/quantities and magical signs available to witchers.
 */

/**
 * @brief Adds a new potion formula to the knowledge base
 * @param potionName The name of the potion to add
 * @param ingredients Vector of ingredient names required for the potion
 * @param quantities Vector of quantities corresponding to each ingredient
 * @return void
 * @side_effects Creates or updates a potion entry in the potions map
 */
void AlchemyKnowledge::addPotionFormula(const string &potionName, const vector<string> &ingredients, const vector<int> &quantities)
{
    // Create or retrieve potion reference and populate with formula data
    Potion &potion = potions[potionName];
    potion.name = potionName;
    potion.ingredientNames = ingredients;
    potion.ingredientQuantities = quantities;
}

/**
 * @brief Adds a new sign to the knowledge base
 * @param signName The name of the sign to add
 * @return void
 * @side_effects Creates a new Sign entry in the signs map
 */
void AlchemyKnowledge::addSign(const string &signName)
{
    // Create new Sign object and store in signs collection
    signs[signName] = Sign(signName);
}

/**
 * @brief Retrieves a pointer to a specific potion
 * @param name The name of the potion to find
 * @return Pointer to the Potion object if found, nullptr otherwise
 */
Potion *AlchemyKnowledge::getPotion(const string &name)
{
    auto it = potions.find(name);
    // Return pointer to potion if exists, otherwise return null
    return (it != potions.end()) ? &it->second : nullptr;
}

/**
 * @brief Checks if a potion exists in the knowledge base
 * @param name The name of the potion to check
 * @return true if potion exists, false otherwise
 */
bool AlchemyKnowledge::hasPotion(const string &name) const
{
    return potions.find(name) != potions.end();
}

/**
 * @brief Checks if a sign exists in the knowledge base
 * @param name The name of the sign to check
 * @return true if sign exists, false otherwise
 */
bool AlchemyKnowledge::hasSign(const string &name) const
{
    return signs.find(name) != signs.end();
}

/**
 * @brief Retrieves formatted ingredient list for a specific potion
 * @param potionName The name of the potion to get ingredients for
 * @return Formatted string of ingredients sorted by quantity (desc) then name (asc), empty string if not found
 * 
 * Format: "quantity ingredient, quantity ingredient, ..."
 * Sorting: Primary by quantity (highest first), secondary by name (alphabetical)
 */
string AlchemyKnowledge::getPotionIngredients(const string &potionName) const
{
    auto it = potions.find(potionName);
    // Return empty string if potion doesn't exist or lacks formula
    if (it == potions.end() || !it->second.hasFormula())
    {
        return "";
    }

    const Potion &potion = it->second;
    vector<pair<string, int>> ingredientPairs;

    // Create ingredient-quantity pairs for sorting
    for (size_t i = 0; i < potion.ingredientNames.size(); ++i)
    {
        ingredientPairs.emplace_back(potion.ingredientNames[i], potion.ingredientQuantities[i]);
    }

    // Sort ingredients: primary by quantity (descending), secondary by name (ascending)
    sort(ingredientPairs.begin(), ingredientPairs.end(), [](const pair<string, int> &a, const pair<string, int> &b)
         {
             if (a.second != b.second)
             {
                 return a.second > b.second; // Sort by quantity first (highest to lowest)
             }
             return a.first < b.first; // If quantities are equal, sort by ingredient name alphabetically
         });

    // Build formatted result string
    string result;
    for (size_t i = 0; i < ingredientPairs.size(); ++i)
    {
        if (i > 0)
            result += ", "; // Add comma separator between ingredients
        result += to_string(ingredientPairs[i].second) + " " + ingredientPairs[i].first;
    }

    return result;
}