#include "engine/physics/Solver.hh"

#include "engine/physics/ColliderComponent.hh"
#include "engine/physics/CollisionHelper.hh"
#include "engine/core/GObject.hh"

namespace solver {
solver_func active_solver = push_noimpulse_solver;

void set_solver(solver_func&& solver) { active_solver = std::move(solver); }

void solve_collision(collision::ContactManifold& manifold) { active_solver(manifold); }

void push_noimpulse_solver(collision::ContactManifold& manifold) {
   ColliderComponent* a = manifold.a;
   ColliderComponent* b = manifold.b;

   vec2 centroid_diff = (a->get_parent()->get_position() + a->get_centroid()) -
                        (b->get_parent()->get_position() + b->get_centroid());

   if (centroid_diff.dot(manifold.normal) < 0.f) {
      manifold.normal = -manifold.normal;
   }

   if (!a->is_static() && !b->is_static()) {
      float total_mass = a->get_mass() + b->get_mass();
      float a_res, b_res;
      if (total_mass == 0.f) {
         a_res = 0.5f;
         b_res = 0.5f;
      } else {
         a_res = b->get_mass() / total_mass;
         b_res = a->get_mass() / total_mass;
      }

      a->push_object(manifold.normal * a_res * manifold.overlap_distance);
      b->push_object(-manifold.normal * b_res * manifold.overlap_distance);
      vec2 av = a->get_velocity();
      vec2 bv = b->get_velocity();
      if (av.dot(manifold.normal) < 0.f) {
         av.project(manifold.tangent);
      }
      if (bv.dot(-manifold.normal) < 0.f) {
         bv.project(manifold.tangent);
      }
      a->set_velocity(av);
      b->set_velocity(bv);
   } else if (a->is_static()) {
      b->push_object(-manifold.normal * manifold.overlap_distance);
      vec2 bv = b->get_velocity();
      if (bv.dot(-manifold.normal) < 0.f) {
         bv.project(manifold.tangent);
      }
      b->set_velocity(bv);
   } else if (b->is_static()) {
      a->push_object(manifold.normal * manifold.overlap_distance);
      vec2 av = a->get_velocity();
      if (av.dot(manifold.normal) < 0.f) {
         av.project(manifold.tangent);
      }
      a->set_velocity(av);
   }
}

}  // namespace solver