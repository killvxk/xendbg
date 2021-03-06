//
// Copyright (C) 2018-2019 NCC Group
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
// the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

#ifndef XENDBG_REGISTER_CONTEXT_HPP
#define XENDBG_REGISTER_CONTEXT_HPP

#include <iostream>
#include <string>

namespace xd::reg {
  namespace {
    template <typename Context_t, typename Reg_t, bool matches>
    struct __offset_of_impl;

    template <typename Context_t, typename Reg_t>
    struct __offset_of_impl<Context_t, Reg_t, true> {
      static constexpr auto offset = Context_t::base;
    };

    template <typename Context_t, typename Reg_t>
    struct __offset_of_impl<Context_t, Reg_t, false> {
      static constexpr auto offset = Context_t::Next::template offset_of<Reg_t>;
    };

    template <typename Context_t, typename Reg_t>
    struct _offset_of_impl : public __offset_of_impl<
      Context_t, Reg_t, std::is_same<
        typename Context_t::Register,
        Reg_t>::value>
    {};

    template <typename Reg_t>
    struct _offset_of_error_impl {
      static_assert(std::is_same<Reg_t, void>::value, "No such register!");
    };
  }

  namespace {
    template <typename Context_t, typename Reg_t, bool matches>
    struct __get_impl;

    template <typename Context_t, typename Reg_t>
    struct __get_impl<Context_t, Reg_t, true> {
    public:
      explicit __get_impl(Context_t &context)
        : _context(context) {};

      const Reg_t &get() const {
        return _context._get();
      }
      Reg_t &get() {
        return _context._get();
      }

    private:
      Context_t &_context;
    };

    template <typename Context_t, typename Reg_t>
    struct __get_impl<Context_t, Reg_t, false> {
    public:
      explicit __get_impl(Context_t &context)
        : _context(context) {};

      const Reg_t &get() const {
        return _context.template _get_next<Reg_t>();
      }
      Reg_t &get() {
        return _context.template _get_next<Reg_t>();
      }

    private:
      Context_t &_context;
    };

    template <typename Context_t, typename Reg_t>
    struct _get_impl : public __get_impl<
      Context_t, Reg_t, std::is_same<
        typename std::remove_const<typename Context_t::Register>::type,
        Reg_t>::value>
    {
      using Base = __get_impl<
            Context_t, Reg_t, std::is_same<
              typename std::remove_const<typename Context_t::Register>::type,
              Reg_t>::value>;

      explicit _get_impl(Context_t &context)
        : Base(context) {};

      const Reg_t &get() const {
        return Base::get();
      }
      Reg_t &get() {
        return Base::get();
      }
    };

    template <typename Reg_t>
    struct _get_error_impl {
      static_assert(std::is_same<Reg_t, void>::value, "No such register!");
    };
  }

  template <size_t _id, size_t _base, typename... Registers_t>
  class _RegisterContext;

  template <size_t _id, size_t _base>
  class _RegisterContext<_id, _base> {
  public:
    static constexpr auto base = _base;

    template <typename F>
    static void for_each_metadata(F) {};

    template <typename F>
    void for_each(F) {};

    template <typename F>
    void for_each(F) const {};

    template <typename Reg_t>
    const Reg_t &get() const {
      _get_error_impl<Reg_t> x;
    }
    template <typename Reg_t>
    Reg_t &get() {
      _get_error_impl<Reg_t> x;
    }

    static constexpr size_t size = 0;

    template <typename Reg_t>
    static constexpr size_t offset_of =
      _offset_of_error_impl<Reg_t>::offset;

    static bool is_valid_id(size_t) {
      return false;
    }

    template <typename MatchFn, typename FoundFn, typename NotFoundFn>
    void find(MatchFn m, FoundFn ff, NotFoundFn nff) {
      nff();
    }

    template <typename MatchFn, typename FoundFn, typename NotFoundFn>
    void find(MatchFn m, FoundFn ff, NotFoundFn nff) const {
      nff();
    }

    template <typename FoundFn, typename NotFoundFn>
    static void find_metadata_by_id(size_t, FoundFn, NotFoundFn nff) {
      nff();
    }
  };

  template <size_t _id, size_t _base, typename Register_t, typename... Registers_t>
  class _RegisterContext<_id, _base, Register_t, Registers_t...>
    : public _RegisterContext<
        _id+1, _base+sizeof(typename Register_t::Value),
        Registers_t...>
  {
  protected:
    template <typename Context_t, typename Reg_t>
    friend struct _offset_of_impl;
    template <typename Context_t, typename Reg_t, bool matches>
    friend struct __offset_of_impl;
    template <typename Context_t, typename Reg_t>
    friend struct _get_impl;
    template <typename Context_t, typename Reg_t, bool matches>
    friend struct __get_impl;

    using This = _RegisterContext<_id, _base, Register_t, Registers_t...>;
    using Next = _RegisterContext<
          _id+1, _base+sizeof(typename Register_t::Value),
          Registers_t...>;

    const Register_t &_get() const {
      return _register;
    }
    Register_t &_get() {
      return _register;
    }

    template <typename Reg_t>
    const Reg_t &_get_next() const {
      return Next::template get<Reg_t>();
    }
    template <typename Reg_t>
    Reg_t &_get_next() {
      return Next::template get<Reg_t>();
    }

  public:
    using Register = Register_t;

    _RegisterContext() {
      _register.clear();
    }

    static constexpr auto id = _id;
    static constexpr auto base = _base;
    static constexpr size_t size = sizeof(typename Register_t::Value) + Next::size;

    template <typename Reg_t>
    static constexpr size_t offset_of =
      _offset_of_impl<This, Reg_t>::offset;

    template <typename Reg_t>
    struct RegisterMetadataReference {
      using Register = Reg_t;

      const size_t &id;
      const size_t &width;
      const size_t &offset;
      const decltype(Register_t::name) &name;
      const decltype(Register_t::alt_name) &alt_name;
      const size_t &gcc_id;
    };

    template <typename Reg_t>
    static constexpr RegisterMetadataReference<Reg_t> metadata_of = {
      id,
      sizeof(typename Reg_t::Value),
      offset_of<Reg_t>,
      Reg_t::name,
      Reg_t::alt_name,
      Reg_t::gcc_id
    };

    template <typename Reg_t>
    const Reg_t &get() const {
      const _get_impl<const This, Reg_t> getter(*this);
      return getter.get();
    };
    template <typename Reg_t>
    Reg_t &get() {
      return _get_impl<This, Reg_t>(*this).get();
    };

    template <typename F>
    void for_each(F f) const {
      f(metadata_of<Register_t>, _register);
      Next::template for_each(f);
    };

    template <typename F>
    void for_each(F f) {
      f(metadata_of<Register_t>, _register);
      Next::template for_each(f);
    };

    template <typename F>
    static void for_each_metadata(F f) {
      f(metadata_of<Register_t>);
      Next::template for_each_metadata(f);
    };

    static bool is_valid_id(size_t target_id) {
      return (id == target_id)
        ? true
        : Next::is_valid_id(target_id-1);
    }

    template <typename MatchFn, typename FoundFn, typename NotFoundFn>
    void find(MatchFn m, FoundFn ff, NotFoundFn nff) const {
      if (m(metadata_of<Register_t>))
        ff(metadata_of<Register_t>, _register);
      else
        Next::find(m, ff, nff);
    }

    template <typename MatchFn, typename FoundFn, typename NotFoundFn>
    void find(MatchFn m, FoundFn ff, NotFoundFn nff) {
      if (m(metadata_of<Register_t>))
        ff(metadata_of<Register_t>, _register);
      else
        Next::find(m, ff, nff);
    }

    template <typename FoundFn, typename NotFoundFn>
    void find_by_id(size_t id, FoundFn ff, NotFoundFn nff) const {
      find([&](const auto &md) {
        return md.id == id;
      }, ff, nff);
    }

    template <typename FoundFn, typename NotFoundFn>
    void find_by_id(size_t id, FoundFn ff, NotFoundFn nff) {
      find([&](const auto &md) {
        return md.id == id;
      }, ff, nff);
    }

    template <typename FoundFn, typename NotFoundFn>
    static void find_metadata_by_id(size_t id, FoundFn ff, NotFoundFn nff) {
      if (id == 0)
        ff(metadata_of<Register_t>);
      else
        Next::find_metadata_by_id(id-1, ff, nff);
    }

    void clear() {
      for_each([](const auto&, auto &reg) {
        reg.clear();
      });
    }

  private:
    Register_t _register;
  };

  template <typename... Registers_t>
  using RegisterContext = _RegisterContext<0, 0, Registers_t...>;

}

#endif //XENDBG_REGISTER_CONTEXT_HPP
