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

#ifndef __REPLAY_APP_HPP__
#define __REPLAY_APP_HPP__

#include "ui_widget.hpp"
#include "ui_navigation.hpp"
#include "ui_receiver.hpp"
#include "ui_replay_view.hpp"
#include "ui_spectrum.hpp"

#include <string>
#include <memory>

namespace ui {

class ReplayAppView : public View {
public:
	ReplayAppView(NavigationView& nav);
	~ReplayAppView();

	void on_hide() override;

	void set_parent_rect(const Rect new_parent_rect) override;

	void focus() override;

	std::string title() const override { return "Replay (BETA)"; };

private:
	NavigationView& nav_;
	
	bool file_error { false };
	
	static constexpr ui::Dim header_height = 2 * 16;

	void on_target_frequency_changed(rf::Frequency f);

	rf::Frequency target_frequency() const;
	void set_target_frequency(const rf::Frequency new_value);

	FrequencyField field_frequency {
		{ 0 * 8, 0 * 16 },
	};
	
	FrequencyStepView field_frequency_step {
		{ 10 * 8, 0 * 16 },
	};

	RFAmpField field_rf_amp {
		{ 16 * 8, 0 * 16 }
	};

	ReplayView replay_view {
		{ 0 * 8, 1 * 16, 30 * 8, 1 * 16 },
		16384, 3
	};

	spectrum::WaterfallWidget waterfall { };
};

} /* namespace ui */

#endif/*__REPLAY_APP_HPP__*/
