#include "WitcherTracker.h"

using namespace std;

/**
 * @brief Bestiary class implementation - manages collection of beasts and their combat data
 * 
 * This class serves as a comprehensive database of creatures, storing information about
 * effective signs and potions for each beast. Provides methods for adding beasts,
 * managing effectiveness data, and retrieving formatted counter lists.
 */

/**
 * @brief Adds a new beast to the bestiary if it doesn't already exist
 * @param name The name of the beast to add
 * @return void
 * @side_effects Creates a new Beast entry in the beasts map if not present
 */
void Bestiary::addBeast(const string &name)
{
    // Only create new beast if it doesn't already exist to avoid overwriting data
    if (beasts.find(name) == beasts.end())
    {
        beasts[name] = Beast(name);
    }
}

/**
 * @brief Adds effectiveness information for a specific beast
 * @param beastName The name of the beast to add effectiveness data for
 * @param counter The name of the sign or potion effective against the beast
 * @param isSign true if counter is a sign, false if it's a potion
 * @return void
 * @side_effects Ensures beast exists and adds the counter to appropriate effectiveness list
 */
void Bestiary::addEffectiveness(const string &beastName, const string &counter, bool isSign)
{
    // Ensure beast exists in bestiary before adding effectiveness data
    addBeast(beastName);
    
    // Add counter to appropriate list based on type
    if (isSign)
    {
        beasts[beastName].addEffectiveSign(counter);
    }
    else
    {
        beasts[beastName].addEffectivePotion(counter);
    }
}

/**
 * @brief Retrieves a pointer to a specific beast
 * @param name The name of the beast to find
 * @return Pointer to the Beast object if found, nullptr otherwise
 */
Beast *Bestiary::getBeast(const string &name)
{
    auto it = beasts.find(name);
    // Return pointer to beast if exists, otherwise return null
    return (it != beasts.end()) ? &it->second : nullptr;
}

/**
 * @brief Retrieves all effective counters for a beast in alphabetical order
 * @param beastName The name of the beast to get counters for
 * @return Comma-separated string of all effective signs and potions, sorted alphabetically
 *         Returns empty string if beast not found
 * 
 * Combines both potions and signs into a single sorted list for comprehensive combat reference
 */
string Bestiary::getEffectiveCounters(const string &beastName) const
{
    auto it = beasts.find(beastName);
    // Return empty string if beast doesn't exist
    if (it == beasts.end())
    {
        return "";
    }

    vector<string> allCounters;

    // Collect all effective potions
    for (const auto &potion : it->second.effectivePotions)
    {
        allCounters.push_back(potion);
    }

    // Collect all effective signs
    for (const auto &sign : it->second.effectiveSigns)
    {
        allCounters.push_back(sign);
    }

    // Sort all counters alphabetically for consistent output
    sort(allCounters.begin(), allCounters.end());

    // Build comma-separated result string
    string result;
    for (size_t i = 0; i < allCounters.size(); ++i)
    {
        if (i > 0)
            result += ", "; // Add comma separator between counters
        result += allCounters[i];
    }

    return result;
}