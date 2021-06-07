#pragma once

#include <memory>
#include <string_view>

// All resources must be destructible, thus inherit from Resource
class Resource {
public:
   virtual ~Resource();
};

struct ResourceRef {
   std::unique_ptr<Resource> inner_data;
   // View from map key
   std::string_view res_name;
   int ref_count;

   ResourceRef() : inner_data(nullptr), ref_count(0) {}
   ResourceRef(std::unique_ptr<Resource> r) : inner_data(std::move(r)), ref_count(0) {}
};