"""Frontend for hand-authored microinstruction packets, independent of any ISA."""
from ..base import Packet

from .settings import FrontendSettings

class PacketFrontend:
    def __init__(self, packets, base=0, instruction_alignment=2, settings=None):
        self.settings = settings or FrontendSettings(base=base)
        base = self.settings.base
        if instruction_alignment < 1: raise ValueError("invalid alignment")
        self.base = base
        self.instruction_alignment = instruction_alignment
        self.packets = {}
        self.end = base
        for packet in packets:
            if packet.pc != self.end or packet.next_pc <= packet.pc:
                raise ValueError('packets must be contiguous and forward-sized')
            self.packets[packet.pc] = packet
            self.end = packet.next_pc

    def packet(self, pc):
        if pc == self.end:
            return None
        return self.packets[pc]
