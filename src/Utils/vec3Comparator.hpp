#pragma once

struct vec3Comparator {
    bool operator()(const glm::vec3& a, const glm::vec3& b) const {
        return std::tie(a.x, a.y, a.z) < std::tie(b.x, b.y, b.z);
    }
};
