/*
 * Copyright (C) 2016 Jared Boone, ShareBrained Technology, Inc.
 * Copyright (C) 2016 Furrtek
 *
 * This file is part of PortaPack.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#include "replay_app.hpp"
#include "string_format.hpp"

#include "baseband_api.hpp"
#include "portapack.hpp"
#include "portapack_persistent_memory.hpp"

using namespace portapack;

namespace ui {

ReplayAppView::ReplayAppView(
	NavigationView& nav
) : nav_ (nav)
{
	std::vector<std::filesystem::path> file_list;
	
	// Search for files with the right extension
	file_list = scan_root_files(u"/", u"*.C16");
	if (!file_list.size()) {
		file_error = true;
		return;
	}
	
	baseband::run_image(portapack::spi_flash::image_tag_replay);

	add_children({
		&field_frequency,
		&field_frequency_step,
		&field_rf_amp,
		&replay_view,
		&waterfall,
	});
	
	replay_view.set_file_list(file_list);
	
	field_frequency.set_value(target_frequency());
	field_frequency.set_step(receiver_model.frequency_step());
	field_frequency.on_change = [this](rf::Frequency f) {
		this->on_target_frequency_changed(f);
	};
	field_frequency.on_edit = [this, &nav]() {
		// TODO: Provide separate modal method/scheme?
		auto new_view = nav.push<FrequencyKeypadView>(this->target_frequency());
		new_view->on_changed = [this](rf::Frequency f) {
			this->on_target_frequency_changed(f);
			this->field_frequency.set_value(f);
		};
	};

	field_frequency_step.set_by_value(receiver_model.frequency_step());
	field_frequency_step.on_change = [this](size_t, OptionsField::value_t v) {
		receiver_model.set_frequency_step(v);
		this->field_frequency.set_step(v);
	};

	replay_view.on_error = [&nav](std::string message) {
		nav.display_modal("Error", message);
	};
}

ReplayAppView::~ReplayAppView() {
	radio::disable();
	baseband::shutdown();
}

void ReplayAppView::on_hide() {
	// TODO: Terrible kludge because widget system doesn't notify Waterfall that
	// it's being shown or hidden.
	waterfall.on_hide();
	View::on_hide();
}

void ReplayAppView::set_parent_rect(const Rect new_parent_rect) {
	View::set_parent_rect(new_parent_rect);

	const ui::Rect waterfall_rect { 0, header_height, new_parent_rect.width(), new_parent_rect.height() - header_height };
	waterfall.set_parent_rect(waterfall_rect);
}

void ReplayAppView::focus() {
	if (!file_error) {
		field_frequency.focus();
	} else
		nav_.display_modal("No files", "No .C16 files in\nSD card root", ABORT, nullptr);
}

void ReplayAppView::on_target_frequency_changed(rf::Frequency f) {
	set_target_frequency(f);
}

void ReplayAppView::set_target_frequency(const rf::Frequency new_value) {
	persistent_memory::set_tuned_frequency(new_value);;
}

rf::Frequency ReplayAppView::target_frequency() const {
	return persistent_memory::tuned_frequency();
}

} /* namespace ui */
