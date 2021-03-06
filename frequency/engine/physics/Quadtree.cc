#include "engine/physics/Quadtree.hh"

#include "engine/core/GObject.hh"
#include "engine/physics/ColliderComponent.hh"

static aabb get_centered_bounds(ColliderComponent* cc) {
   aabb bbox = cc->bounding_box();
   bbox.shift(cc->position());
   return bbox;
}

// Assume insert if insert is called, cc will entirely fit in THIS node
void Quadtree::insert(aabb const& bounds, ColliderComponent* cc) {
   if (leaf) {
      contained_objects.push_back(cc);
      if (contained_objects.size() > MAX_COLLIDERS) {
         subdivide();
      }
   } else {
      for (Quadtree* child : children) {
         if (child->cell_bounds.contains(bounds)) {
            child->insert(bounds, cc);
            return;
         }
      }
      contained_objects.push_back(cc);
   }
}

void Quadtree::query(aabb const& bounds, std::vector<ColliderComponent*>& results_out) {
   results_out.insert(results_out.end(), contained_objects.begin(), contained_objects.end());

   // find the position of where cc belongs, query on that
   if (!leaf) {
      for (Quadtree* child : children) {
         if (child->cell_bounds.contains(bounds)) {
            child->query(bounds, results_out);
            return;
         }
      }

      for (Quadtree* child : children) {
         child->collect(results_out);
      }
   }
}

void Quadtree::remove(aabb const& bounds, ColliderComponent* cc) {
   if (leaf) {
      auto it = std::find(contained_objects.begin(), contained_objects.end(), cc);
      if (it != contained_objects.end()) {
         contained_objects.erase(it);
      }
   } else {
      bool found = false;
      for (Quadtree* child : children) {
         if (child->cell_bounds.contains(bounds)) {
            child->remove(bounds, cc);
            found = true;
         }
      }
      if (found) {
         bool should_kill_children = true;
         for (Quadtree* child : children) {
            if (!child->contained_objects.empty() || !child->leaf) {
               should_kill_children = false;
               break;
            }
         }
         if (should_kill_children) {
            for (int i = 0; i < 4; i++) {
               delete children[i];
               children[i] = nullptr;
            }
            // since we have no children now, we are a leaf
            leaf = true;
         }
      } else {
         auto it = std::find(contained_objects.begin(), contained_objects.end(), cc);
         if (it != contained_objects.end()) {
            contained_objects.erase(it);
         }
      }
   }
}

void Quadtree::clear() {
   if (!leaf) {
      for (int i = 0; i < children.size(); i++) {
         delete children[i];
         children[i] = nullptr;
      }
   }
   leaf = true;
   contained_objects.clear();
}

void Quadtree::collect(std::vector<ColliderComponent*>& out) {
   out.insert(out.end(), contained_objects.begin(), contained_objects.end());

   if (!leaf) {
      for (Quadtree* child : children) {
         child->collect(out);
      }
   }
}

void Quadtree::subdivide() {
   if ((level + 1) >= MAX_LEVELS) {
      return;
   }

   aabb top_left, top_right, bottom_left, bottom_right;

   top_left.min = cell_bounds.min;
   top_left.max = (cell_bounds.min + cell_bounds.max) * 0.5f;

   top_right.min = vec2((cell_bounds.min.x + cell_bounds.max.x) * 0.5f, cell_bounds.min.y);
   top_right.max = vec2(cell_bounds.max.x, (cell_bounds.min.y + cell_bounds.max.y) * 0.5f);

   bottom_left.min = vec2(cell_bounds.min.x, (cell_bounds.min.y + cell_bounds.max.y) * 0.5f);
   bottom_left.max = vec2((cell_bounds.min.x + cell_bounds.max.x) * 0.5f, cell_bounds.max.y);

   bottom_right.min = top_left.max;
   bottom_right.max = cell_bounds.max;

   children[0] = new Quadtree(top_left, level + 1);
   children[1] = new Quadtree(top_right, level + 1);
   children[2] = new Quadtree(bottom_left, level + 1);
   children[3] = new Quadtree(bottom_right, level + 1);

   leaf = false;

   std::vector<ColliderComponent*> tmp = std::move(contained_objects);
   // contained objects is empty, tmp contains that data

   for (ColliderComponent* cc : tmp) {
      insert(get_centered_bounds(cc), cc);
   }
}