#pragma once

#include <functional>

namespace collision {
struct ContactManifold;
}

namespace solver {

using solver_func = std::function<void(collision::ContactManifold&)>;

void set_solver(solver_func&& solver);

void solve_collision(collision::ContactManifold& manifold);

void push_noimpulse_solver(collision::ContactManifold& manifold);

}