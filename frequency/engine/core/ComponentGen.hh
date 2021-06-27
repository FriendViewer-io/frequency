#pragma once

#include <stdint.h>

#include <tuple>
#include <type_traits>
#include <utility>

#include "engine/core/Component.hh"

namespace ch_detail {
template <typename... Ts>
struct pointer_tuple : std::tuple<Ts*...> {};
template <typename... Ts>
struct const_pointer_tuple : pointer_tuple<const Ts...> {};

template <typename T>
size_t find_single_type(std::vector<Component*> const& comp_ls) {
   for (size_t i = 0; i < comp_ls.size(); i++) {
      if (comp_ls[i]->hierarchy_check(T::component_typename())) {
         return i;
      }
   }
   return static_cast<size_t>(-1);
}

template <typename T>
size_t find_single_type(std::vector<Component const*> const& comp_ls) {
   for (size_t i = 0; i < comp_ls.size(); i++) {
      if (comp_ls[i]->hierarchy_check(T::component_typename())) {
         return i;
      }
   }
   return static_cast<size_t>(-1);
}

// Base case
template <size_t i, typename... Ts>
std::enable_if_t<i >= sizeof...(Ts), bool> assign_all_types(pointer_tuple<Ts...>& deps,
                                                            std::vector<Component*>& comp_ls) {
   return true;
}

// Recursive case
template <size_t i, typename... Ts>
    std::enable_if_t < i<sizeof...(Ts), bool> assign_all_types(pointer_tuple<Ts...>& deps,
                                                               std::vector<Component*>& comp_ls) {
   using i_type = typename std::tuple_element_t<i, std::tuple<Ts...>>;
   const size_t idx = find_single_type<i_type>(comp_ls);
   if (idx == static_cast<size_t>(-1)) {
      return false;
   }
   std::get<i>(deps) = static_cast<i_type*>(comp_ls[idx]);
   return assign_all_types<i + 1, Ts...>(deps, comp_ls);
}

// Base case
template <size_t i, typename... Ts>
std::enable_if_t<i >= sizeof...(Ts), bool> assign_all_types(
    const_pointer_tuple<Ts...>& deps, std::vector<Component const*>& comp_ls) {
   return true;
}

// Recursive case
template <size_t i, typename... Ts>
    std::enable_if_t <
    i<sizeof...(Ts), bool> assign_all_types(const_pointer_tuple<Ts...>& deps,
                                            std::vector<Component const*>& comp_ls) {
   using i_type = typename std::tuple_element_t<i, std::tuple<Ts...>>;
   const size_t idx = find_single_type<i_type>(comp_ls);
   if (idx == static_cast<size_t>(-1)) {
      return false;
   }
   std::get<i>(deps) = static_cast<i_type const*>(comp_ls[idx]);
   return assign_all_types<i + 1, Ts...>(deps, comp_ls);
}

template <typename Tt, size_t i, typename... Ts>
constexpr std::enable_if_t<i >= sizeof...(Ts), Tt*> get_by_component_typename(
    pointer_tuple<Ts...>& tup) {
   static_assert(false, "Type Tt not found in dependency list");
   return nullptr;
}

template <typename Tt, size_t i, typename... Ts>
    constexpr std::enable_if_t <
    i<sizeof...(Ts), Tt*> get_by_component_typename(pointer_tuple<Ts...>& tup) {
   using Ts_i = typename std::tuple_element_t<i, std::tuple<Ts...>>;
   if constexpr (Tt::component_typename() == Ts_i::component_typename()) {
      return std::get<i>(tup);
   } else {
      return get_by_component_typename<Tt, i + 1, Ts...>(tup);
   }
}

template <typename Tt, size_t i, typename... Ts>
constexpr std::enable_if_t<i >= sizeof...(Ts), Tt const*> get_by_component_typename(
    pointer_tuple<Ts...> const& tup) {
   static_assert(false, "Type Tt not found in dependency list");
   return nullptr;
}

template <typename Tt, size_t i, typename... Ts>
    constexpr std::enable_if_t <
    i<sizeof...(Ts), Tt const*> get_by_component_typename(pointer_tuple<Ts...> const& tup) {
   using Ts_i = typename std::tuple_element_t<i, std::tuple<Ts...>>;
   if constexpr (Tt::component_typename() == Ts_i::component_typename()) {
      return std::get<i>(tup);
   } else {
      return get_by_component_typename<Tt, i + 1, Ts...>(tup);
   }
}

template <typename... Ts>
struct is_all_component {
   static inline constexpr bool value = std::conjunction_v<std::is_base_of<Component, Ts>...>;
};

template <typename... Ts>
constexpr bool is_ptuple_all_component(pointer_tuple<Ts...>) {
   return is_all_component<Ts...>::value;
}
}  // namespace ch_detail

// Defines boilerplate for a derived component class. Place at the start of your component class.
// Use this when your component has no dependencies, it should reduce clutter.
// Arguments:
//            name   -> Full name of component class
//            parent -> Full name of base component class
#define DEFINE_COMPONENT_INLINE_NODEP(name, parent)                                                \
public:                                                                                            \
   using base_type = parent;                                                                       \
   using tuple_type = ch_detail::pointer_tuple<>;                                                  \
   constexpr static std::string_view component_typename() { return #name; }                        \
   std::string_view get_exact_typename() const override { return name::component_typename(); }     \
   virtual bool hierarchy_check(std::string_view sv) const {                                       \
      return (sv == name::component_typename()) || base_type::hierarchy_check(sv);                 \
   }                                                                                               \
                                                                                                   \
public:                                                                                            \
   bool dep_assign(std::vector<Component*>& comp_ls, std::vector<Component const*>& comp_ls_const) \
       override {                                                                                  \
      return base_type::dep_assign(comp_ls, comp_ls_const);                                        \
   }

// Defines and declares boilerplate for a derived component class. Place at the start of your
// component class. Use this when your component depends on others.
// Arguments:
//            name    -> Full name of component type
//            parent  -> Full name of base component type
//            varargs -> Full names of components this depends
#define DEFINE_COMPONENT_CLASS_CHUNK(name, parent, ...)                                            \
public:                                                                                            \
   using base_type = parent;                                                                       \
   using tuple_type = ch_detail::pointer_tuple<__VA_ARGS__>;                                       \
   constexpr static std::string_view component_typename() { return #name; }                        \
   std::string_view get_exact_typename() const override { return name::component_typename(); }     \
   virtual bool hierarchy_check(std::string_view sv) const {                                       \
      return (sv == name::component_typename()) || base_type::hierarchy_check(sv);                 \
   }                                                                                               \
                                                                                                   \
private:                                                                                           \
   ch_detail::pointer_tuple<__VA_ARGS__> deps;                                                     \
   ch_detail::const_pointer_tuple<__VA_ARGS__> old_deps;                                           \
   friend class name##_get;                                                                        \
                                                                                                   \
public:                                                                                            \
   bool dep_assign(std::vector<Component*>& comp_ls, std::vector<Component const*>& comp_ls_const) \
       override;

// Defines remaining boilerplate for a derived component class with dependencies. Place this in
// global scope after all other dependencies are defined (e.g. associated .cc file)
// Arguments:
//            name -> Full name of component type
#define DEFINE_COMPONENT_GLOBAL_CHUNK(name)                                                       \
   static_assert(is_ptuple_all_component(name::tuple_type{}),                                     \
                 "DEFINE_COMPONENT_CLASS_CHUNK and DEFINE_COMPONENT_GLOBAL_CHUNK expected the "   \
                 "dependency list "                                                               \
                 "to be all value-type classes derived from Component. Ensure "                   \
                 "DEFINE_COMPONENT_GLOBAL_CHUNK is invoked after all dependencies are declared"); \
   bool name::dep_assign(std::vector<Component*>& comp_ls,                                        \
                         std::vector<Component const*>& comp_ls_const) {                          \
      return base_type::dep_assign(comp_ls, comp_ls_const) &&                                     \
             ch_detail::assign_all_types<0>(deps, comp_ls) &&                                     \
             ch_detail::assign_all_types<0>(old_deps, comp_ls_const);                             \
   }                                                                                              \
   class name##_get : public name {                                                               \
   public:                                                                                        \
      template <typename T>                                                                       \
      constexpr T* _get() {                                                                       \
         static_assert(std::is_base_of_v<Component, T>,                                           \
                       "_get<T>() Expected value-type derived from Component");                   \
         return ch_detail::get_by_component_typename<T, 0>(deps);                                 \
      }                                                                                           \
      template <typename T>                                                                       \
      constexpr T const* _get() const {                                                           \
         static_assert(std::is_base_of_v<Component, T>,                                           \
                       "_get<T>() Expected value-type derived from Component");                   \
         return ch_detail::get_by_component_typename<T, 0>(deps);                                 \
      }                                                                                           \
      template <typename T>                                                                       \
      constexpr T const* _get_old() const {                                                       \
         static_assert(std::is_base_of_v<Component, T>,                                           \
                       "_get_old<T>() Expected value-type derived from Component");               \
         return ch_detail::get_by_component_typename<T, 0>(old_deps);                             \
      }                                                                                           \
   };                                                                                             \
   namespace {                                                                                    \
   template <typename T>                                                                          \
   constexpr T* get(name* inst) {                                                                 \
      static_assert(std::is_base_of_v<Component, T>,                                              \
                    "get<T>() Expected value-type derived from Component");                       \
      return static_cast<name##_get*>(inst)->_get<T>();                                           \
   }                                                                                              \
   template <typename T>                                                                          \
   constexpr T const* get(name const* inst) {                                                     \
      static_assert(std::is_base_of_v<Component, T>,                                              \
                    "get<T>() Expected value-type derived from Component");                       \
      return static_cast<name##_get const*>(inst)->_get<T>();                                     \
   }                                                                                              \
   template <typename T>                                                                          \
   constexpr T const* get_old(name const* inst) {                                                 \
      static_assert(std::is_base_of_v<Component, T>,                                              \
                    "get_old<T>() Expected value-type derived from Component");                   \
      return static_cast<name##_get const*>(inst)->_get_old<T>();                                 \
   }                                                                                              \
   }