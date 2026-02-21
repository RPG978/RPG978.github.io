#pragma once
#include <string>

/**
 * @class Puzzle
 * @brief Immutable puzzle metadata loaded from the database, plus minimal runtime state.
 *
 * Responsibilities:
 * 
 *   - Store puzzle configuration (type, group, prompt, solution, reward)
 * 
 *   - Track whether the puzzle has been solved
 * 
 *   - Provide optional per‑puzzle runtime state (e.g., scrambled word)
 *
 * Design Notes:
 * 
 *   - Puzzle contains no puzzle logic; PuzzleEngine executes puzzles.
 * 
 *   - All fields except solved and runtime scratch data are immutable after construction.
 * 
 *   - scrambledWord is used only by the word‑scramble puzzle and persists for the session.
 */
class Puzzle {
public:
    /**
     * @brief Construct a puzzle from database fields.
     */
    Puzzle(int id,
        const std::string& type,
        const std::string& group,
        const std::string& name,
        int roomId,
        const std::string& trigger,
        const std::string& prompt,
        const std::string& solutionData,
        const std::string& rewardData,
        bool repeatable)
        : id(id),
        puzzleType(type),
        puzzleGroup(group),
        name(name),
        roomId(roomId),
        trigger(trigger),
        prompt(prompt),
        solutionData(solutionData),
        rewardData(rewardData),
        repeatable(repeatable),
        solved(false)
    {}

    /******************
    * BASIC ACCESSORS *
    ******************/

    int getId() const { 
        return id; 
    }

    const std::string& getType() const { 
        return puzzleType; 
    }

    const std::string& getGroup() const { 
        return puzzleGroup; 
    }

    const std::string& getName() const { 
        return name; 
    }

    int getRoomId() const { 
        return roomId; 
    }

    const std::string& getTrigger() const { 
        return trigger; 
    }

    const std::string& getPrompt() const { 
        return prompt; 
    }

    const std::string& getSolutionData() const { 
        return solutionData; 
    }

    const std::string& getRewardData() const { 
        return rewardData; 
    }

    bool isRepeatable() const { 
        return repeatable; 
    }

    /***************
    * SOLVED STATE *
    ***************/

    bool isSolved() const { 
        return solved; 
    }

    void setSolved(bool v) { 
        solved = v; 
    }

    /***********************
    * RUNTIME SCRATCH DATA *
    ***********************/

    /**
     * @brief Get the cached scrambled word for this puzzle.
     *        Used only by the word‑scramble puzzle type.
     */
    const std::string& getScrambledWord() const { 
        return scrambledWord; 
    } 

    /**
     * @brief Set the scrambled word (generated once per session).
     */
    void setScrambledWord(const std::string& s) { 
        scrambledWord = s; 
    }


private:
    // Immutable database fields
    int id;
    std::string puzzleType;
    std::string puzzleGroup;
    std::string name;
    int roomId;
    std::string trigger;
    std::string prompt;
    std::string solutionData;
    std::string rewardData;
    bool repeatable;

    // Runtime state
    bool solved;

    // Scratch data for specific puzzle types
    std::string scrambledWord;
};
