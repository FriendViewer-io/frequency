#include "engine/physics/CellPartition.hh"

#include "engine/core/GObject.hh"
#include "engine/physics/ColliderComponent.hh"

aabb get_centered_bounds(ColliderComponent* cc) {
   aabb bbox = cc->bounding_box();
   bbox.shift(cc->get_parent()->get_staging_position());
   return bbox;
}

CellPartition::CellPartition(aabb const& world_bounds, vec2 cell_dimensions)
    : world_bounds(world_bounds), cell_dimensions(cell_dimensions) {
   int num_cells_width =
       static_cast<int>(ceilf((world_bounds.max.x - world_bounds.min.x) / cell_dimensions.x));
   int num_cells_height =
       static_cast<int>(ceilf((world_bounds.max.y - world_bounds.min.y) / cell_dimensions.y));
   object_map.resize(num_cells_height);

   for (auto& row : object_map) {
      row.resize(num_cells_width);
   }
}

void CellPartition::insert(ColliderComponent* cc) { 
   aabb bounds = get_centered_bounds(cc);
   // find the cell that bounds.min and bounds.max belongs to
   // cell_dimensions.x = width of each cell
   // cell_dimensions.y = height of each cell
   int xmin = static_cast<int>((bounds.min.x - world_bounds.min.x) / cell_dimensions.x);
   int ymin = static_cast<int>((bounds.min.y - world_bounds.min.y) / cell_dimensions.y);
   int xmax = static_cast<int>((bounds.max.x - world_bounds.min.x) / cell_dimensions.x);
   int ymax = static_cast<int>((bounds.max.y - world_bounds.min.y) / cell_dimensions.y);

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

void CellPartition::query(ColliderComponent* cc, std::vector<ColliderComponent*>& query_result) {
   aabb bounds = get_centered_bounds(cc);

   int xmin = static_cast<int>((bounds.min.x - world_bounds.min.x) / cell_dimensions.x);
   int ymin = static_cast<int>((bounds.min.y - world_bounds.min.y) / cell_dimensions.y);
   int xmax = static_cast<int>((bounds.max.x - world_bounds.min.x) / cell_dimensions.x);
   int ymax = static_cast<int>((bounds.max.y - world_bounds.min.y) / cell_dimensions.y);

   if (xmin == xmax && ymin == ymax) {
      query_result.insert(query_result.end(), object_map[ymin][xmin].begin(), object_map[ymin][xmin].end());
   } else {
      for (int i = ymin; i <= ymax; i++) {
         for (int j = xmin; j <= xmax; j++) {
            query_result.insert(query_result.end(), object_map[i][j].begin(), object_map[i][j].end());
         }
      }
   }
}

void CellPartition::remove(ColliderComponent* cc) {
   aabb bounds = get_centered_bounds(cc);

   int xmin = static_cast<int>((bounds.min.x - world_bounds.min.x) / cell_dimensions.x);
   int ymin = static_cast<int>((bounds.min.y - world_bounds.min.y) / cell_dimensions.y);
   int xmax = static_cast<int>((bounds.max.x - world_bounds.min.x) / cell_dimensions.x);
   int ymax = static_cast<int>((bounds.max.y - world_bounds.min.y) / cell_dimensions.y);

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