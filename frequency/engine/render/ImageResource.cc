#include "engine/render/ImageResource.hh"

#include <IL/il.h>

ImageResource::ImageResource(std::string_view path)
   : file_loaded(false) {
   if (!il_initialized) {
      il_initialized = true;
      ilInit();
   }

   il_image_handle = ilGenImage();
   ilBindImage(il_image_handle);
   if (!ilLoadImage(path.data())) {
      ilDeleteImage(il_image_handle);
      il_image_handle = 0;
   } else {
      file_loaded = true;
   }
}

bool ImageResource::loaded() const {
   return il_image_handle != 0 && file_loaded;
}

int ImageResource::width() const {
   if (loaded()) {
      ilBindImage(il_image_handle);
      return ilGetInteger(IL_IMAGE_WIDTH);
   }
   return -1;
}

int ImageResource::height() const {
   if (loaded()) {
      ilBindImage(il_image_handle);
      return ilGetInteger(IL_IMAGE_HEIGHT);
   }
   return -1;
}

uint8_t const* ImageResource::data() const {
   if (loaded()) {
      ilBindImage(il_image_handle);
      return static_cast<uint8_t const*>(ilGetData());
   }
   return nullptr;
}

ImageResource::~ImageResource() {
   if (loaded()) {
      ilBindImage(il_image_handle);
      ilDeleteImage(il_image_handle);
      il_image_handle = 0;
      file_loaded = false;
   }
}