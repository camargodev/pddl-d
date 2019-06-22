#ifndef PLANOPT_HEURISTICS_H_IPDB_H
#define PLANOPT_HEURISTICS_H_IPDB_H

#include "canonical_pdbs.h"

#include "../heuristic.h"

namespace planopt_heuristics {
class IPDBHeuristic : public Heuristic {
    CanonicalPatternDatabases cpdbs;
protected:
    virtual int compute_heuristic(const GlobalState &state) override;
public:
    explicit IPDBHeuristic(const options::Options &options);
};
}
#endif
