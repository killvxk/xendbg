//
// Created by Spencer Michaels on 8/17/18.
//

#include <fcntl.h>
#include <sys/errno.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "BridgeHeaders/domctl.h"
#include "BridgeHeaders/privcmd.h"

#include "DomCtl.h"
#include "XenException.hpp"

using xd::xen::DomCtl;
using xd::xen::XenException;

DomCtl::DomCtl()
  : _privcmd_fd(open("/dev/xen/privcmd", O_RDWR))
{
  if (_privcmd_fd < 0)
    throw XenException("Failed to open privcmd interface: " + std::string(std::strerror(errno)));

  int flags = fcntl(_privcmd_fd, F_GETFD);
  if (flags < 0)
    throw XenException("Failed to get file handle flags: " + std::string(std::strerror(errno)));

  if (fcntl(_privcmd_fd, F_SETFD, flags | FD_CLOEXEC) < 0)
    throw XenException("Failed to set file handle flags: " + std::string(std::strerror(errno)));
}

DomCtl::~DomCtl() {
  close(_privcmd_fd);
}

void xd::xen::DomCtl::do_hypercall(xen_domctl& domctl, void *arg, int size) {
  privcmd_hypercall hypercall;
  hypercall.op = __HYPERVISOR_domctl;
  hypercall.arg[0] = (unsigned long)&domctl;

  if (arg && size && mlock(arg, size))
    throw XenException("Failed to pin domctl arg: " + std::string(std::strerror(errno)));

  if (ioctl(_privcmd_fd, IOCTL_PRIVCMD_HYPERCALL, &hypercall))
    throw XenException("Hypercall failed: " + std::string(std::strerror(errno)));

  if (arg && size)
    munlock(arg, size)
}
