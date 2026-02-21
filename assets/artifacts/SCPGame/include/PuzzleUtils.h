#pragma once
#include <vector>
#include <string>

/** 
* @file PuzzleUtils.h 
* @brief Utility functions for working with lever patterns. 
* 
* These helpers convert between: 
* 
* - Raw comma‑separated strings (e.g. 1,0,1,1) 
* 
* - Internal boolean vectors (true = UP, false = DOWN) 
* 
* - Human‑readable descriptions (UP / DOWN / UP / UP) 
* 
* Design Notes: 
* 
* - These functions are intentionally free functions rather than a class. 
* 
* - Lever puzzles and PuzzleEngine both rely on this shared formatting logic. 
* 
* - The utilities contain no game logic; they only transform data. 
*/ 

/** 
* @brief Parse a comma‑separated pattern string into a vector<bool>. 
* 
* e.g., 1, 0, 1 -> true, false, true
*/
std::vector<bool> parsePattern(const std::string& s);

/**
 * @brief Convert a vector<bool> back into a comma‑separated pattern string.
 * 
 * e.g., true, false, true -> 1, 0, 1
 */
std::string patternToString(const std::vector<bool>& v);

/**
 * @brief Convert a raw pattern string into a human‑readable form.
 * 
 * e.g., 1, 0, 1 -> UP / DOWN / UP
 */
std::string formatPattern(const std::string& pattern);
