#include <iostream>
#include <vector>

std::vector<std::string> input_parser(const std::string& input);
std::string builtin[3] = {"exit", "echo", "type"};
int main()
{
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  // Uncomment this block to pass the first stage
  while(true)
  {
    std::cout << "$ ";

    std::string input;
    std::getline(std::cin, input);

    std::vector<std::string> parsedInput = input_parser(input);

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
      bool isBuiltIn = false;
      int builtinSize = sizeof(builtin)/sizeof(std::string);
      for(int i = 0; i < builtinSize; i++)
      {
        if (parsedInput[1] == builtin[i])
        {
          std::cout << parsedInput[1] <<" is a shell Builtin" << '\n';
          isBuiltIn = true;
        }
      }

      if(!isBuiltIn)
        std::cout << "invalid_command: not found" << '\n';
    }

    else
    {
      std::cout << input << ": command not found" << std::endl;
    }
  }

  return 0;
}

std::vector<std::string> input_parser(const std::string& input)
{
  std::vector<std::string> parsedInput{};
  parsedInput.reserve(8);
  std::string word=""; // empty string

  for(int i = 0; i < input.length(); i++)
  {
    if(input[i] != ' ') // find spaces
    {
      word += input[i];
      if(i == input.length()-1) // push last word
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
