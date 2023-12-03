#pragma once

#include "esphome.h"
#include "../xmcosy.h"

namespace esphome {
namespace xmcosy {

	class XmCosyLight : public Component, public light::LightOutput {
		public:

		void setup() override;

		void on_light_state(LightState *state);

		void setup_state(light::LightState *light_state) override;

		light::LightTraits get_traits() override;

		void set_color_type(XmCosyLightType color_type);

		void write_state(light::LightState *state) override;

		void set_xmcosy_parent(XmCosy* parent) { parent_ = parent; }

		protected:
		light::LightState *light_state_{nullptr};
		XmCosyLightType color_type_{};
		XmCosy* parent_;
		bool has_initial_state_{false};
		bool color_temperature_invert_{true};
	};
}
}
