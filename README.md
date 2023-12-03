# xmcosy-esphome


A somewhat working component for the [XMCOSY+ Floor Lamp, 2400LM Smart RGBW LED Standing Lamp with Modern Double-Side Lighting](https://www.amazon.com/dp/B09QYFWJ61)

I flashed this device by desoldering the chip (WB2S), attching to the correct pins, and used `ltchiptool` to flash. Then I soldered
the chip back. Sadly I messed up the stock firmware before I could get a dump. The PCB is in the lamp head and is pretty annoying to get to.

The lamp comes with two lights, an RGB light and a white light. Through the Tuya app you can control both lights individually
and they can both be on at the same time. Neither the Tuya local, Tuya, or smartlife integration seemed to give me this 
same control. In the aforementioned integrations only one light could be on at a time.

This component exposes two lights allowing them to be controlled individually and can both be on at the same time.

This Tuya device has a datapoint that (AFAIK) is not well documented. The data point is called `mix_rgbcw` and contains
the `HSVBT` (hue, saturation, value, brightess, white temp) values (each 2 bytes)


It can get out of sync if the light is controlled manually and has some other issues, but it works well enough to use I think.


```yaml
# example configuration:

tuya:

uart:
  baud_rate: 9600
  tx_pin: TX1
  rx_pin: RX1

button:
  - platform: restart
    name: "Restart"

switch:
  - platform: tuya
    switch_datapoint: 20
    name: Power

xmcosy:

light:
  - platform: xmcosy
    name: RGB
    color_type: RGB
    data_point_id: 51

  - platform: xmcosy
    name: White
    color_type: WHITE
    data_point_id: 51


```
