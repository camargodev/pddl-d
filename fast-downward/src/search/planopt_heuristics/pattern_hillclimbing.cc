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

    /** states_with_p stores the number of abstract states achieavable with the variables in pattern p
        values stored in variable_domains[] are explained in line 50 of tnf_task.h
    **/
    int states = 0;
    for (auto p : collection) { // for each pattern...
        int states_with_p = 1;
        for (auto v : p) { //... and for each variable in that pattern...
            states_with_p = states_with_p * task.variable_domains[v]; // multiplies states_with_p by the number of
                                                                      // values the variable v can assume. doing this
                                                                      // for every variable will give us the total
                                                                      // amount of abstract states the pattern p can
                                                                      // assume.
        }
        states += states_with_p;
        if (states >= size_bound) {
            return false;
        }
    }
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
    for(unsigned int v = 0; v < task.goal_state.size(); v++){
      Pattern p;
      p.push_back(v);
      collection.push_back(p);
    }

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
    for(auto p : collection){
      set<int> s1;
      set<int>::iterator it;
      for(auto v : p){ //compute the set of variables that are causally relevant for any V in P
        for(auto v2 : causally_relevant_variables[v]){
          s1.insert(v2);
        }
      }
      for(auto v : p){ //remove all variables from this set that already occur in P
        it=s1.find(v);
        if(it!=s1.end()){
          s1.erase(it);
        }
      }
      for(auto v : s1){ //for each variable V in the resulting set:
        vector<Pattern> new_collection = collection;
        set<int> new_p_set(p.begin(), p.end());
        unsigned int prev_size = new_p_set.size();
        new_p_set.insert(v); //P u {V}
        unsigned int curr_size = new_p_set.size();
        if(curr_size > prev_size){ // successful insertion of another variable to the pattern P (if false, the pattern is equal to one that's already in the collection)
          bool new_p_exists_in_c = false;
          for(auto old_p : collection){ // checks if the pattern created above is already part of the collection
            set<int> old_p_set(old_p.begin(), old_p.end());
            if(old_p_set == new_p_set){
              new_p_exists_in_c = true;
              break;
            }
          }
          if(!new_p_exists_in_c){ // if not, then we add it to the collection and the new collection is a neighbor of the old collection
            Pattern new_p(new_p_set.begin(), new_p_set.end());
            new_collection.push_back(new_p);
            if(fits_size_bound(new_collection)){
              neighbors.push_back(new_collection);
            }
          }
        }
      }      
    }
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
    while(true){
      vector<Pattern> next_collection;
      int improvement = 0;
      vector<vector<Pattern>> neighs = compute_neighbors(current_collection);
      for(auto neigh : neighs){
        vector<int> next_sample_values = compute_sample_heuristics(neigh);
        int counter = 0;
        for(unsigned int i = 0; i < next_sample_values.size(); i++){
          if(next_sample_values[i] > current_sample_values[i])
            counter++;
        }
        if(counter > improvement){
          improvement = counter;
          next_collection = neigh;
        }
      }
      if(improvement == 0)
        return current_collection;
      current_collection = next_collection;
      current_sample_values = compute_sample_heuristics(current_collection);
    }

    return current_collection;
}
}