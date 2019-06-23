#include "canonical_pdbs.h"

#include "../algorithms/max_cliques.h"

using namespace std;

// single execution examples (for debugging):
    //./fast-downward/fast-downward.py --build=release64 nomystery-opt11-strips/p01.pddl --search "astar(planopt_cpdbs(patterns=[[0, 2, 3, 4]]))"
    //./fast-downward/fast-downward.py --build=release64 nomystery-opt11-strips/p03.pddl --search 'astar(planopt_cpdbs(patterns=[[4, 5, 6], [2], [3]]))'

namespace planopt_heuristics {

void print_matrix(vector<vector<int> > matrix){
    cout << "Matrix:" << endl;
	for(unsigned int i = 0; i < matrix.size(); i++){
		for(unsigned int j = 0; j < matrix.size(); j++){
			cout << matrix[i][j] << "\t";
		}
		cout << "\n";
	}
}

bool affects_pattern(const TNFOperator &op, const Pattern &pattern) {
    for (TNFOperatorEntry entry : op.entries) {
        for (int var_id : pattern) {
            if (entry.variable_id == var_id && entry.precondition_value != entry.effect_value) {
                return true;
            }
        }
    }
    return false;
}

vector<vector<int>> build_compatibility_graph(const vector<Pattern> &patterns, const TNFTask &task) {
    /*
      Build the compatibility graph of the given pattern collection in the form
      of adjacency lists: the outer vector has one entry for each pattern
      representing the vertices of the graph. Each such entry is a vector of
      ints that represents the outgoing edges of that vertex, i.e., an edge
      to each other vertex that represents an additive pattern.
    */

    vector<vector<int>> graph(patterns.size());
    /*for(unsigned int i = 0; i < graph.size(); i++){
        graph[i].resize(patterns.size());
    }*/
    for(unsigned int i = 0; i < patterns.size(); i++){      //selects one pattern from pattern
        for(unsigned int j = 0; j < patterns.size(); j++){  //selects one more pattern to be compared with the pattern selected above
            bool affects_i = false;
            bool affects_j = false;
            for(auto o : task.operators){
                affects_i = affects_pattern(o, patterns[i]);
                affects_j = affects_pattern(o, patterns[j]);
                if(affects_i and affects_j) //if at least one operator affects both patterns...
                    break;                  //...then there is no edge between them and we can stop analysing the operators.
            }
            if(affects_i and affects_j){
                //graph[i][j]=0;
                //graph[j][i]=0;
            }
            else{ //if no operator affects both patterns, then i has an outgoing edge to j
                //graph[i][j]=1;
                //graph[j][i]=1;
                graph[i].push_back(j);
            }
        }
    }
    return graph;
}

CanonicalPatternDatabases::CanonicalPatternDatabases(
    const TNFTask &task, const vector<Pattern> &patterns) {
    for (const Pattern &pattern : patterns) {
        pdbs.emplace_back(task, pattern);
    }

    vector<vector<int>> compatibility_graph = build_compatibility_graph(patterns, task);
    max_cliques::compute_max_cliques(compatibility_graph, maximal_additive_sets);

}

int CanonicalPatternDatabases::compute_heuristic(const TNFState &original_state) {
    /*
      To avoid the overhead of looking up the heuristic value of a PDB multiple
      times (if that PDB occurs in multiple cliques), we pre-compute all
      heuristic values. Use heuristic_values[i] for the heuristic value of
      pdbs[i] in your code below.
    */
    vector<int> heuristic_values;
    heuristic_values.reserve(pdbs.size());
    for (const PatternDatabase &pdb : pdbs) {
        heuristic_values.push_back(pdb.lookup_distance(original_state));
        /*
          special case: if one of the PDBs detects unsolvability, we can
          return infinity right away. Otherwise, we would have to deal with
          integer overflows when adding numbers below.
        */
        if (heuristic_values.back() == numeric_limits<int>::max()) {
            return numeric_limits<int>::max();
        }
    }

    /*
      Use maximal_additive_sets and heuristic_values to compute the value
      of the canonical heuristic.
    */
    int h = 0;

    for(unsigned int clique = 0; clique < maximal_additive_sets.size(); clique++){ // iterates through each clique in maximal_additive_sets
        int sum = 0;
        for(auto pdb : maximal_additive_sets[clique]){ // for each pdb in the clique, add its heuristic value to sum
            sum += heuristic_values[pdb];
        }
        if(sum>h) // selects the greatest value of sum as the heuristic value
            h = sum;
    }

    return h;
}






}
