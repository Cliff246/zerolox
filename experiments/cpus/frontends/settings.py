from dataclasses import dataclass

@dataclass(frozen=True)
class FrontendSettings:
    base: int = 0
    packets_per_cycle: int = 1

    def __post_init__(self):
        if self.base < 0 or self.packets_per_cycle < 1:
            raise ValueError('invalid frontend configuration')
