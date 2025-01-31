#include <iostream>
#include <vector>
#include <array>
#include <cstdlib>
#include <string>
#include <filesystem>
#include <fstream>
#include <termios.h>
#include <unistd.h>

enum ValidCommands
{
  exitt,
  echo,
  type,
  pwd,
  cd,
  // cat,
  // ls,
  exe_file
};

ValidCommands isValid(std::string command)
{
  if(command == "exit") return ValidCommands::exitt;
  else if(command == "echo") return ValidCommands::echo;
  else if(command == "type") return ValidCommands::type;
  else if(command == "pwd") return ValidCommands::pwd;
  else if(command == "cd") return ValidCommands::cd;
  // else if(command == "cat") return ValidCommands::cat;
  // else if(command == "ls") return ValidCommands::ls;
  else return ValidCommands::exe_file;
}

std::vector<std::string> mySpliter(const std::string& input, const char& delim);
std::vector<std::string> fileNamesInDirectory(const std::string& directory);
std::vector<std::string> fileNamesInDirectory(const std::filesystem::path& directory);
std::string getClosestMatch(const std::string& input);
std::string doEcho(const std::string& input);
std::string getPath(const std::string& path);
bool isContain(const std::string& longString, const std::string& shortString);
void handle_cd();
void handle_ls();
void handle_cat();
void handle_type();
void modTerminal();
std::string inputWithAutoComplete();
void resetTerminalToOriginal();


std::array <std::string, 5> builtins = {"exit", "echo", "type", "pwd", "cd"};
std::vector<std::string> parsedInput;
std::vector<std::string> parsedPathValues;
termios orgTerm, modTerm;

int main()
{
  while(true)
  {
    // Flush after every std::cout / std:cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;
    std::cout << "$ ";

    std::string input = inputWithAutoComplete();
    // std::getline(std::cin, input);
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
        handle_type();
        break;
      }

      case pwd:
      {
        std::cout << std::filesystem::current_path().string() << std::endl;
        break;
      }

      case cd:
      {
        handle_cd();
        break;
      }

      // case ls:
      // {
      //   handle_ls();
      //   break;
      // }

      // case cat:
      // {
      //   handle_cat();
      //   break;
      // }
      case exe_file:
      {
        bool isExe = false;

        if(getPath(parsedInput[0]) != "")
        {
          std::system(input.c_str());
          isExe = true;
        }

        else if(parsedInput.size() > 1) // quoted executable
        {
          if(std::filesystem::exists(parsedInput[parsedInput.size()-1]))
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

std::string inputWithAutoComplete()
{
  std::vector<std::string> parsedInput;
  parsedInput.reserve(8);
  std::string input{};
  char singleChar{};
  size_t cursor_pos{};
  modTerminal();
  while(true)
  {
    singleChar = std::cin.get();
    if(singleChar == '\n')
    {
      std::cout << std::endl;
      break;
    }
    else if(singleChar == 65 || // up arrow
            singleChar == 66 || // down arrow
            singleChar == 67 || // right arrow
            singleChar == 68 )  // left arrow
    {
    }
    else if(singleChar == 127)
    {
      if(cursor_pos > 0)
      {
        std::cout << "\b";
        printf("\e[0K");
        cursor_pos--;
      }
      if(!input.empty())
      {
        input.pop_back();
      }
    }
    else if(singleChar == '\t')
    {
      parsedInput = mySpliter(input, ' ');
      std::string suggestion = getClosestMatch(parsedInput[0]);
      if(suggestion != "")
      {
        parsedInput[0] = suggestion;
        while(cursor_pos > 0)
        {
          std::cout << '\b';
          cursor_pos--;
        }
        input = "";
        for(auto i : parsedInput)
        {
          input += i + ' ';
          cursor_pos += i.length() + 1;
          std::cout << i + " ";
        }
      }
    }
    else
    {
      input += singleChar;
      std::cout << singleChar;
      cursor_pos++;
    }
  }
  return input;
}

void modTerminal()
{
  tcgetattr(STDIN_FILENO, &orgTerm);
  tcgetattr(STDIN_FILENO, &modTerm);
  modTerm.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &modTerm);
  atexit(resetTerminalToOriginal);
}

void resetTerminalToOriginal()
{
  tcsetattr(STDIN_FILENO, TCSANOW, &orgTerm);
}

std::string getClosestMatch(const std::string& input)
{
  int closestMatchIdx{-1};
  int longestMatch{};
  for(int i = 0; i < builtins.size(); i++)
  {
    int j;
    for(j = 0; j < builtins[i].length(); j++)
    {
      if(builtins[i][j] != input[j]) break;
    }
    if(longestMatch < j)
    {
        longestMatch = j;
        closestMatchIdx = i;
    }
  }
  if(closestMatchIdx == -1) return "";
  return builtins[closestMatchIdx];
}


void handle_type()
{
  bool isBuiltIn = false, isInPath = false;
  for(int i = 0; i < builtins.size(); i++)
  {
    if (parsedInput[1] == builtins[i])
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
    }
  }
  if(!isBuiltIn && !isInPath)
    std::cout << parsedInput[1] << ": not found" << '\n';
}

void handle_cd()
{
  if(parsedInput[0] == "cd")
  {
    if(parsedInput[1] == "~")
    {
      std::filesystem::current_path(getenv("HOME"));
    }
    else if(std::filesystem::exists(parsedInput[1]))
    {
      std::filesystem::current_path(parsedInput[1]);
    }
    else
    {
      std::cout << "cd: " << parsedInput[1] << ": No such file or directory\n";
    }
  }
}

void handle_cat()
{
  if(parsedInput[0] == "cat")
  {
    bool isRedirectOutput = false;
    std::string directTo = "";
    if(parsedInput[parsedInput.size()-2] == ">" || parsedInput[parsedInput.size()-2] == "1>")
    {
      isRedirectOutput = true;
      directTo = parsedInput[parsedInput.size()-1];
    }
    if(isRedirectOutput && directTo != "")
    {
      for(int i = 1; i < parsedInput.size()-2; i++)
      {
        if(std::filesystem::exists(parsedInput[i]))
        {
          std::string content{};
          std::fstream readFrom {parsedInput[i], std::ios::in};
          std::fstream writeTo {directTo, std::ios::out};
          while(getline(readFrom, content))
          {
            writeTo << content << '\n';
          }
        }
        else
        {
          std::cout << "cat: " << parsedInput[i] << ": No such file or directory" << std::endl;
        }
      }
      return;
    }
    else
    {
      for(int i = 1; i < parsedInput.size(); i++)
      {
        if(std::filesystem::exists(parsedInput[i]))
        {
          std::string content{};
          std::fstream readFrom {parsedInput[i], std::ios::in};
          while(getline(readFrom, content))
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
}

void handle_ls() // stinky implementation
{
  if(parsedInput[0] == "ls")
  {
    if(parsedInput.size() == 1) // e.g "ls"
    {
      for (const auto& fileName : fileNamesInDirectory(std::filesystem::current_path()))
        std::cout << fileName << ' ';
      std::cout << '\n';
    }

    else if(parsedInput.size() == 3 && parsedInput[1] == ">") // e.g "ls > {file}"
    {
      std::fstream file {parsedInput[2], std::ios::out};
      for(const auto& fileName : fileNamesInDirectory(std::filesystem::current_path()))
        file << fileName << '\n';
    }

    else if(parsedInput.size() == 4 && std::filesystem::exists(parsedInput[1]) && parsedInput[2] == ">") // e.g "ls {directory} > {file}"
    {
      std::fstream file {parsedInput[3], std::ios::out};
      for (const auto& fileName : fileNamesInDirectory(parsedInput[1]))
        file << fileName << '\n';
    }

    else if(parsedInput.size() == 5 && parsedInput[1] == "-1" && std::filesystem::exists(parsedInput[2])
            && parsedInput[3] == ">") // e.g "ls -1 {directory} > {file}"
    {
      std::fstream file {parsedInput[4], std::ios::out};
      for (const auto& fileName : fileNamesInDirectory(parsedInput[2]))
        file << fileName << '\n';
      file.close();
    }
  }
}

std::string doEcho(const std::string& input)
{
  std::vector<std::string> parsedEcho;
  parsedEcho.reserve(8);
  std::string result = "";
  std::string token = "";
  for(int i = 5; i < input.length();)
  {
    if(input[i] == '"')
    {
      while(input[++i] != '"')
      {
        if(input[i] == '\\')
        {
          token += input[++i];
          continue;
        }
        token += input[i];
      }
      if(input[i++] == '"') continue;
      parsedEcho.emplace_back(token);
      token = "";
      i++;
      continue;
    }

    if(input[i] == '\'')
    {
      while(input[++i] != '\'')
      {
        token += input[i];
      }
      if(input[i++] == '\'') continue;
      parsedEcho.emplace_back(token);
      token = "";
      i++;
      continue;
    }

    if(input[i] == '\\')
    {
      token += input[++i];
      i++;
      continue;
    }

    if(input[i] != ' ')
    {
      token += input[i++];
    }
    else
    {
      while(input[i] == ' ')
      {
        i++;
      }
      if(token != "")
        parsedEcho.emplace_back(token);
      token = "";
    }
  }

  if(token != "")
    parsedEcho.emplace_back(token);

  int i = 0;
  result = parsedEcho[i++];
  while (i < parsedEcho.size() && parsedEcho[i] != ">" && parsedEcho[i] != "1>"
         && parsedEcho[i] != "2>" && parsedEcho[i] != ">>" && parsedEcho[i] != "1>>"
         && parsedEcho[i] != "2>>")
  {
    result += ' ' + parsedEcho[i++];
  }

  if(parsedEcho[i] == ">" || parsedEcho[i] == "1>")
  {
    std::fstream file {parsedEcho[++i], std::ios::out};
    file << result << std::endl;
    return "";
  }

  if(parsedEcho[i] == "2>")
  {
    std::fstream file {parsedEcho[++i], std::ios::out};
  }

  if(parsedEcho[i] == ">>" || parsedEcho[i] == "1>>")
  {
    std::fstream file {parsedEcho[++i], std::ios::app};
    file << result << std::endl;
    return "";
  }

  if(parsedEcho[i] == "2>>")
  {
    std::fstream file {parsedEcho[++i], std::ios::app};
  }

  return result;
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
    try
    {
      if(std::filesystem::exists(result))
      {
        return result;
      }
    }
    catch(const std::exception& e)
    {
      // std::cerr << e.what() << '\n';
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