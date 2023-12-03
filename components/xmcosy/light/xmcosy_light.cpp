#include "xmcosy_light.h"

namespace esphome {
namespace xmcosy {

	void XmCosyLight::setup()
    {
        this->parent_->subscribe_raw_to_data_point(this->color_type_, [=](LightState* state) {
			on_light_state(state);
        });
    }

	void XmCosyLight::set_color_type(XmCosyLightType color_type) {
		color_type_ = color_type;
	}

	light::LightTraits XmCosyLight::get_traits() {
		auto traits = light::LightTraits();

		if(color_type_ == XmCosyLightType::WHITE) {
			traits.set_supported_color_modes({light::ColorMode::COLOR_TEMPERATURE});
			traits.set_max_mireds(370);
			traits.set_min_mireds(166);
		} else {
			traits.set_supported_color_modes({light::ColorMode::RGB});
		}

		return traits;
	}

	void XmCosyLight::on_light_state(LightState *state) {
		if(color_type_ == XmCosyLightType::WHITE) {
			return;
		} else {
			float current_red, current_green, current_blue;
			this->light_state_->current_values_as_rgb(&current_red, &current_green, &current_blue);

			if (int(state->red) != int(current_red) || int(state->green) != (int)current_green || (int)state->blue != (int)current_blue) {
				ESP_LOGD("custom", "Setting RGB");
				auto rgb_call = this->light_state_->make_call();
				rgb_call.set_rgb(state->red, state->green, state->blue);
				rgb_call.perform();
			}
		}
	}

	void XmCosyLight::setup_state(light::LightState *light_state) {
		this->light_state_ = light_state;
		this->light_state_->set_default_transition_length(0);
	}

	void XmCosyLight::write_state(light::LightState *state) {
		if (state->current_values != state->remote_values) {
			ESP_LOGD("custom", "Light is transitioning, datapoint change ignored");
			return;
		}

		ESP_LOGD("custom", "Writing to tuya");


		this->parent_->write_to_tuya(this->color_type_, state);
	}
}
}
