#include "WitcherTracker.h"

/**
 * @brief Beast class implementation - manages beast combat knowledge
 * 
 * This class handles the storage of effective signs and potions that can be used
 * against specific beasts, preventing duplicate entries in the collections.
 */

/**
 * @brief Adds an effective sign to the beast's weakness list
 * @param signName The name of the sign effective against this beast
 * @return void
 * @side_effects Adds signName to effectiveSigns vector if not already present
 */
void Beast::addEffectiveSign(const std::string &signName)
{
    // Check if sign already exists in the list to prevent duplicates
    if (std::find(effectiveSigns.begin(), effectiveSigns.end(), signName) == effectiveSigns.end())
    {
        effectiveSigns.push_back(signName);
    }
}

/**
 * @brief Adds an effective potion to the beast's combat strategy list
 * @param potionName The name of the potion effective against this beast
 * @return void
 * @side_effects Adds potionName to effectivePotions vector if not already present
 */
void Beast::addEffectivePotion(const std::string &potionName)
{
    // Check if potion already exists in the list to prevent duplicates
    if (std::find(effectivePotions.begin(), effectivePotions.end(), potionName) == effectivePotions.end())
    {
        effectivePotions.push_back(potionName);
    }
}