#include "WitcherTracker.h"

using namespace std;

/**
 * @brief Potion class implementation - manages potion recipes and ingredients
 * 
 * This class handles the storage of potion formulas including ingredient names
 * and their corresponding quantities required for brewing.
 */

/**
 * @brief Adds an ingredient with its quantity to the potion formula
 * @param ingredientName The name of the ingredient to add
 * @param ingredientQuantity The quantity of the ingredient required
 * @return void
 * @side_effects Appends ingredient name and quantity to respective vectors
 * 
 * Maintains parallel vectors where ingredientNames[i] corresponds to ingredientQuantities[i]
 */
void Potion::addIngredient(const string &ingredientName, int ingredientQuantity)
{
    // Add ingredient name to the names list
    ingredientNames.push_back(ingredientName);
    // Add corresponding quantity to maintain parallel structure
    ingredientQuantities.push_back(ingredientQuantity);
}