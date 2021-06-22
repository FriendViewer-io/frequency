#include "engine/render/ImageResource.hh"

// clang-format off
#include <IL/il.h>
#include <gl/glew.h>
// clang-format on

ImageResource::ImageResource(std::string_view path) : file_loaded(false) {
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
   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

   glActiveTexture(GL_TEXTURE0);
   glGenTextures(1, &gl_image_handle);
   glBindTexture(GL_TEXTURE_2D, gl_image_handle);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width(), height(), 0, ilGetInteger(IL_IMAGE_FORMAT),
                GL_UNSIGNED_BYTE, data());

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

bool ImageResource::loaded() const { return il_image_handle != 0 && file_loaded; }

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

void ImageResource::bind_image() const {
   if (loaded()) {
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, gl_image_handle);
   }
}

ImageResource::~ImageResource() {
   if (loaded()) {
      ilBindImage(il_image_handle);
      ilDeleteImage(il_image_handle);
      il_image_handle = 0;
      file_loaded = false;
   }
}