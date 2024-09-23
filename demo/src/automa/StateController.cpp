
#include "StateController.hpp"
#include "../graphics/Transition.hpp"

namespace automa {
void StateController::switch_rooms(int source, int destination, flfx::Transition& transition) {
	if (!transition.is_done()) { return; }
	source_id = source;
	next_state = destination;
	actions.set(Actions::trigger);
}

} // namespace automa
