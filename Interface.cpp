#include "Interface.hpp"

int cli_main(int argc, char** argv) {

  args::ArgumentParser parser(
      "Mastering the art of grading.",
      "Project under development at http://github.com/legendddhgf/GradeMaster"
      );
  args::HelpFlag help(parser, "help", "display this help menu", {'h', "help"});

  args::Group commands(parser, "commands");
  args::Command config(commands, "config", "configure grading",
      &config_main);
  args::Command grade(commands, "grade", "grade submissions",
      &grade_main);

  // Execute parser
  try {
    parser.ParseCLI(argc, argv);
  } catch(args::Help) {
    std::cout << parser;
    return 0;
  } catch(args::ParseError &e) {
    std::cerr << e.what() << std::endl;
    std::cerr << parser;
    return 1;
  }
  return 0;
}

void config_main(args::Subparser &parser) {
  /*args::ValueFlag<std::string> view(parser,
      "view", "get the value of a config", {'v', "view"});
  args::ValueFlag<std::tuple<std::string,std::string>> edit(parser,
      "edit", "set the value of a config", {'e', "edit"});
  parser.Parse();

  if(view) {
    //std::cout << "Viewing " << args::get(view) << std::endl;
  }

  if(edit) {
    //std::cout << "Setting " << std::get<0>(args::get(edit)) << " to " <<
    //std::get<1>(args::get(edit) << std::endl;
  }*/

}

void grade_main(args::Subparser &parser) {
  parser.Parse();
}
