from esphome.components import light
import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.const import (
    CONF_OUTPUT_ID,
    CONF_MIN_VALUE,
    CONF_MAX_VALUE,
    CONF_GAMMA_CORRECT,
    CONF_DEFAULT_TRANSITION_LENGTH,
    CONF_SWITCH_DATAPOINT,
    CONF_COLD_WHITE_COLOR_TEMPERATURE,
    CONF_WARM_WHITE_COLOR_TEMPERATURE,
    CONF_COLOR_INTERLOCK,
)
from .. import xmcosy_ns, CONF_XMCOSY_ID, XmCosy

DEPENDENCIES = ["xmcosy"]

XmCosyLightType = xmcosy_ns.enum("XmCosyLightType")

CONF_COLOR_TYPE = "color_type"
CONF_DATA_POINT_ID = "data_point_id"

COLOR_TYPES = {
    "RGB": XmCosyLightType.RGB,
    "WHITE": XmCosyLightType.WHITE,
}

XmCosyLight = xmcosy_ns.class_("XmCosyLight", light.LightOutput, cg.Component)

COLOR_CONFIG_ERROR = (
    "This option has been removed, use color_datapoint and color_type instead."
)

CONFIG_SCHEMA = cv.All(
    light.BRIGHTNESS_ONLY_LIGHT_SCHEMA.extend(
        {
            cv.GenerateID(CONF_OUTPUT_ID): cv.declare_id(XmCosyLight),
            cv.GenerateID(CONF_XMCOSY_ID): cv.use_id(XmCosy),
            cv.Required(CONF_DATA_POINT_ID): cv.int_,
            cv.Inclusive(CONF_COLOR_TYPE, "color"): cv.enum(COLOR_TYPES, upper=True),
        }
    ).extend(cv.COMPONENT_SCHEMA),
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_OUTPUT_ID])
    await cg.register_component(var, config)
    await light.register_light(var, config)
    

    cg.add(var.set_color_type(config[CONF_COLOR_TYPE]))

    paren = await cg.get_variable(config[CONF_XMCOSY_ID])
    cg.add(var.set_xmcosy_parent(paren))
