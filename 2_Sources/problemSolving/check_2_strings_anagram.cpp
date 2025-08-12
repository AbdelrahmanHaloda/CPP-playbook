#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

// check if 2 strings are anagram

// Solution 1: Optimal Approach using a fixed-size array (std::vector).
// This has O(n) time and O(1) space complexity because the array size is constant.
// It's highly efficient for character sets like ASCII.
bool are_anagrams_vector(const std::string& s1, const std::string& s2)
{
    // Anagrams must have the same length.
    if (s1.length() != s2.length())
    {
        return false;
    }

    // Create a frequency map for all 256 possible ASCII characters.
    // Initialize all counts to 0.
    std::vector<int> counts(256, 0);

    // Increment count for each character in the first string.
    // Using unsigned char for the index prevents issues with negative char values.
    for (char c : s1)
    {
        counts[static_cast<unsigned char>(c)]++;
    }
    // Decrement count for each character in the second string.
    for (char c : s2)
    {
        counts[static_cast<unsigned char>(c)]--;
    }
    // If the strings are anagrams, all counts must be zero.
    for (int count : counts)
    {
        if (count != 0)
        {
            return false;
        }
    }
    return true;
}

// Solution 2: Original approach using a hash map (std::unordered_map).
// This also has O(n) average time complexity but may have slightly more
// overhead due to dynamic memory allocation and hashing. It's more flexible
// for larger character sets (e.g., Unicode).
bool are_anagrams_map(const std::string& s1, const std::string& s2)
{
    if (s1.length() != s2.length())
    {
        return false;
    }

    std::unordered_map<char, int> counts;

    // Increment count for each character in the first string.
    for (char c : s1)
    {
        counts[c]++;
    }
    // Decrement count for each character in the second string.
    for (char c : s2)
    {
        counts[c]--;
    }
    // If the strings are anagrams, all counts must be zero.
    for (const auto& pair : counts)
    {
        if (pair.second != 0)
        {
            return false;
        }
    }
    return true;
}

int main()
{
    std::string s1("listen");
    std::string s2("silent");

    std::cout << "Checking with vector-based solution: " << (are_anagrams_vector(s1, s2) ? "Is an anagram." : "Is not an anagram.") << std::endl;
    std::cout << "Checking with map-based solution:    " << (are_anagrams_map(s1, s2) ? "Is an anagram." : "Is not an anagram.") << std::endl;

    return 0;
}
