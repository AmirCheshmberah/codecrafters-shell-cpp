#include <iostream>
#include <vector>
#include <cstdlib>
#include <string>
#include <filesystem>

enum ValidCommands
{
  exitt,
  echo,
  type,
  cat,
  exe_file
};

ValidCommands isValid(std::string command)
{
  if(command == "exit") return ValidCommands::exitt;
  else if(command == "echo") return ValidCommands::echo;
  else if(command == "type") return ValidCommands::type;
  else if(command == "cat") return ValidCommands::cat;
  else return ValidCommands::exe_file;
}

std::vector<std::string> mySpliter(const std::string& input, const char& delim);
std::vector<std::string> echoParser (const std::string& input);
std::string doEcho(const std::string& input);
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
      case exitt:
      {
        return stoi(parsedInput[1]);
        break;
      }

      case echo:
      {
        std::cout << doEcho(input) << std::endl;
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
      case cat:
      {
        std::system(input.c_str());
        break;
      }
      case exe_file:
      {
        bool isExe = false;
        if(parsedInput.size() > 1) // quoted executable
        {
          if(std::filesystem::exists(parsedInput[parsedInput.size()-1]))
          {
            std::system(input.c_str());
            isExe = true;
          }
        }
        for(int i = 0; i < parsedPathValue.size(); i++) // PATH
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
        break;
      }
    }
  }

  return 0;
}

std::string doEcho(const std::string& input)
{
  std::string result = "";
  for(int i = 5; i < input.length();)
  {
    if(input[i] == '"')
    {
      while(input[++i] != '"')
      {
        if(input[i] == '\\')
        {
          result += input[++i];
          continue;
        }
        result += input[i];
      }
      i++;
      continue;
    }

    if(input[i] == '\'')
    {
      while(input[++i] != '\'')
      {
        result += input[i];
      }
      i++;
      continue;
    }

    if(input[i] == '\\')
    {
      result += input[++i];
      i++;
      continue;
    }

    if(input[i] != ' ')
    {
      result += input[i++];
    }
    else
    {
      while(input[i] == ' ')
      {
        i++;
      }
      if(result != "")
        result += ' ';
    }
  }
  return result;
}

std::vector<std::string> echoParser(const std::string& input)
{
  std::vector<std::string> parsedEcho{};
  std::string word = "";
  for(int i = 5; i < input.length(); i++)
  {
    if(input[i] == '"')
    {
      while(input[++i] != '"')
      {
        if(input[i] == '\\')
        {
          word += input[++i];
          continue;
        }
        word += input[i];
      }
      continue;
    }

    if(input[i] == '\'')
    {
      while(input[++i] != '\'')
      {
        word += input[i];
      }
      continue;
    }

    if(input[i] == '\\')
    {
      word += input[++i];
      i++;
      continue;
    }

    if(input[i] == ' ')
    {
      if(word != "")
        parsedEcho.emplace_back(word);
      word = "";
    }
    else
    {
      word += input[i];
    }
  }

  if(word != "")
    parsedEcho.emplace_back(word);
  return parsedEcho;
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
