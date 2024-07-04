#include "Inventory.hpp"
#include "../../gui/Console.hpp"
#include "../../service/ServiceProvider.hpp"

namespace player {

Inventory::Inventory() {
	item_labels.insert({1, "bryns_notebook"});
	item_labels.insert({2, "factory_access_permit"});
	item_labels.insert({3, "bipolar_junction_transistor"});
	item_labels.insert({4, "willets_floppy_disc"});
	item_labels.insert({5, "track_zero_sensor"});
	item_labels.insert({6, "head_motor"});
	item_labels.insert({7, "gas_mask"});
	item_labels.insert({8, "firstwind_hangar_key"});
	item_labels.insert({9, "woodshine_warehouse_key"});
	item_labels.insert({10, "nimbus_iii_boiler_room_key"});
	item_labels.insert({11, "laboratory_key"});
	item_labels.insert({12, "bit_cell_key"});
	item_labels.insert({13, "four_of_diamonds"});
}

void Inventory::update(automa::ServiceProvider& svc) {
	int index{};
	for (auto& item : items) {
		item.update(svc, index);
		++index;
	}
}

void Inventory::add_item(automa::ServiceProvider& svc, int item_id, int amount) {
	if (item_id == 0) { return; }
	bool found{};
	for (auto& item : items) {
		if (item.get_id() == item_id) {
			item.add_item(amount);
			found = true;
		}
	}
	if (!found) {
		items.push_back(item::Item(svc, item_labels.at(item_id)));
		items.back().set_id(item_id);
		items.back().add_item(amount);
	}
	update(svc);
}

void Inventory::reveal_item(int item_id) {
	for (auto& item : items) {
		if (item.get_id() == item_id) { item.reveal(); }
	}
}

void Inventory::clear() { items.clear(); }

bool Inventory::has_item(int id) const {
	auto ret{false};
	for (auto& item : items) {
		if (item.get_id() == id) { ret = true; }
	}
	return ret;
}

} // namespace player
