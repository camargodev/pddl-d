#include "h_ipdb.h"

#include "pattern_hillclimbing.h"

#include "../globals.h"
#include "../option_parser.h"
#include "../plugin.h"

#include "../task_utils/sampling.h"
#include "../task_utils/task_properties.h"
#include "../utils/logging.h"
#include "../utils/rng.h"

using namespace std;

namespace planopt_heuristics {
CanonicalPatternDatabases create_cpdbs_by_hillclimbing(const TaskProxy &task_proxy, int size_bound) {
    TNFTask task = create_tnf_task(task_proxy);

    vector<Pattern> sampling_collection;
    for (FactProxy goal : task_proxy.get_goals()) {
        sampling_collection.push_back({goal.get_variable().get_id()});
    }
    CanonicalPatternDatabases sampling_heuristic(task, sampling_collection);

    int init_h = sampling_heuristic.compute_heuristic(task_proxy.get_initial_state().get_values());
    utils::RandomNumberGenerator rng(2017);
    int average_operator_cost = task_properties::get_average_operator_cost(task_proxy);

    g_log << "Sampling states for iPDB hillclimbing" << endl;
    vector<State> samples = sampling::sample_states_with_random_walks(
        task_proxy, *g_successor_generator, 1000, init_h,
        average_operator_cost,
        rng,
        [&](const State &state) {
            return sampling_heuristic.compute_heuristic(state.get_values()) == numeric_limits<int>::max();
        });
    vector<TNFState> tnf_samples;
    tnf_samples.reserve(samples.size());
    for (const State &sample : samples) {
        tnf_samples.push_back(sample.get_values());
    }
    g_log << "Finished sampling states for iPDB hillclimbing" << endl;

    vector<Pattern> collection = HillClimber(task, size_bound, move(tnf_samples)).run();
    return CanonicalPatternDatabases(task, collection);
}

IPDBHeuristic::IPDBHeuristic(const options::Options &options)
    : Heuristic(options),
      cpdbs(create_cpdbs_by_hillclimbing(task_proxy, options.get<int>("size_bound"))) {
}

int IPDBHeuristic::compute_heuristic(const GlobalState &global_state) {
    TNFState state = global_state.get_values();

    int h = cpdbs.compute_heuristic(state);
    if (h == numeric_limits<int>::max()) {
        return DEAD_END;
    } else {
        return h;
    }
}

static Heuristic *_parse(OptionParser &parser) {
    Heuristic::add_options_to_parser(parser);
    parser.add_option<int>("size_bound");
    Options opts = parser.parse();
    if (parser.dry_run())
        return nullptr;
    else
        return new IPDBHeuristic(opts);
}

static Plugin<Heuristic> _plugin("planopt_ipdb", _parse);

}
