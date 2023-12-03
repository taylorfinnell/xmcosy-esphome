#include "esphome/core/log.h"
#include "xmcosy.h"

std::vector<uint8_t> HexToBytes(const std::string& hex) {
  std::vector<uint8_t> bytes{0, 7};

  for (unsigned int i = 0; i < hex.length(); i += 2) {
    std::string byteString = hex.substr(i, 2);
    uint8_t byte = (uint8_t) strtol(byteString.c_str(), NULL, 16);
    bytes.push_back(byte);
  }

  return bytes;
}

void convert_rgbct_to_tuya(float red, float green, float blue, float color_temp, float brightness, 
		int *tuya_hue, int *tuya_saturation, int *tuya_value, int *tuya_color_temp, int *tuya_brightness) {
	int hue;
	float saturation, value;

	esphome::rgb_to_hsv(red * 255.0f, green * 255.0f, blue * 255.0f, hue, saturation, value);

	*tuya_hue = (int)hue;
	*tuya_saturation = (int)(saturation * 1000.0f);
	*tuya_value = (int)((value/255.0f) * 1000.0f);
	*tuya_color_temp = (int)(color_temp * 1000.0f);
	*tuya_brightness = (int)(brightness * 1000.0f);
}

void convert_tuya_to_rgct(std::vector<uint8_t> bytes, float *red, float *green, float *blue, float *color_temp, float *brightness) {
	bytes.erase(bytes.begin(), bytes.begin() + 2);
	esphome::ESP_LOGD("custom", "Got sent %d bytes", bytes.size());

	int tuya_hue = (bytes[0] << 8) | bytes[1];
	int tuya_saturation = (bytes[2] << 8) | bytes[3];
	int tuya_value = (bytes[4] << 8) | bytes[5];
	int tuya_brightness = (bytes[6] << 8) | bytes[7];
	int tuya_color_temp = (bytes[8] << 8) | bytes[9];

	float real_hue = tuya_hue;
	float real_saturation = tuya_saturation / 1000.0f;
	float real_value = (tuya_value / 1000.0f) * 255.0f;
	float real_brightness = tuya_brightness / 1000.0f;
	float real_color_temp = tuya_color_temp / 1000.0f;

	float r, g, b;
	esphome::hsv_to_rgb(real_hue, real_saturation, real_value, r, g, b);

	*red = r / 255.0f;
	*green = g / 255.0f;
	*blue = b / 255.0f;
	*color_temp = real_color_temp / 255.0f;
	*brightness = real_brightness / 255.0f;

	esphome::ESP_LOGD("custom", "Got the following after final conversion: red: %f, green: %f, blue: %f, temp: %f, bri: %f",
			*red, *green, *blue, *color_temp, *brightness);
}


namespace esphome {
namespace xmcosy {
	void XmCosy::setup() {
		this->state_ = new LightState();
	}

	void XmCosy::set_tuya_parent(tuya::Tuya *tuya) { 
		this->tuya_ = tuya;
	}

	void XmCosy::subscribe_raw_to_data_point(XmCosyLightType type, std::function<void(LightState*)>&& f) {
		tuya_->register_listener(51, [=](const esphome::tuya::TuyaDatapoint &datapoint) {
				auto datapoint_value = datapoint.value_raw;

				float red, green, blue, color_temp, brightness;
				convert_tuya_to_rgct(datapoint_value, &red, &green, &blue, &color_temp, &brightness);

				if(type == XmCosyLightType::WHITE) {
					red = state_->red;
					green = state_->green;
					blue = state_->blue;
				} else {
					color_temp = state_->color_temp;
					brightness = state_->brightness;
				}

				update_light_state(red, green, blue, color_temp, brightness);

				f(state_);
		});
	}

	void XmCosy::dump_config(){
	}

	void XmCosy::write_to_tuya(XmCosyLightType type, light::LightState* state){
		if (state->current_values != state->remote_values) {
			ESP_LOGD("custom", "Light is transitioning, datapoint change ignored");
			return;
		}

		float red = 0.0f, green = 0.0f, blue = 0.0f, brigthness = 0.0f, temp = 0.0f;

		if(type == XmCosyLightType::WHITE) {
			state->current_values_as_ct (&temp, &brigthness);
			red = state_->red;
			green = state_->green;
			blue = state_->blue;
		} else {
			state->current_values_as_rgb(&red, &green, &blue);
			temp = state_->color_temp;
			brigthness = state_->brightness;
		}

		int tuya_hue, tuya_saturation, tuya_value, tuya_color_temp, tuya_brightness;
		convert_rgbct_to_tuya(red, green, blue, temp, brigthness, &tuya_hue, &tuya_saturation, &tuya_value, &tuya_color_temp, &tuya_brightness);

		char hex[13];
		sprintf(hex, "%04X%04X%04X%04X%04X", tuya_hue, tuya_saturation, tuya_value, tuya_brightness, tuya_color_temp);
		std::string in(hex);
		std::vector<uint8_t> out = HexToBytes(in);

		update_light_state(red, green, blue, temp, brigthness);

		tuya_->set_raw_datapoint_value(51, out);
	}
}
}
