//
// Created by Spencer Michaels on 8/13/18.
//

#ifndef XENDBG_FOREIGNMEMORY_HPP
#define XENDBG_FOREIGNMEMORY_HPP

#include <memory>

#include <arch-x86_64.h> // TODO: remove?

#include "Common.hpp"

struct xenforeignmemory_handle;

namespace xd::xen {

  class Domain;
  class XenForeignMemory;

  class XenForeignMemory {
  public:
    using MappedMemory = std::shared_ptr<void>;

  public:
    XenForeignMemory();

    MappedMemory map(Domain& domain, Address address, size_t size, int prot);

  private:
    std::shared_ptr<xenforeignmemory_handle> _xen_foreign_memory;
  };

}

#endif //XENDBG_FOREIGNMEMORY_HPP