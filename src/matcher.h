#pragma once

#include <algorithm>
#include <cstdlib>
#include <functional>
#include <memory>
#include <tuple>
#include <vector>
#include "common_types.h"
#include "crash.h"

struct Rejector {
    u16 mask;
    u16 unexpected;
    bool Rejects(u16 instruction) const {
        return (instruction & mask) == unexpected;
    }
};

template <typename Visitor, typename Free = void (*)(void*)>
class PackagedMethod {
public:
    using visitor_type = Visitor;
    using handler_return_type = typename Visitor::instruction_return_type;

    template <typename F, typename... T>
    struct Package {
        F f;
        std::tuple<T...> ts;
    };

    template <typename Alloc, typename F, typename... T>
    PackagedMethod(Alloc alloc, Free free, F f, T... t)
        : package(alloc(alignof(Package<F, T...>), sizeof(Package<F, T...>)), free) {

        using PackageT = Package<F, T...>;

        invoker = [](Visitor& v, void* p_package) {
            PackageT& package = *reinterpret_cast<PackageT*>(p_package);
            return std::apply(std::mem_fn(package.f), std::tuple_cat(std::tie(v), package.ts));
        };

        new (package.get()) PackageT{f, std::make_tuple(t...)};
    }

    handler_return_type Invoke(Visitor& v) {
        return invoker(v, package.get());
    }

private:
    std::unique_ptr<void, Free> package;
    handler_return_type (*invoker)(Visitor&, void*);
};

template <typename Visitor>
class Matcher {
public:
    using visitor_type = Visitor;
    using handler_return_type = typename Visitor::instruction_return_type;
    using handler_function = std::function<handler_return_type(Visitor&, u16, u16)>;

    Matcher(const char* const name, u16 mask, u16 expected, bool expanded, handler_function func)
        : name{name}, mask{mask}, expected{expected}, expanded{expanded}, fn{std::move(func)} {}

    static Matcher AllMatcher(handler_function func) {
        return Matcher("*", 0, 0, false, std::move(func));
    }

    const char* GetName() const {
        return name;
    }

    bool NeedExpansion() const {
        return expanded;
    }

    bool Matches(u16 instruction) const {
        return (instruction & mask) == expected &&
               std::none_of(rejectors.begin(), rejectors.end(),
                            [instruction](const Rejector& rejector) {
                                return rejector.Rejects(instruction);
                            });
    }

    Matcher Except(Rejector rejector) const {
        Matcher new_matcher(*this);
        new_matcher.rejectors.push_back(rejector);
        return new_matcher;
    }

    handler_return_type call(Visitor& v, u16 instruction, u16 instruction_expansion = 0) const {
        ASSERT(Matches(instruction));
        return fn(v, instruction, instruction_expansion);
    }

private:
    const char* name;
    u16 mask;
    u16 expected;
    bool expanded;
    handler_function fn;
    std::vector<Rejector> rejectors;
};
