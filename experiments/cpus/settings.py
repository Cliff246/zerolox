"""Named machine configurations. Reference profiles, not hardware claims."""
from dataclasses import dataclass
from typing import Callable
from .base import Frontend, Backend
from .backends.in_order import InOrderBackend
from .backends.settings import InOrderSettings
from .frontends.zerolox import ZeroloxFrontend
from .frontends.rv32i import RV32IFrontend
from .frontends.settings import FrontendSettings

@dataclass(frozen=True)
class MachineSettings:
    frontend: Callable[..., Frontend]
    backend: Callable[..., Backend]
    backend_settings: object
    frontend_settings: FrontendSettings = FrontendSettings()

    def build(self, code, **frontend_settings):
        from dataclasses import replace
        config = frontend_settings.pop("settings", self.frontend_settings)
        if "base" in frontend_settings:
            config = replace(config, base=frontend_settings.pop("base"))
        return self.backend(self.frontend(code, settings=config, **frontend_settings), self.backend_settings)

ZLX_SCALAR = MachineSettings(ZeroloxFrontend, InOrderBackend, InOrderSettings())
ZLX_DUAL = MachineSettings(ZeroloxFrontend, InOrderBackend, InOrderSettings(issue_width=2))

RV32_SCALAR = MachineSettings(RV32IFrontend, InOrderBackend, InOrderSettings())
RV32_DUAL = MachineSettings(RV32IFrontend, InOrderBackend, InOrderSettings(issue_width=2))
