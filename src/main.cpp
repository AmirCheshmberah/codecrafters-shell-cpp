#include <iostream>
#include <vector>
#include <cstdlib>
#include <string>
#include <filesystem>

enum ValidCommands
{
  _exit,
  echo,
  type,
  exe_file
};

ValidCommands isValid(std::string command)
{
  if(command == "exit") return ValidCommands::_exit;
  else if(command == "echo") return ValidCommands::echo;
  else if(command == "type") return ValidCommands::type;
  else return ValidCommands::exe_file;
}

std::vector<std::string> mySpliter(const std::string& input, const char& delim);
bool isContain(const std::string& longString, const std::string& shortString);
std::string builtin[3] = {"exit", "echo", "type"};

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
    std::vector<std::string> parsedPathValue = mySpliter(getenv("PATH"), ':');

    switch (isValid(parsedInput[0]))
    {
      case _exit:
      {
        return stoi(parsedInput[1]);
        break;
      }

      case echo:
      { 
        char* cur = &input[5];
        while(*cur != '\'') cur++;
        if(*cur == '\'')
        {
          while(*(++cur) != '\'')
            std::cout << *cur;
          std::cout << '\n';
          continue;
        }
        for(int i = 1; i < parsedInput.size(); i++)
        {
          std::cout << parsedInput[i];
          if(i < parsedInput.size()-1) std::cout << ' ';
        }
        std::cout << std::endl;
        break;
      }

      case type:
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
          for(int i = 0; i < parsedPathValue.size(); i++)
          {
            std::string filePath = parsedPathValue[i] + '/' + parsedInput[1];
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
        break;
        }

      case exe_file:
      {
        bool isExe = false;
        for(int i = 0; i < parsedPathValue.size(); i++)
        {
          std::string filePath = parsedPathValue[i] + '/' + parsedInput[0];
          if(std::filesystem::exists(filePath))
          {
            std::system(input.c_str());
            isExe = true;
          }
        }
        if(!isExe)
          std::cout << input << ": command not found" << std::endl;
        break;
      }
      default:
      {
        std::cout << input << ": command not found" << std::endl;
      }
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
