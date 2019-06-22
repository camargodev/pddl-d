#ifndef PLANOPT_HEURISTICS_PATTERN_HILLCLIMBING_H
#define PLANOPT_HEURISTICS_PATTERN_HILLCLIMBING_H

#include "projection.h"

#include <set>
#include <vector>

namespace planopt_heuristics {
class HillClimber {
    const TNFTask &task;
    int size_bound;
    std::vector<TNFState> samples;
    const std::vector<std::set<int>> causally_relevant_variables;

    bool fits_size_bound(const std::vector<Pattern> &collection) const;
    std::vector<Pattern> compute_initial_collection();
    std::vector<std::vector<Pattern>> compute_neighbors(
        const std::vector<Pattern> &collection);
    std::vector<int> compute_sample_heuristics(const std::vector<Pattern> &collection);
public:
    HillClimber(const TNFTask &task, int size_bound, std::vector<TNFState> &&samples);
    std::vector<Pattern> run();
};
}

#endif
