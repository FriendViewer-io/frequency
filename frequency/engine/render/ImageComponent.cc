#include "engine/render/ImageComponent.hh"

#include "engine/core/GObject.hh"
#include "engine/core/ResourceManager.hh"
#include "engine/core/StateManager.hh"
#include "engine/render/Camera.hh"
#include "engine/render/ImageResource.hh"
#include "engine/render/RenderExtension.hh"
#include "engine/render/RenderMeshResource.hh"
#include "engine/render/ShaderResource.hh"

void ImageComponent::on_post_tick(float dt) const {
   vec2 half_dims = vec2(get_scaled_width(), get_scaled_height()) * 0.5f;
   vec2 real_center = position() + center_offset;

   render_bounds = aabb(real_center - half_dims, real_center + half_dims);

   // does NOT deal with rotation
}

void ImageComponent::load_data(std::string_view image_source) {
   auto tmp = ResourceManager::lookup_resource<RenderMeshResource>("??IMC_square");
   if (!tmp) {
      auto res = std::make_unique<RenderMeshResource>();
      MeshDescriptor md;
      // clang-format off
      md.raw_vertex_data = {
         -1.f, -1.f, 0.f, 1.f,
         -1.f,  1.f, 0.f, 0.f,
          1.f, -1.f, 1.f, 1.f,
          1.f,  1.f, 1.f, 0.f
      };
      md.raw_index_data = {
         0, 1, 2,
         2, 3, 1
      };
      // clang-format on
      md.vertex_attribute_offsets.push_back(2);
      md.vertex_attribute_offsets.push_back(2);
      res->load_mesh(md);
      mesh = ResourceManager::wrap_resource<RenderMeshResource>("??IMC_square", std::move(res))
                 .value();
   } else {
      mesh = tmp.value();
   }

   shader =
       ResourceManager::load_file<ShaderResource>("shaders/image", load_shader_from_file).value();
   tex_data = ResourceManager::load_file<ImageResource>(image_source, [](std::string_view path) {
                 auto image_res = std::make_unique<ImageResource>();
                 image_res->load_image(path);
                 return std::move(image_res);
              }).value();
}

void ImageComponent::bind_data(Camera const* camera) const {
   RenderComponent::bind_data(camera);

   tex_data->bind_image();
   float win_w = static_cast<float>(statemgr::get_render_extension()->window_width()),
         win_h = static_cast<float>(statemgr::get_render_extension()->window_height());
   shader->set_uniform("window_dims", vec2(win_w, win_h));
   shader->set_uniform("image_dims",
                       vec2(tex_data->width(), tex_data->height()) * scale());

   shader->set_uniform("position", camera->get_adjusted_center(get_parent()) + center_offset);
   shader->set_uniform("anchor_offset", anchor_offset);
   shader->set_uniform("rotation", rotation());
   shader->set_uniform_i("image_tex", 0);
}

float ImageComponent::get_base_width() const { return static_cast<float>(tex_data->width()); }

float ImageComponent::get_base_height() const { return static_cast<float>(tex_data->height()); }

float ImageComponent::get_scaled_width() const {
   return get_base_width() * scale().x;
}

float ImageComponent::get_scaled_height() const {
   return get_base_height() * scale().y;
}

ImageComponent::~ImageComponent() {}