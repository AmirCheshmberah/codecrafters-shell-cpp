#include <iostream>
#include <vector>

std::vector<std::string> input_parser(std::string& input);

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

    std::cout << input << ": command not found" << std::endl;
  }

  return 0;
}

std::vector<std::string> input_parser(std::string& input)
{
  input += ' ';
  std::vector<std::string> parsedInput{};
  parsedInput.reserve(8);
  std::string word="";
  for(int i = 0; i < input.length(); i++)
  {
    if(input[i] != ' ')
    {
      word += input[i];
    }
    else
    {
      if (word != "")
        parsedInput.emplace_back(word);
      word = "";
    }
  }
  return parsedInput;
}
