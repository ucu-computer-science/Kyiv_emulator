from bitarray import bitarray

class Registers:
    """Registers of Kyiv machine"""
    def __init__(self):
        self.address_r = bitarray('0' * 10, endian='little')
        self.address_r[-1] = 1
        print(self.address_r.to01())
        self.return_r = bitarray('0' * 11, endian='little')
        self.cycle_r = bitarray('0' * 11)
        self.com_counter = bitarray('0' * 11)
        self.command_r = bitarray('0' * 41)


r = Registers()
