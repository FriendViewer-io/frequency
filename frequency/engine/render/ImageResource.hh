#pragma once

#include <string_view>

#include "engine/core/EngineResource.hh"

class ImageResource : public Resource {
public:
   ImageResource();

   void load_image(std::string_view path);
   virtual bool loaded() const;
   int width() const;
   int height() const;
   uint8_t const* data() const;
   void bind_image() const;

   virtual ~ImageResource();

protected:
   uint32_t il_image_handle;
   uint32_t gl_image_handle;
   bool file_loaded;

   inline static bool il_initialized = false;
};