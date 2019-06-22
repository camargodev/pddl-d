#include "pattern_hillclimbing.h"

#include "canonical_pdbs.h"

#include "../globals.h"

#include "../utils/logging.h"

using namespace std;

namespace planopt_heuristics {
vector<set<int>> compute_causally_relevant_variables(const TNFTask &task) {
    /*
        v is causally relevant for w
      iff
        v is a predecessor of w in the causal graph, or
        v is a successor of w in the causal graph and mentioned in the goal
      Since we are considering TNF tasks where all variables are mentioned in
      the goal, this condition simplifies to

        v is causally relevant for w
      iff
        v and w are neighbors in the causal graph.
      iff
        v and w occur in the same operator (and at least one of them changes the state).
    */

    int num_variables = task.variable_domains.size();
    vector<set<int>> relevant(num_variables);
    for (const TNFOperator &op : task.operators) {
        for (const TNFOperatorEntry &e1 : op.entries) {
            for (const TNFOperatorEntry &e2 : op.entries) {
                if (e1.variable_id != e2.variable_id &&
                        (e1.precondition_value != e1.effect_value ||
                         e2.precondition_value != e2.effect_value)) {
                    relevant[e1.variable_id].insert(e2.variable_id);
                }
            }
        }
    }
    return relevant;
}

bool HillClimber::fits_size_bound(const std::vector<Pattern> &collection) const {
    /*
      Compute the number of abstract states in the given collection without
      explicitly computing the projections. Return true if the total size
      is below size_bound and false otherwise.
    */


    // TODO: add your code for exercise (f) here.

    return true;
}


HillClimber::HillClimber(const TNFTask &task, int size_bound, vector<TNFState> &&samples)
    : task(task),
      size_bound(size_bound),
      samples(move(samples)),
      causally_relevant_variables(compute_causally_relevant_variables(task)){
}


vector<Pattern> HillClimber::compute_initial_collection() {
    /*
      In TNF, every variable occurs in the goal state, so we create the
      collection {{v} | v \in V}.
    */
    vector<Pattern> collection;

    // TODO: add your code for exercise (f) here.

    return collection;
}

vector<vector<Pattern>> HillClimber::compute_neighbors(const vector<Pattern> &collection) {
    /*
      for each pattern P in the collection C:
          compute the set of variables that are causally relevant for any V in P
             (Use the precomputed information in causally_relevant_variables.)
          remove all variables from this set that already occur in P
          for each variable V in the resulting set:
              add the collection C' := C u {P u {V}} to neighbors

    */
    vector<vector<Pattern>> neighbors;

    // TODO: add your code for exercise (f) here.

    return neighbors;
}

vector<int> HillClimber::compute_sample_heuristics(const vector<Pattern> &collection) {
    CanonicalPatternDatabases cpdbs(task, collection);
    vector<int> values;
    values.reserve(samples.size());
    for (const TNFState &sample : samples) {
        values.push_back(cpdbs.compute_heuristic(sample));
    }
    return values;
}

vector<Pattern> HillClimber::run() {
    vector<Pattern> current_collection = compute_initial_collection();
    vector<int> current_sample_values = compute_sample_heuristics(current_collection);

    /*
      current := an initial candidate
      loop forever:
          next := a neighbor of current with maximal improvement over current
          if improvement(next) = 0:
              return current
          current := next

      To measure improvement, use compute_sample_heuristics to compute
      heuristic values for all sample state. Compare the result to
      current_sample_values and count the number of states that have a higher
      heuristic value. Remember to update current_sample_values when you
      modify current_collection.
    */


    // TODO: add your code for exercise (f) here.
    return current_collection;
}
}
