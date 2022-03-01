
from contextlib import contextmanager
from pathlib import Path
from tempfile import NamedTemporaryFile


@contextmanager
def temporary_file(contents: str,  **kwargs) -> Path:
    t = NamedTemporaryFile("w", encoding="utf-8", delete=False, **kwargs)
    p = Path(t.name)
    try:
        t.write(contents)
        t.close()
        yield p
    finally:
        p.unlink(missing_ok=True)
