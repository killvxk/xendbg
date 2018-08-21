//
// Created by Spencer Michaels on 8/21/18.
//

#ifndef XENDBG_COMMANDBASE_HPP
#define XENDBG_COMMANDBASE_HPP

#include <optional>
#include <string>
#include <vector>

#include "Action.hpp"

namespace xd::repl::cmd {

  class CommandBase {
  public:
    CommandBase(std::string name, std::string description)
        : _name(std::move(name)), _description(std::move(description)) {};

    std::string get_name() const { return _name; };
    std::string get_description() const { return _description; };

    virtual std::optional<Action> match(const std::string &s) const = 0;
    virtual std::vector<std::string> complete(const std::string& s) const = 0;

  private:
    const std::string _name;
    const std::string _description;
  };

}

#endif //XENDBG_COMMANDBASE_HPP
