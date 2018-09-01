//
// Created by Spencer Michaels on 8/12/18.
//

#include <iostream>
#include <string>

#include <readline/readline.h>

#include "REPL.hpp"
#include "Command/MakeCommand.hpp"
#include "../Util/IndentHelper.hpp"
#include "../Util/string.hpp"

using xd::repl::REPL;
using xd::repl::cmd::Argument;
using xd::repl::cmd::CommandVerb;
using xd::repl::cmd::Flag;
using xd::repl::cmd::Verb;
using xd::repl::cmd::make_command;
using xd::util::IndentHelper;
using xd::util::string::skip_whitespace;


REPL *REPL::_s_instance;
std::vector<std::string> REPL::_s_completion_options;

void REPL::run(REPL &repl) {
  init_readline();
  REPL::_s_instance = &repl;
  REPL::_s_instance->run(); // TODO: exception handling
  REPL::_s_instance = nullptr;
  deinit_readline();
}

void REPL::init_readline() {
  rl_attempted_completion_function = REPL::attempted_completion_function;
}

void REPL::deinit_readline() {
  rl_attempted_completion_function = nullptr;
}

char **REPL::attempted_completion_function(const char *text, int begin, int end) {
  rl_attempted_completion_over = 1;

  if (!_s_instance)
    return nullptr;

  /*
   * Because there are subcommands to complete, completion options depend on
   * the *entire* line (rl_line_buffer) rather than just the last word (text).
   */
  _s_completion_options = REPL::_s_instance->complete(rl_line_buffer);

  return rl_completion_matches(text, REPL::command_generator);
}

char *REPL::command_generator(const char *text, int state) {
  static size_t s_index, s_text_len;
  
  if (!state) {
    s_index = 0;
    s_text_len = strlen(text);
  }

  while (s_index < _s_completion_options.size()) {
    const auto &s = _s_completion_options.at(s_index++);
    if (strncmp(text, s.c_str(), s_text_len) == 0) {
      return strdup(s.c_str());
    }
  }

  return nullptr;
}

REPL::REPL()
  : _running(false), _prompt("> ")
{}

void REPL::add_command(REPL::CommandPtr cmd) {
  _commands.push_back(std::move(cmd));

  // Sorting every time a command is added is inefficient, but this is
  // irrelevant given that there will be only tens of commands at most.
  std::sort(_commands.begin(), _commands.end(),
    [](const auto& cmd1, const auto& cmd2) {
      return cmd1->get_name() < cmd2->get_name();
    });
}

void REPL::exit() {
  _running = false;
}

void REPL::run() {
  _running = true;
  while (_running) {
    if (_prompt_configurator)
      _prompt = _prompt_configurator();
    auto line = read_line();
    interpret_line(line);
  };
}

std::vector<std::string> REPL::complete(const std::string &s) {
  for (const auto &cmd : _commands) {
    auto options = cmd->complete(s.begin(), s.end());
    if (options.has_value()) {
      return options.value();
      }
  }

  std::vector<std::string> options;
  options.reserve(_commands.size());
  std::transform(_commands.begin(), _commands.end(), std::back_inserter(options),
    [](const auto& cmd) {
      return cmd->get_name();
    });
  return options;
}

void REPL::interpret_line(const std::string& line) {
  // Ignore empty lines, but print a newline so the prompt doesn't double up
  if (skip_whitespace(line.begin(), line.end()) == line.end()) {
    std::cout << std::endl;
    return;
  }

  for (const auto &cmd : _commands) {
    auto action = cmd->match(line.begin(), line.end());
    if (action) {
      (action.value())();
      return;
    }
  }

  if (_no_match_handler) {
    _no_match_handler(line);
    return;
  }

  std::cerr << "Invalid input." << std::endl;
}

void REPL::print_help(std::ostream &out) {
  IndentHelper indent;
  for (const auto& cmd : _commands) {
    cmd->print(out, indent);
  }
}

std::string REPL::read_line() {
  auto line = readline(_prompt.c_str());
  if (line)
    return std::string(line);
  return "";
}
