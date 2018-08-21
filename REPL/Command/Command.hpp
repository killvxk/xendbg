//
// Created by Spencer Michaels on 8/19/18.
//

#ifndef XENDBG_COMMAND_HPP
#define XENDBG_COMMAND_HPP

#include "CommandBase.hpp"
#include "Verb.hpp"

namespace xd::repl::cmd {

  class Command : public CommandBase {
  public:
    Command(std::string name, std::string description, std::vector<Verb> verbs)
        : CommandBase(std::move(name), std::move(description)),
          _verbs(std::move(verbs)) {};

    std::optional<Action> match(const std::string& s) const override;
    std::vector<std::string> complete(const std::string& s) const override;

    void add_verb(const Verb& verb) { _verbs.push_back(verb); }

  private:
    std::vector<Verb> _verbs;
  };

}

#endif //XENDBG_COMMAND_HPP
