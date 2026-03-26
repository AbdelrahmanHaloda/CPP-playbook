#include <iostream>
#include <string>
#include<unordered_map>
using namespace std;

string SearchingChallenge(string str) {
  
  // code goes here  

  int stringSize = str.size();
  unordered_map<string, int> freqMap;
  string longestString {};
  string returnPrefix {};
  string result {};

  
  for (int i = 0; i < stringSize; i++)
  {
    for (int len = 2; len + i <= stringSize; len++)
    {
      string temp = str.substr(i,len);
      freqMap[temp] ++;
    }
  }

  for(auto& pair : freqMap)
  {
    string temp = pair.first;
    int count = pair.second;

    if (count >=2 && temp.size())
    {
      longestString = temp;
    }
  }

  if(longestString.empty())
  {
    returnPrefix = "no null ";
  }
  else
  {
    returnPrefix = "yes ";
  }

  result = returnPrefix + longestString;

  return result;

}

// keep this function call here
int main(void) { 
   
  cout << SearchingChallenge(coderbyteInternalStdinFunction(stdin));
  return 0;
    
}