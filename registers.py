from bitarray import bitarray

class Registers:
    """Registers of Kyiv machine"""
    def __init__(self):
        self.address_r = bitarray('0' * 10)
        self.return_r = bitarray('0' * 11)
        self.cycle_r = bitarray('0' * 11)
        self.com_counter = bitarray('0' * 11)
        self.command_r = bitarray('0' * 41)
