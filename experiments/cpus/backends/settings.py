from dataclasses import dataclass

@dataclass(frozen=True)
class InOrderSettings:
    issue_width: int = 1
    buffer_size: int = 8
    memory_size: int = 65536
    forward: bool = True

    def __post_init__(self):
        if self.issue_width not in (1, 2) or self.buffer_size < 1 or self.memory_size < 1:
            raise ValueError('invalid backend configuration')
