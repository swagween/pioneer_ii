#include "TextWriter.hpp"
#include <SFML/Graphics.hpp>
#include <string>
#include "../service/ServiceProvider.hpp"

namespace text {

TextWriter::TextWriter(automa::ServiceProvider& svc) : m_services(&svc) {
	font.loadFromFile(svc.text.text_font);
	font.setSmooth(false);
	special_characters.insert({Codes::prompt, '%'});
	special_characters.insert({Codes::quest, '$'});
	special_characters.insert({Codes::item, '^'});
	special_characters.insert({Codes::voice, '&'});
	special_characters.insert({Codes::emotion, '@'});
	special_characters.insert({Codes::hash, '#'});
	help_marker.set_color(svc.styles.colors.ui_white);
	help_marker.set_alpha(0);
}

void TextWriter::start() {

	// to be replaced with something prettier later (maybe)
	indicator.setSize({4.f, 4.f});
	indicator.setOrigin({2.f, 2.f});
	indicator.setFillColor(m_services->styles.colors.bright_orange);

	if (iterators.current_suite_set >= suite.size()) { return; }
	if (suite.at(iterators.current_suite_set).empty()) { return; }

	working_message = suite.at(iterators.current_suite_set).front().data;
	// calculate number of lines and call wrap() that many times.
	// can't call wrap() tick-wise because it's very slow
	auto num_glyphs = suite.at(iterators.current_suite_set).front().data.getString().getSize();
	auto length = suite.at(iterators.current_suite_set).front().data.getCharacterSize() + suite.at(iterators.current_suite_set).front().data.getLineSpacing();
	auto gpl = bounds.x / length;
	auto num_lines = num_glyphs / gpl;
	for (int i = 0; i < num_lines; ++i) { wrap(); }

	check_for_event(suite.at(iterators.current_suite_set).front(), Codes::prompt);
	check_for_event(suite.at(iterators.current_suite_set).front(), Codes::voice);
	check_for_event(suite.at(iterators.current_suite_set).front(), Codes::emotion);
	check_for_event(suite.at(iterators.current_suite_set).front(), Codes::item);
	check_for_event(suite.at(iterators.current_suite_set).front(), Codes::quest);

	activate();
}

void TextWriter::update() {

	delay.update();
	if (flags.test(MessageState::done_writing) && !flags.test(MessageState::started_delay)) {
		delay.start();
		flags.set(MessageState::started_delay);
	}

	if (iterators.current_suite_set >= suite.size()) { return; }
	if (suite.at(iterators.current_suite_set).empty()) { shutdown(); }
	if (!writing()) { return; }

	if (tick_count % writing_speed == 0) {
		char const next_char = (char)suite.at(iterators.current_suite_set).front().data.getString().getData()[glyph_count];
		working_str += next_char;
		working_message.setString(working_str);
		++glyph_count;
	}
	if (glyph_count >= suite.at(iterators.current_suite_set).front().data.getString().getSize()) {
		reset();
		deactivate();
	}
	++tick_count;
}

void TextWriter::set_position(sf::Vector2<float> pos) { position = pos; }

void TextWriter::set_bounds(sf::Vector2<float> new_bounds) { bounds = new_bounds; }

void TextWriter::wrap() {

	if (iterators.current_suite_set >= suite.size()) { return; }
	if (suite.at(iterators.current_suite_set).empty()) { return; }

	auto horizontal_extent = position.x + suite.at(iterators.current_suite_set).front().data.getLocalBounds().width;
	if (horizontal_extent > bounds.x) {

		// get index of last in-bounds space
		int last_space_index{};
		for (auto i{0}; i < suite.at(iterators.current_suite_set).front().data.getString().getSize(); ++i) {
			char const current_char = (char)suite.at(iterators.current_suite_set).front().data.getString().getData()[i];
			if (std::isspace(current_char)) {
				if (suite.at(iterators.current_suite_set).front().data.findCharacterPos(i).x < bounds.x) {
					last_space_index = i;
				} else {
					// splice!
					std::string left = suite.at(iterators.current_suite_set).front().data.getString().substring(0, last_space_index);
					std::string right = suite.at(iterators.current_suite_set).front().data.getString().substring(last_space_index + 1, suite.at(iterators.current_suite_set).front().data.getString().getSize() - 1);
					left += '\n';

					suite.at(iterators.current_suite_set).front().data.setString(left + right);
					return;
				}
			}
		}
	}
}

void TextWriter::load_single_message(std::string_view message) {
	suite.clear();
	responses.clear();
	auto message_container = std::deque<Message>{};
	message_container.push_back({sf::Text(), false});
	message_container.back().data.setString(message.data());
	stylize(message_container.back().data, true);
	suite.push_back(message_container);
}

void TextWriter::load_message(dj::Json& source, std::string_view key) {
	suite.clear();
	responses.clear();

	// suite
	for (auto& set : source[key]["suite"].array_view()) {
		auto this_set = std::deque<Message>{};
		for (auto& msg : set.array_view()) {
			this_set.push_back({sf::Text(), false});
			this_set.back().data.setString(msg.as_string().data());
			stylize(this_set.back().data, true);
		}
		suite.push_back(this_set);
	}
	// responses
	for (auto& set : source[key]["responses"].array_view()) {
		auto this_set = std::deque<Message>{};
		for (auto& msg : set.array_view()) {
			this_set.push_back({sf::Text(), false});
			this_set.back().data.setString(msg.as_string().data());
			stylize(this_set.back().data, false);
		}
		responses.push_back(this_set);
	}
	working_message = suite.at(iterators.current_suite_set).front().data;
	help_marker.init(*m_services, "Press [", "main_action", "] to continue.");
}

void TextWriter::append(std::string_view content) {
	if (suite.empty()) { return; }
	if (suite.back().empty()) { return; }
	auto msg = suite.back().back().data.getString();
	suite.back().back().data.setString(msg + content.data());
}

void TextWriter::stylize(sf::Text& msg, bool is_suite) const {
	msg.setCharacterSize(text_size);
	msg.setFillColor(m_services->styles.colors.ui_white);
	msg.setFont(font);
	msg.setLineSpacing(1.5f);
	if (is_suite) {
		msg.setPosition(position);
	} else {
		msg.setPosition(response_position);
	}
}
	void TextWriter::write_instant_message(sf::RenderWindow& win) {
	if (iterators.current_suite_set >= suite.size()) { return; }
	if (suite.at(iterators.current_suite_set).empty()) { return; }
	win.draw(suite.at(iterators.current_suite_set).front().data);
}

void TextWriter::write_gradual_message(sf::RenderWindow& win) {
	if (iterators.current_suite_set >= suite.size()) { return; }
	if (suite.at(iterators.current_suite_set).empty()) { return; }
	if (!writing()) {
		win.draw(suite.at(iterators.current_suite_set).front().data);
		if (!selection_mode()) { help_marker.render(win); }
		flags.set(MessageState::done_writing);
		return;
	}
	help_marker.start();
	win.draw(working_message);
}

void TextWriter::write_responses(sf::RenderWindow& win) {
	if (iterators.current_response_set >= responses.size()) { return; }
	if (selection_mode()) {
		sf::Vector2<float> newpos{position.x + response_offset.x, position.y + response_offset.y};
		for (auto& msg : responses.at(iterators.current_response_set)) {
			msg.data.setPosition(newpos);
			win.draw(msg.data);
			newpos.y += pad; // provisional
		}
		indicator.setPosition(position.x + response_offset.x - pad / 2, position.y + pad / 3 + pad * iterators.current_selection + response_offset.y);
		win.draw(indicator);
	}
}

void TextWriter::reset() {
	writing_speed = default_writing_speed;
	help_marker.reset();
	glyph_count = 0;
	tick_count = 0;
	working_message = {};
	working_str = {};
	iterators.current_selection = 0;
}

void TextWriter::skip_ahead() { writing_speed = fast_writing_speed; }

void TextWriter::enable_skip() { flags.reset(MessageState::cannot_skip); }

void TextWriter::activate() { flags.set(MessageState::writing); }

void TextWriter::deactivate() { flags.reset(MessageState::writing); }

void TextWriter::request_next() {
	if (writing()) { return; }
	if (suite.empty() || iterators.current_suite_set >= suite.size()) {
		reset();
		return;
	}
	if (suite.at(iterators.current_suite_set).empty()) {
		reset();
		return;
	}
	if (suite.at(iterators.current_suite_set).front().prompt) {
		flags.set(MessageState::selection_mode);// check for response events
		if (iterators.current_response_set >= responses.size()) { return; }
		for (auto& res : responses.at(iterators.current_response_set)) {
			check_for_event(res, Codes::item, true);
			check_for_event(res, Codes::quest, true);
			check_for_event(res, Codes::prompt, true);
		}
		return;
	} else {
		suite.at(iterators.current_suite_set).pop_front();
		if (suite.at(iterators.current_suite_set).empty()) {
			shutdown();
			return;
		}
		reset();
		activate();
		start();
	}
}

void TextWriter::check_for_event(Message& msg, Codes code, bool response) {
	auto index = msg.data.getString().find(special_characters.at(code));
	if (index == std::string::npos) { return; }

	// handle prompts
	if (code == Codes::prompt) {
		if (index < msg.data.getString().getSize() - 1) {
			msg.target = (int)msg.data.getString().getData()[index + 1] - '0';
			msg.data.setString(msg.data.getString().substring(0, index));
			flags.set(MessageState::response_trigger);
			msg.prompt = true;
		}
		return;
	}
	// handle item and quest out_keys
	if (code == Codes::voice) {
		std::string cue = msg.data.getString().substring(index + 1, index + 1);
		communicators.out_voice.set(std::stoi(cue));
		msg.data.setString(msg.data.getString().substring(index + 2, msg.data.getString().getSize() - 1));
		return;
	}
	if (code == Codes::emotion) {
		std::string cue = msg.data.getString().substring(index + 1, index + 1);
		communicators.out_emotion.set(std::stoi(cue));
		msg.data.setString(msg.data.getString().substring(index + 2, msg.data.getString().getSize() - 1));
		return;
	}
	if (code == Codes::item) {
		std::string cue = msg.data.getString().substring(index + 1, index + 1);
		communicators.out_item.set(std::stoi(cue));
		msg.data.setString(msg.data.getString().substring(0, index));
		return;
	}
	if (code == Codes::quest) {
		std::string cue = msg.data.getString().substring(index + 1, index + 1);
		communicators.out_quest.set(std::stoi(cue));

		std::string hash = msg.data.getString().substring(index + 1, msg.data.getString().getSize() - 1);
		//std::cout << "Quest key read: " << hash << "\n";

		auto push = decoder.decode(hash, '#');
		if (push.size() == 3) { out_quest = util::QuestKey{push[0], push[1], push[2]}; }
		if (push.size() == 4) { out_quest = util::QuestKey{push[0], push[1], push[2], push[3]}; } // progression amount provided
		if (push.size() == 5) { out_quest = util::QuestKey{push[0], push[1], push[2], push[3], push[4]}; } // hard set provided
		//std::cout << "Decoded: " << out_quest.type << ", " << out_quest.id << ", " << out_quest.source_id << ", " << out_quest.amount << ", " << out_quest.hard_set << "\n";
		if(response) {
		} else {
			process_quest();
		}

		msg.data.setString(msg.data.getString().substring(0, index));
		if (out_quest.type == 63) { msg.data.setString(msg.data.getString() + std::to_string(m_services->stats.time_trials.bryns_gun)); } // fetch text
		if (!response) { out_quest = {}; }

		return;
	}
}

void TextWriter::adjust_selection(int amount) {
	if (!selection_mode()) { return; }
	if (iterators.current_response_set >= responses.size()) { return; }
	iterators.current_selection += amount;
	m_services->soundboard.flags.console.set(audio::Console::shift);
	if (iterators.current_selection < 0) { iterators.current_selection = static_cast<int>(responses.at(iterators.current_response_set).size() - 1); }
	if (iterators.current_selection >= responses.at(iterators.current_response_set).size()) { iterators.current_selection = 0; }
}

void TextWriter::process_selection() {
	if (!selection_mode()) { return; }
	if (iterators.current_response_set >= responses.size()) { return; }
	if (iterators.current_selection >= responses.at(iterators.current_response_set).size()) { return; }

	// check for response events
	auto ctr{0};
	for (auto& res : responses.at(iterators.current_response_set)) {
		if (ctr == get_current_selection()) {
			process_quest();
			out_quest = {};
		}
		++ctr;
	}
	// flush the suite until we reach the target determined by the selection
	for (auto i = 0; i <= responses.at(iterators.current_response_set).at(iterators.current_selection).target; ++i) {
		if (suite.empty()) {
			return;
		}
		suite.pop_front();
		if (suite.empty()) {
			m_services->soundboard.flags.console.set(audio::Console::done);
			shutdown();
			return;
		}
	}
	responses.pop_front();

	m_services->soundboard.flags.console.set(audio::Console::next);
	flags.set(MessageState::cannot_skip);
	flags.reset(MessageState::selection_mode);
	reset();
	activate();
	start();
}

void TextWriter::process_quest() {
	m_services->quest.process(out_quest);
	m_services->data.push_quest(out_quest);
	if (out_quest.type == 27) { m_services->state_controller.actions.set(automa::Actions::retry); }
	if (out_quest.type == 33) { communicators.reveal_item.set(out_quest.id); }
	if (out_quest.type == 88) { m_services->state_controller.actions.set(automa::Actions::console_transition); }
	if (out_quest.type == 89) { m_services->state_controller.actions.set(automa::Actions::main_menu); }
	if (out_quest.type == 69) { m_services->state_controller.actions.set(automa::Actions::print_stats); }
	//std::cout << "Processed: " << out_quest.type << ", " << out_quest.id << ", " << out_quest.source_id << ", " << out_quest.amount << ", " << out_quest.hard_set << "\n";
}

void TextWriter::shutdown() {
	reset();
	suite.clear();
	responses.clear();
	flags = {};
	iterators = {};
}

Message& TextWriter::current_message() {
	if (iterators.current_suite_set >= suite.size()) { return zero_option; }
	if (suite.at(iterators.current_suite_set).empty()) { return zero_option; }
	return suite.at(iterators.current_suite_set).front();
}

Message& TextWriter::current_response() {
	if (iterators.current_response_set >= responses.size()) { return zero_option; }
	if (responses.at(iterators.current_response_set).empty()) { return zero_option; }
	return responses.at(iterators.current_response_set).front();
}

int TextWriter::get_current_selection() const { return iterators.current_selection; }

int TextWriter::get_current_suite_set() const { return iterators.current_suite_set; }

} // namespace text
