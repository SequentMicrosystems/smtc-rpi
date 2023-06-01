import smbus2

__version__ = "1.0.1"
_CARD_BASE_ADDRESS = 0x16
_STACK_LEVEL_MAX = 7
_IN_CH_COUNT = 8
_TEMP_SIZE_BYTES = 2
_TEMP_SCALE_FACTOR = 10.0

_TCP_VAL1_ADD = 0
_TCP_TYPE1_ADD = 16
_REVISION_HW_MAJOR_MEM_ADD = 47
_REVISION_HW_MINOR_MEM_ADD = 48

_TC_TYPE_B = 0
_TC_TYPE_E = 1
_TC_TYPE_J = 2
_TC_TYPE_K = 3
_TC_TYPE_N = 4
_TC_TYPE_R = 5
_TC_TYPE_S = 6
_TC_TYPE_T = 7

_TC_TYPES = ['B', 'E', 'J', 'K', 'N', 'R', 'S', 'T']

class SMtc:
    def __init__(self, stack = 0, i2c = 1):
        if stack < 0 or stack > _STACK_LEVEL_MAX:
            raise ValueError('Invalid stack level!')
        self._hw_address_ = _CARD_BASE_ADDRESS + stack
        self._i2c_bus_no = i2c
        bus = smbus2.SMBus(self._i2c_bus_no)
        try:
            self._card_rev_major = bus.read_byte_data(self._hw_address_, _REVISION_HW_MAJOR_MEM_ADD)
            self._card_rev_minor = bus.read_byte_data(self._hw_address_, _REVISION_HW_MINOR_MEM_ADD)
        except Exception as e:
            bus.close()
            raise Exception("Fail to read with exception " + str(e))
        bus.close()

    def set_sensor_type(self, channel, cfg):
        if channel < 1 or channel > _IN_CH_COUNT:
            raise ValueError('Invalid input channel number number must be [1..8]!')
        if cfg < _TC_TYPE_B or cfg > _TC_TYPE_T:
            raise ValueError('Invalid thermocouple type, must be [0..7]!')
        bus = smbus2.SMBus(self._i2c_bus_no)
        try:
            bus.write_byte_data(self._hw_address_, _TCP_TYPE1_ADD + channel - 1, cfg)
        except Exception as e:
            bus.close()
            raise Exception("Fail to read with exception " + str(e))
        bus.close()

    def get_sensor_type(self, channel):
        if channel < 1 or channel > _IN_CH_COUNT:
            raise ValueError('Invalid input channel number number must be [1..8]!')
        bus = smbus2.SMBus(self._i2c_bus_no)
        try:
            val = bus.read_byte_data(self._hw_address_, _TCP_TYPE1_ADD + channel - 1)
        except Exception as e:
            bus.close()
            raise Exception("Fail to read with exception " + str(e))
        bus.close()
        return val

    def get_temp(self, channel):
        if channel < 1 or channel > _IN_CH_COUNT:
            raise ValueError('Invalid input channel number number must be [1..8]!')
        bus = smbus2.SMBus(self._i2c_bus_no)
        try:
            val = bus.read_word_data(self._hw_address_, _TCP_VAL1_ADD + (channel - 1) * _TEMP_SIZE_BYTES)
        except Exception as e:
            bus.close()
            raise Exception("Fail to read with exception " + str(e))
        bus.close()
        return val / _TEMP_SCALE_FACTOR

    def print_sensor_type(self, channel):
        print(_TC_TYPES[self.get_sensor_type(channel)])