#pragma once

#include "esphome/core/component.h"
#include "esphome/components/tuya/tuya.h"
#include "esphome/components/light/light_state.h"

namespace esphome {
namespace xmcosy {
	struct LightState {
		public:
			float red = 0.0f;
			float green = 0.0f;
			float blue = 0.0f;
			float color_temp = 0.0f;
			float brightness = 0.0f;
	};

	enum XmCosyLightType {
		RGB,
		WHITE,
	};

		
	class XmCosy : public Component{
	 public:
	  void setup() override;
	  void dump_config() override;

	  void set_tuya_parent(tuya::Tuya *tuya);

	  void write_to_tuya(XmCosyLightType type, light::LightState* state);

	  void subscribe_raw_to_data_point(XmCosyLightType type, std::function<void(LightState*)>&& f);

	  void update_light_state(float r, float g, float b, float c, float br) {
		  state_->red = r;
		  state_->blue = b;
		  state_->green = g;
		  state_->color_temp = c;
		  state_->brightness = br;

		  esphome::ESP_LOGD("custom", "Set Internal state: red: %f, green: %f, blue: %f, temp: %f, bri: %f",
				  state_->red,  state_->green, state_->blue, state_->color_temp, state_->brightness);
	  }

	 protected:
	  tuya::Tuya *tuya_{nullptr};
	  LightState *state_{};
	};


}  // namespace xmcosy
}  // namespace esphome
   //
   //
