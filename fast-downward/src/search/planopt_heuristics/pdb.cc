#include "pdb.h"

#include "../utils/logging.h"

#include <queue>

using namespace std;

namespace planopt_heuristics {

/*
  An entry in the queue is a tuple (h, i) where h is the goal distance of state i.
  See comments below for details.
*/
using QueueEntry = pair<int, int>;

PatternDatabase::PatternDatabase(const TNFTask &task, const Pattern &pattern)
    : projection(task, pattern) {
    /*
      We want to compute goal distances for all abstract states in the
      projected task. To do so, we start by assuming every abstract state has
      an infinite goal distance and then do a backwards uniform cost search
      updating the goal distances of all encountered states.

      Instead of searching on the actual states, we use perfect hashing to
      run the search on the hash indices of states. To go from a state s to its
      index use rank(s) and to go from an index i to its state use unrank(i).
    */
    const TNFTask &projected_task = projection.get_projected_task();
    distances.resize(projected_task.get_num_states(), numeric_limits<int>::max());

    /*
      Priority queues usually order entries so the largest entry is the first.
      By using the comparator greater<T> instead of the default less<T>, we
      change the ordering to sort the smallest element first.
    */
    priority_queue<QueueEntry, vector<QueueEntry>, greater<QueueEntry>> queue;
    /*
      Note that we start with the goal state to turn the search into a regression.
      We also have to switch the role of precondition and effect in operators
      later on. This is sufficient to turn the search into a regression since
      the task is in TNF.
    */
    auto goal_state_index = projection.rank_state(projected_task.goal_state);
    distances[goal_state_index] = 0;
    queue.push(make_pair(distances[goal_state_index], goal_state_index));

    while(queue.size() > 0) {
        auto queue_entry = queue.top();
        queue.pop();
        auto current_state_cost = queue_entry.first;
        auto current_state = projection.unrank_state(queue_entry.second);
        
        for (auto tnf_operator : projected_task.operators) {
            bool is_pred_state_reachable = true;
            auto pred_state = current_state; 
            
            for (auto entry : tnf_operator.entries) {
                // uma entrada de um operador é uma tripla (v,p,e), onde:
                // v = variável (variable_id)
                // p = valor da variável antes do operador (precondition_value)
                // e = valor da variável depois do operator (effect_value)
                // se os efeitos de todas as entradas forem iguais ao estado atual S,
                // então trocar os valores de S para os valores das pré-cond, 
                // gerará um estado S' que é predecessor de S  
                if (entry.effect_value != pred_state[entry.variable_id]) 
                    is_pred_state_reachable = false;
                pred_state[entry.variable_id] = entry.precondition_value;
            }
            
            if (!is_pred_state_reachable) 
                continue;
            
            int curr_state_cost_with_operator = current_state_cost + tnf_operator.cost;
            auto pred_state_index = projection.rank_state(pred_state);
            if (curr_state_cost_with_operator >= distances[pred_state_index])
                continue;

            distances[pred_state_index] = curr_state_cost_with_operator;
            queue.push(make_pair(curr_state_cost_with_operator, pred_state_index));
        }
    }
}

int PatternDatabase::lookup_distance(const TNFState &original_state) const {
    TNFState abstract_state = projection.project_state(original_state);
    int index = projection.rank_state(abstract_state);
    return distances[index];

}

}
