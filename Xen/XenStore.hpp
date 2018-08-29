//
// Created by Spencer Michaels on 8/13/18.
//

#ifndef XENDBG_XENSTORE_HPP
#define XENDBG_XENSTORE_HPP

#include <memory>
#include <string>
#include <vector>

#include "BridgeHeaders/xenstore.h"

#include "Common.hpp"

namespace xd::xen {

  class XenStore {
  public:
    XenStore();

    std::string read(const std::string& file) const;
    std::vector<std::string> read_directory(const std::string& dir) const;
    DomID get_domid_from_name(const std::string& name) const;
    std::vector<DomID> get_guest_domids() const;

  private:
    std::unique_ptr<struct xs_handle, decltype(&xs_close)> _xenstore;
  };

}

#endif //XENDBG_XENSTORE_HPP
