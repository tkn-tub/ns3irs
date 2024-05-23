#ifndef LOOKUP_STRUCTS_H
#define LOOKUP_STRUCTS_H

#include <utility>

struct IrsEntry {
    double gain;
    double phase_shift;
};

struct hash_tuple {
    template <class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2>& p) const {
        auto hash1 = std::hash<T1>{}(p.first);
        auto hash2 = std::hash<T2>{}(p.second);
        return hash1 ^ hash2; // Combine the hashes
    }
};

#endif // LOOKUP_STRUCTS_H
