#include <iostream>
#include <vector>
#include <cstdlib>
#include <string>
#include <filesystem>

std::vector<std::string> mySpliter(const std::string& input, const char& delim);
bool isContain(const std::string& longString, const std::string& shortString);
std::string builtin[3] = {"exit", "echo", "type"};
std::vector<std::string> directories{};
int main()
{
  while(true)
  {
    // Flush after every std::cout / std:cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;
    std::cout << "$ ";

    std::string input;
    std::getline(std::cin, input);

    std::vector<std::string> parsedInput = mySpliter(input, ' ');

    if(parsedInput[0] == "exit")
    {
      return stoi(parsedInput[1]);
    }

    else if(parsedInput[0] == "echo")
    {
      for(int i = 1; i < parsedInput.size(); i++)
      {
        std::cout << parsedInput[i];
        if(i < parsedInput.size()-1) std::cout << ' ';
      }
      std::cout << std::endl;
    }

    else if(parsedInput.size() == 2 && parsedInput[0] == "type")
    {
      bool isBuiltIn = false, isInPath = false;
      int builtinSize = sizeof(builtin)/sizeof(std::string);
      for(int i = 0; i < builtinSize; i++)
      {
        if (parsedInput[1] == builtin[i])
        {
          std::cout << parsedInput[1] <<" is a shell builtin" << '\n';
          isBuiltIn = true;
          break;
        }
      }
      if(!isBuiltIn) // PATH
      {
        std::string pathValue = getenv("PATH");
        std::string command = '/' + parsedInput[1];
        directories = mySpliter(pathValue, ':');
        for(int i = 0; i < directories.size(); i++)
        {
          std::string filePath = directories[i] + command;
          if(std::filesystem::exists(filePath))
          {
            std::cout << parsedInput[1] << " is " << filePath << '\n';
            isInPath = true;
            break;
          }
        }
      }
      if(!isBuiltIn && !isInPath)
        std::cout << parsedInput[1] << ": not found" << '\n';
    }
    else
    {
      bool isExec = false;
      for(int i = 0; i < directories.size(); i++)
      {
        std::string filePath = directories[i] + '/' + parsedInput[1];
        if(std::filesystem::exists(filePath))
        {
          system(filePath.c_str());
          isExec = true;
        }
      }
      if(!isExec)
        std::cout << input << ": command not found" << std::endl;
    }
  }

  return 0;
}

bool isContain(const std::string& longString, const std::string& shortString)
{
  if(shortString.length() > longString.length()) return false;
  for(int i = 0; i < longString.length();)
    {
      int j = 0;
      for(j = 0; j < shortString.length(); j++)
      {
        if(longString[i++] != shortString[j]) break;
      }
      if(j == shortString.length())
      {
        return true;
      }
    }
    return false;
}

std::vector<std::string> mySpliter(const std::string& input, const char& delim)
{
  std::vector<std::string> parsedInput{};
  parsedInput.reserve(8);
  std::string word=""; // empty string

  for(int i = 0; i < input.length(); i++)
  {
    if(input[i] != delim) // split
    {
      word += input[i];
      if(i == input.length()-1) // push last string
      {
        parsedInput.emplace_back(word);
      }
    }

    else
    {
      if (word != "") // make sure dont push any empty string
        parsedInput.emplace_back(word);
      word = "";
    }
  }
  return parsedInput;
}
