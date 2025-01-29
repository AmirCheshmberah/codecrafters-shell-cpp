#include <iostream>
#include <vector>
#include <cstdlib>
#include <string>
#include <filesystem>
#include <fstream>

enum ValidCommands
{
  exitt,
  echo,
  type,
  cat,
  ls,
  exe_file
};

ValidCommands isValid(std::string command)
{
  if(command == "exit") return ValidCommands::exitt;
  else if(command == "echo") return ValidCommands::echo;
  else if(command == "type") return ValidCommands::type;
  else if(command == "cat") return ValidCommands::cat;
  else if(command == "ls") return ValidCommands::ls;
  else return ValidCommands::exe_file;
}

std::vector<std::string> mySpliter(const std::string& input, const char& delim);
std::vector<std::string> echoParser (const std::string& input);
std::vector<std::string> fileNamesInDirectory(const std::string& directory);
std::vector<std::string> fileNamesInDirectory(const std::filesystem::path& directory);
std::string doEcho(const std::string& input);
std::string getPath(const std::string& path);
bool isContain(const std::string& longString, const std::string& shortString);
void handle_ls();
void handle_cat();

std::string builtin[3] = {"exit", "echo", "type"};

std::vector<std::string> parsedInput;
std::vector<std::string> parsedPathValues;

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

    parsedInput = mySpliter(input, ' ');
    parsedPathValues = mySpliter(getenv("PATH"), ':');

    switch (isValid(parsedInput[0]))
    {
      case exitt:
      {
        return stoi(parsedInput[1]);
        break;
      }

      case echo:
      {
        std::string _doEcho = doEcho(input);
        if(_doEcho != "")
        {
          std::cout << _doEcho << std::endl;
        }
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
          std::string filePath = getPath(parsedInput[1]);
          if(filePath != "")
          {
            std::cout << parsedInput[1] << " is " << filePath << '\n';
            isInPath = true;
            break;
          }
        }
        if(!isBuiltIn && !isInPath)
          std::cout << parsedInput[1] << ": not found" << '\n';
        break;
      }

      // case ls:
      // {
      //   handle_ls();
      //   break;
      // }

      case cat:
      {
        handle_cat();
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
        if(getPath(parsedInput[0]) != "")
        {
          std::system(input.c_str());
          isExe = true;
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

void handle_cat()
{
  if(parsedInput[0] == "cat")
  {
    for(int i = 1; i < parsedInput.size(); i++)
    {
      if(std::filesystem::exists(parsedInput[i]))
      {
        std::string content{};
        std::fstream f {parsedInput[i], std::ios::in};
        while(getline(f, content))
        {
          std::cout << content << '\n';
        }
      }
      else
      {
        std::cout << "cat: " << parsedInput[i] << ": No such file or directory" << std::endl;
      }
    }
  }
}

void handle_ls()
{
  if(parsedInput[0] == "ls") // e.g "ls"
  {
    if(parsedInput.size() == 1)
    {
      for (auto& fileName : fileNamesInDirectory(std::filesystem::current_path()))
        std::cout << fileName << ' ';
      std::cout << '\n';
    }

    else if(parsedInput.size() == 3 && parsedInput[1] == ">") // e.g "ls > {file}"
    {
      std::fstream file {parsedInput[2], std::ios::out};
      for(auto& fileName : fileNamesInDirectory(std::filesystem::current_path()))
        file << fileName << '\n';
    }

    else if(parsedInput.size() == 4 && std::filesystem::exists(parsedInput[1]) && parsedInput[2] == ">") // e.g "ls {directory} > {file}"
    {
      int i{};
      while(parsedInput[1][i] == '/')
        i++;
      parsedInput[1] = parsedInput[1].substr(i, parsedInput[1].length());

      std::fstream file {parsedInput[3], std::ios::out};
      for (auto& fileName : fileNamesInDirectory(parsedInput[1]))
        file << fileName << '\n';
    }
  }
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

    if(input[i] == '>')
    {
      if(result.back() == ' ')
      {
        result = result.substr(0, result.length()-1);
      }
      std::string fileName{};
      while(++i < input.length())
      {
        if(input[i] != ' ')
        {
          fileName += input[i];
        }
      }
      std::fstream file {fileName, std::ios::out};
      file << result << std::endl;
      return "";
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

std::string getPath(const std::string& pathFile)
{
  std::stringstream streamPathValue {getenv("PATH")};
  std::string pathValue{};
  while(getline(streamPathValue, pathValue, ':'))
  {
    std::string result = pathValue + '/' + pathFile;
    if(std::filesystem::exists(result))
    {
      return result;
    }
  }
  return "";
  // std::string result;
  // for(int i = 0; i < parsedPathValues.size(); i++)
  // {
  //   result = parsedPathValues[i] + '/' + path;
  //   if(std::filesystem::exists(result))
  //   {
  //     return result;
  //   }
  // }
  // return "";
}

std::vector<std::string> fileNamesInDirectory(const std::string& directory)
{
  std::vector<std::string> fileNames;
  fileNames.reserve(8);
  for(auto &entry : std::filesystem::directory_iterator(directory))
  {
    fileNames.emplace_back(entry.path().filename().generic_string());
  }
  return fileNames;
}

std::vector<std::string> fileNamesInDirectory(const std::filesystem::path& directory)
{
  std::vector<std::string> fileNames;
  fileNames.reserve(8);
  for(auto &entry : std::filesystem::directory_iterator(directory))
  {
    fileNames.emplace_back(entry.path().filename().generic_string());
  }
  return fileNames;
}