#pragma once

class ColliderComponent;

namespace collision {

bool overlap_test_gjk(ColliderComponent* a, ColliderComponent* b);

}