#include "engine/render/SpritesheetResource.hh"

#include <fstream>
#include <regex>

namespace {
void create_clip(AnimData& data, std::string const& line) {
   std::regex clip_rx("^\\(\\s*([0-9]+)\\s*,\\s*([0-9]+)\\)\\s*\\(\\s*([0-9]+)\\s*,\\s*([0-9]+)\\)\\s*$");

   std::smatch matches;
   if (std::regex_match(line, matches, clip_rx)) {
      if (matches.size() < 5) {
         return;
      }
      std::string s0x = matches[1], s0y = matches[2], s1x = matches[3], s1y = matches[4];
      unsigned long x0 = strtoul(s0x.c_str(), nullptr, 10);
      unsigned long y0 = strtoul(s0y.c_str(), nullptr, 10);
      unsigned long x1 = strtoul(s1x.c_str(), nullptr, 10);
      unsigned long y1 = strtoul(s1y.c_str(), nullptr, 10);

      aabb new_clip = aabb(vec2(x0, y0), vec2(x1, y1));
      vec2 new_clip_dims = new_clip.max - new_clip.min;
      data.max_dims.x = std::max(new_clip_dims.x, data.max_dims.x);
      data.max_dims.y = std::max(new_clip_dims.y, data.max_dims.y);

      data.sprite_clips.emplace_back(new_clip);
   }
}

void create_frame(AnimData& data, std::string const& line) {
   std::regex frame_rx("^([0-9]+)\\s*,\\s*([0-9]+)\\s*$");

   std::smatch matches;
   if (std::regex_match(line, matches, frame_rx)) {
      if (matches.size() < 3) {
         return;
      }
      std::string clip_index_str = matches[1], duration_str = matches[2];
      unsigned long sprite_clip_index = strtoul(clip_index_str.c_str(), nullptr, 10);
      unsigned long duration_ticks = strtoul(duration_str.c_str(), nullptr, 10);

      data.frames.emplace_back(static_cast<int>(sprite_clip_index), static_cast<int>(duration_ticks));
   }
}

inline void rtrim(std::string &s) {
   s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
      return !std::isspace(ch);
   }).base(), s.end());
}
}

SpritesheetResource::SpritesheetResource() : ImageResource(), sheet_loaded(false) {}

void SpritesheetResource::load_sheet(std::string_view sheet_path) {
   std::ifstream ss_stream(std::string(sheet_path), std::ios::in);

   AnimData* cur_animation = nullptr;

   while (!ss_stream.eof()) {
      std::string line;
      std::getline(ss_stream, line);

      std::regex tag_rx("^\\s*<(\\w+)>\\s*(.*)\\s*$");
      std::smatch matches;

      if (std::regex_match(line, matches, tag_rx)) {
         if (matches.size() < 3) {
            continue;
         }

         std::string type = matches[1];
         if (type == "Sheet") {
            std::string image_path = matches[2];
            rtrim(image_path);
            load_image(image_path);
         } else if (type == "Anim") {
            std::string anim_name = matches[2];
            rtrim(anim_name);
            auto& new_anim = animations[anim_name];
            new_anim.anim_name = anim_name;
            cur_animation = &new_anim;
         } else if (type == "Clip") {
            if (cur_animation == nullptr) {
               continue;
            }
            create_clip(*cur_animation, matches[2]);
         } else if (type == "Frame") {
            if (cur_animation == nullptr) {
               continue;
            }
            create_frame(*cur_animation, matches[2]);
         }
      }
   }
}