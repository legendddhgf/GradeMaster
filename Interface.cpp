#include "Interface.hpp"

int cli_main(int argc, char** argv) {

  args::ArgumentParser parser(
      "Mastering the art of grading.",
      "Project under development at http://github.com/legendddhgf/GradeMaster"
      );
  args::HelpFlag help(parser, "help", "display this help menu", {'h', "help"});

  // Execute parser
  try {
    parser.ParseCLI(argc, argv);
  } catch(args::Help) {
    std::cout << parser;
    return 0;
  } catch(args::ParseError& e) {
    std::cerr << e.what() << std::endl;
    std::cerr << parser;
    return 1;
  }
  return 0;
}
