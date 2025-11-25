"""WARNING -- FOR CI ONLY

This script is meant to be execute in CI only.
Running this locally will alter the Doxyfile.
Such changes SHOULD NOT be pushed to the remote.
"""

from pathlib import Path

DOCS_DIR = Path(__file__).parent.parent / "docs"
PROPERTIES = DOCS_DIR.parent / "library.properties"
CONFIG = DOCS_DIR / "Doxyfile"
TMP = DOCS_DIR / "doxygenAction"


def overwrite_doxygen_value():
    properties = PROPERTIES.read_text(encoding="utf-8").splitlines()
    for line in properties:
        if line.startswith("version="):
            _, value = line.split("=", 1)
            version = value.strip()
            break
    else:
        raise RuntimeError("version not found in library.properties")
    config = CONFIG.read_text(encoding="utf-8")
    TMP.write_text(f"PROJECT_NUMBER = {version}\n", encoding="utf-8")
    config += f"\n@INCLUDE = {str(TMP)}\n"
    CONFIG.write_text(config, encoding="utf-8")


if __name__ == "__main__":
    overwrite_doxygen_value()
