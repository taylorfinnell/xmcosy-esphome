import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID

from esphome.components.tuya import Tuya, CONF_TUYA_ID

DEPENDENCIES = ["tuya"]

xmcosy_ns = cg.esphome_ns.namespace('xmcosy')

XmCosy = xmcosy_ns.class_('XmCosy', cg.Component)
CONF_XMCOSY_ID = "xmcosy_id"

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(XmCosy),
    cv.GenerateID(CONF_TUYA_ID): cv.use_id(Tuya),
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    paren = await cg.get_variable(config[CONF_TUYA_ID])

    cg.add(var.set_tuya_parent(paren))
