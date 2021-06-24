#include "engine/render/AnimationComponent.hh"

#include "engine/core/GObject.hh"
#include "engine/core/ResourceManager.hh"
#include "engine/core/StateManager.hh"
#include "engine/render/Camera.hh"
#include "engine/render/RenderExtension.hh"
#include "engine/render/RenderMeshResource.hh"
#include "engine/render/ShaderResource.hh"
#include "engine/render/SpritesheetResource.hh"

AnimationComponent::AnimationComponent()
    : cur_animation(nullptr), cur_frame(0), ticks_on_frame(0) {}

void AnimationComponent::on_tick(float dt) {
   if (cur_animation != nullptr) {
      if (ticks_on_frame >= cur_animation->frames[cur_frame].duration_ticks) {
         cur_frame++;
         if (cur_frame >= cur_animation->frames.size()) {
            cur_frame = 0;
         }
         ticks_on_frame = 0;
      } else {
         ticks_on_frame++;
      }
   }
}

void AnimationComponent::set_animation(std::string_view name) {
   if (sheet_data) {
      cur_animation = sheet_data->get_animation(name);
   }
}

void AnimationComponent::on_post_tick(float dt) const {
   if (cur_animation != nullptr) {
      vec2 half_dims = cur_animation->max_dims * 0.5f;
      vec2 real_center = get_parent()->get_position() + center_offset;
      render_bounds = aabb(real_center - half_dims, real_center + half_dims);
   } else {
      render_bounds = aabb(vec2(FLT_MAX, FLT_MAX), vec2(FLT_MAX, FLT_MAX));
   }

   // does NOT deal with rotation
}

void AnimationComponent::load_data(std::string_view sprite_data_source) {
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

   shader = ResourceManager::load_file<ShaderResource>("shaders/spritesheet", load_shader_from_file)
                .value();
   sheet_data = ResourceManager::load_file<SpritesheetResource>(
                    sprite_data_source,
                    [](std::string_view path) {
                       auto sheet_res = std::make_unique<SpritesheetResource>();
                       sheet_res->load_sheet(path);
                       return std::move(sheet_res);
                    })
                    .value();
}

void AnimationComponent::bind_data(Camera const* camera) const {   
   RenderComponent::bind_data(camera);

   sheet_data->bind_image();
   float win_w = static_cast<float>(statemgr::get_render_extension()->window_width()),
         win_h = static_cast<float>(statemgr::get_render_extension()->window_height());
   shader->set_uniform("window_dims", vec2(win_w, win_h));
   shader->set_uniform("sheet_dims",
                       vec2(sheet_data->width(), sheet_data->height()) * get_parent()->get_scale());
   if (cur_animation == nullptr) {
      shader->set_uniform("sprite_clip_min", vec2());
      shader->set_uniform("sprite_clip_max", vec2());
   } else {
      aabb const& clip = cur_animation->sprite_clips[cur_animation->frames[cur_frame].sprite_clip_index];
      shader->set_uniform("sprite_clip_min", clip.min);
      shader->set_uniform("sprite_clip_max", clip.max);
   }
   shader->set_uniform("position", camera->get_adjusted_center(get_parent()) + center_offset);
   shader->set_uniform("anchor_offset", anchor_offset);
   shader->set_uniform("rotation", get_parent()->get_rotation());
   shader->set_uniform_i("image_tex", 0);
}

AnimationComponent::~AnimationComponent() {}