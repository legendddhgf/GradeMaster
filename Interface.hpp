#ifndef _GM_INTERFACE_H_
#define _GM_INTERFACE_H_

#include <iostream>

#include "lib/args/args.hxx"

int cli_main(int argc, char** argv);
void config_main(args::Subparser &parser);
void grade_main(args::Subparser &parser);

#endif // _GM_INTERFACE_H_
