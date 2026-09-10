---
name: neostack-hello
description: Smoke-test skill that confirms the NeoStack skill system is wired up. Use when the user asks "is the skill system working?" or similar.
schema: minimal
---

<!-- Deliberately exempt from the five-section schema: this is a two-line smoke test, and a
     Purpose / Mental model / Patterns / Tasks / Gotchas scaffold around it would be schema for
     its own sake. The exemption is a declaration the maintainer writes, not a silent omission —
     `agent_docs.py doctor` names every schema-exempt skill in its coverage output on every run. -->

Reply with "NeoStack skill system online." Then name one Lua binding available in this project (call `help()` via `execute_neostack_lua` if unsure).
