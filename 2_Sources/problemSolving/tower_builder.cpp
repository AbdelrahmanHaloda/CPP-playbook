// Online C++ compiler to run C++ program online
#include <iostream>
#include <string>
#include <vector>

using namespace std;

std::vector<std::string> towerBuilder(unsigned nFloors) 
{
  std::vector<std::string> list{nFloors};
  
  for(size_t i = 0; i < nFloors; i++)
  {
    for(size_t k = 0; k < nFloors - i-1; k++)
    {
      list[i].push_back('#');
    }
    for(size_t j = 0; j < (i * 2) + 1; j++)
    {
      list[i].push_back('*');
    } 
  }
  
  return list;
}

int main() {
    
    std::vector<std::string> tower = towerBuilder(3);

    for(const auto& floor : tower)
    {
        std::cout << floor << std::endl;
    }

    return 0;
}