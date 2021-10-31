"""
Logical functions for Kyiv machine. There are 4 such functions:
1) logical shift;
2) logical or;
3) logical and;
4) logical XOR.
"""

from typing import Optional

def shift(number: Optional(bitarray), shift_size: int) -> Optional(bitarray):
    """
    Logical shift of a number on a shift_size.
    If shift_size > 0, left shift is made.
    If shift_size < 0, right shift is made.
    Returns shifted bitarray.
    """
    if shift_size > 0:
        number <<= shift_size
    else:
        number >>= abs(shift_size)
    return number



