#include <iostream>
#include <string>
#include <vector>

// Finds the first character in a string that does not repeat.
char find_first_non_repeating(const std::string& s)
{
    if (s.empty())
    {
        return '\0'; // Return null character for empty string
    }

    // Step 1: Build a frequency map of all characters in the string.
    // Using a vector is a modern C++ equivalent of a C-style array.
    std::vector<int> counts(256, 0);
    for (char c : s)
    {
        // Use unsigned char to prevent issues with negative char values as array indices.
        counts[static_cast<unsigned char>(c)]++;
    }

    // Step 2: Iterate through the original string again to find the *first*
    // character with a frequency count of 1. This preserves the order.
    for (char c : s)
    {
        if (counts[static_cast<unsigned char>(c)] == 1)
        {
            return c; // Found it, return immediately.
        }
    }

    // If no non-repeating character is found, return a null character.
    return '\0';
}

int main()
{
    std::string s1("abdelrahman");

    char first_char = find_first_non_repeating(s1);

    if (first_char != '\0') {
        std::cout << "The first non-repeating character is: '" << first_char << "'" << std::endl;
    } else {
        std::cout << "No non-repeating character found." << std::endl;
    }

    return 0;
}