#!/usr/bin/env python3
"""Print only the current release's changelog section."""
from pathlib import Path
import json
import re

root=Path(__file__).resolve().parents[1]
data=json.loads((root/'catalog/catalog.json').read_text(encoding='utf-8'))
text=(root/'CHANGELOG.md').read_text(encoding='utf-8')
sections=re.split(r'(?m)(?=^## )',text)
expected=f'## 脚手架 v{data["version"]} / 手册 v{data["handbook_version"]}'
section=next((part.strip() for part in sections if part.startswith(expected+'\n')),None)
if section is None:raise SystemExit('Current version is missing from CHANGELOG.md')
print(section)
