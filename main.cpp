#include <iostream>
#include <string>
#include <algorithm>
#include <termios.h>
#include <unistd.h>
#include <chrono>
#include <random>

// Display the rules of the game
void displayRules()
{
    std::cout << "\nWelcome to Wordle!\n";
    std::cout << "The objective of the game is to guess a word of a specific length within a certain number of rounds.\n\n";
    std::cout << "After each guess, the program will provide feedback as follows:\n";
    std::cout << "- 'Y' indicates a correct letter in the correct position.\n";
    std::cout << "- 'O' indicates a correct letter in the wrong position.\n";
    std::cout << "- 'X' indicates a wrong letter.\n\n";
    std::cout << "Guess the word correctly within the given rounds to win the game!\n\n";
    std::cout << "Let's get started!\n";
}

// Get the desired word length from the user
int getWordLength()
{
    int wordLength;
    while (true)
    {
        std::cout << "\nEnter the word length: ";
        std::cin >> wordLength;

        if (wordLength >= 1)
            break;

        std::cout << "Please enter a positive word length.\n";
    }

    return wordLength;
}

// Get the target word from the user
std::string getTargetWord(int wordLength)
{
    std::string targetWord;
    while (true)
    {
        std::cout << "\nEnter the target word (" << wordLength << " letters): ";

        // Disable terminal input echo for privacy
        struct termios oldSettings, newSettings;
        tcgetattr(STDIN_FILENO, &oldSettings);
        newSettings = oldSettings;
        newSettings.c_lflag &= ~ECHO;
        tcsetattr(STDIN_FILENO, TCSANOW, &newSettings);

        std::cin >> targetWord;

        // Restore terminal settings
        tcsetattr(STDIN_FILENO, TCSANOW, &oldSettings);

        // Convert the word to uppercase
        std::transform(targetWord.begin(), targetWord.end(), targetWord.begin(), ::toupper);

        if (targetWord.length() == wordLength)
            break;

        std::cout << "Please enter a word with exactly " << wordLength << " letters.\n";
    }

    return targetWord;
}

// Get the user's guess for a round
std::string getGuess(int round, int wordLength)
{
    std::string guess;
    while (true)
    {
        std::cout << "\nRound " << round << " Guess (" << wordLength << " letters): ";
        std::cin >> guess;
        std::transform(guess.begin(), guess.end(), guess.begin(), ::toupper);

        if (guess.length() == wordLength)
            break;

        std::cout << "Please enter a word with exactly " << wordLength << " letters.\n";
    }

    return guess;
}

// Provide feedback on the user's guess
void provideFeedback(const std::string &targetWord, const std::string &guess)
{
    const int WORD_LENGTH = targetWord.length();
    int correctLetters = 0;

    for (int i = 0; i < WORD_LENGTH; i++)
    {
        if (guess[i] == targetWord[i])
        {
            std::cout << "Y ";
            correctLetters++;
        }
        else if (targetWord.find(guess[i]) != std::string::npos)
        {
            std::cout << "O ";
        }
        else
        {
            std::cout << "X ";
        }
    }

    std::cout << std::endl;
    std::cout << "Correct letters in guess: " << correctLetters << std::endl;
    std::cout << "Correct letters in total: " << correctLetters << std::endl;
}

// Check if the user has won the game
bool checkWin(const std::string &targetWord, const std::string &guess)
{
    return targetWord == guess;
}

// Hide the letters of a word
std::string hideWord(const std::string &word)
{
    std::string hiddenWord(word.length(), 'x');
    return hiddenWord;
}

// Get a random character from the available characters in the target word
std::string getRandomCharacter(const std::string &word, const std::string &exclude)
{
    std::string availableChars;
    for (char c : word)
    {
        if (exclude.find(c) == std::string::npos)
        {
            availableChars += c;
        }
    }

    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<int> distribution(0, availableChars.length() - 1);
    int randomIndex = distribution(generator);

    return std::string(1, availableChars[randomIndex]);
}

// Ask the user if they want to play again
bool playAgain()
{
    std::string answer;
    std::cout << "\nPlay Again? (Y/N): ";
    std::cin >> answer;
    std::cout << "\n";

    return (answer == "Y" || answer == "y");
}

// Start the Wordle game
void playWordle()
{
    const int MAX_ROUNDS = 6;

    while (true)
    {
        displayRules();
        int wordLength = getWordLength();

        int numRounds;
        if (wordLength < 4)
        {
            numRounds = wordLength;
        }
        else
        {
            numRounds = wordLength + 1;
        }

        std::string targetWord = getTargetWord(wordLength);

        std::string hiddenTargetWord = hideWord(targetWord);
        std::cout << "\nThe target word is: " << hiddenTargetWord << std::endl;

        int round = 0;
        int correctLetters = 0;
        std::string hintCharacter;

        while (round < numRounds && correctLetters < targetWord.length())
        {
            round++;
            std::string guess = getGuess(round, wordLength);
            provideFeedback(targetWord, guess);
            if (checkWin(targetWord, guess))
            {
                std::cout << "\nCongratulations! You guessed the word \"" << targetWord << "\" correctly!" << std::endl;
                return;  // Exit the function after displaying the message
            }
            else if (round == numRounds - 1 && wordLength >= 4)
            {
                hintCharacter = getRandomCharacter(targetWord, guess);
                std::cout << "Hint: One Letter Is:  \"" << hintCharacter << "\"." << std::endl;
            }
        }

        if (round >= numRounds || correctLetters >= targetWord.length())
        {
            std::cout << "\nSorry, you couldn't guess the word \"" << targetWord << "\" within " << numRounds << " rounds." << std::endl;
        }

        if (!playAgain())
        {
            break;
        }
    }

    std::cout << "\nThanks for playing!\n";
}

// The main function of the program
int main()
{
    playWordle();
    return 0;
}
