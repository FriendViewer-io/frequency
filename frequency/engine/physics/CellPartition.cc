#include "engine/physics/CellPartition.hh"

#include "engine/core/GObject.hh"
#include "engine/physics/ColliderComponent.hh"

static aabb get_centered_bounds(ColliderComponent* cc) {
   aabb bbox = cc->bounding_box();
   bbox.shift(cc->position());
   return bbox;
}

template <typename T>
static T clamp(T min, T max, T val) {
   return std::min(max, std::max(min, val));
}

CellPartition::CellPartition(aabb const& world_bounds, vec2 cell_dimensions)
    : world_bounds(world_bounds), cell_dimensions(cell_dimensions) {
   col_count =
       static_cast<int>(ceilf((world_bounds.max.x - world_bounds.min.x) / cell_dimensions.x));
   row_count =
       static_cast<int>(ceilf((world_bounds.max.y - world_bounds.min.y) / cell_dimensions.y));
   object_map.resize(row_count);

   for (auto& row : object_map) {
      row.resize(col_count);
   }
}

void CellPartition::insert(aabb const& bounds, ColliderComponent* cc) {
   int xmin = static_cast<int>((bounds.min.x - world_bounds.min.x) / cell_dimensions.x);
   int ymin = static_cast<int>((bounds.min.y - world_bounds.min.y) / cell_dimensions.y);
   int xmax = static_cast<int>((bounds.max.x - world_bounds.min.x) / cell_dimensions.x);
   int ymax = static_cast<int>((bounds.max.y - world_bounds.min.y) / cell_dimensions.y);

   xmin = clamp(0, col_count - 1, xmin);
   xmax = clamp(0, col_count - 1, xmax);
   ymin = clamp(0, row_count - 1, ymin);
   ymax = clamp(0, row_count - 1, ymax);

   if (xmin == xmax && ymin == ymax) {
      object_map[ymin][xmin].push_back(cc);
   } else {
      for (int i = ymin; i <= ymax; i++) {
         for (int j = xmin; j <= xmax; j++) {
            object_map[i][j].push_back(cc);
         }
      }
   }
}

void CellPartition::query(aabb const& bounds, std::vector<ColliderComponent*>& query_result) {
   int xmin = static_cast<int>((bounds.min.x - world_bounds.min.x) / cell_dimensions.x);
   int ymin = static_cast<int>((bounds.min.y - world_bounds.min.y) / cell_dimensions.y);
   int xmax = static_cast<int>((bounds.max.x - world_bounds.min.x) / cell_dimensions.x);
   int ymax = static_cast<int>((bounds.max.y - world_bounds.min.y) / cell_dimensions.y);

   xmin = clamp(0, col_count - 1, xmin);
   xmax = clamp(0, col_count - 1, xmax);
   ymin = clamp(0, row_count - 1, ymin);
   ymax = clamp(0, row_count - 1, ymax);

   if (xmin == xmax && ymin == ymax) {
      query_result.insert(query_result.end(), object_map[ymin][xmin].begin(),
                          object_map[ymin][xmin].end());
   } else {
      for (int i = ymin; i <= ymax; i++) {
         for (int j = xmin; j <= xmax; j++) {
            query_result.insert(query_result.end(), object_map[i][j].begin(),
                                object_map[i][j].end());
         }
      }
      // yes sort by address
      std::sort(query_result.begin(), query_result.end());
      query_result.erase(std::unique(query_result.begin(), query_result.end()), query_result.end());
   }
}

void CellPartition::remove(aabb const& bounds, ColliderComponent* cc) {
   int xmin = static_cast<int>((bounds.min.x - world_bounds.min.x) / cell_dimensions.x);
   int ymin = static_cast<int>((bounds.min.y - world_bounds.min.y) / cell_dimensions.y);
   int xmax = static_cast<int>((bounds.max.x - world_bounds.min.x) / cell_dimensions.x);
   int ymax = static_cast<int>((bounds.max.y - world_bounds.min.y) / cell_dimensions.y);

   xmin = clamp(0, col_count - 1, xmin);
   xmax = clamp(0, col_count - 1, xmax);
   ymin = clamp(0, row_count - 1, ymin);
   ymax = clamp(0, row_count - 1, ymax);

   if (xmin == xmax && ymin == ymax) {
      auto it = std::find(object_map[ymin][xmin].begin(), object_map[ymin][xmin].end(), cc);
      if (it != object_map[ymin][xmin].end()) {
         object_map[ymin][xmin].erase(it);
      }
   } else {
      for (int i = ymin; i <= ymax; i++) {
         for (int j = xmin; j <= xmax; j++) {
            auto it = std::find(object_map[i][j].begin(), object_map[i][j].end(), cc);
            if (it != object_map[i][j].end()) {
               object_map[i][j].erase(it);
            }
         }
      }
   }
}

void CellPartition::clear() {
   for (auto& row : object_map) {
      for (auto& cell : row) {
         cell.clear();
      }
   }
}