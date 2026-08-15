#!/usr/bin/env python3
"""Unify the executable/package name across the repo.

Rewrites every exact, known statement that names the product/executable --
it does NOT do a blind find/replace of the name (that would also corrupt
unrelated prose, e.g. README.md's "This demo is licensed...", and would
mangle a name that's already a substring of another, e.g. "SVision3" inside
"SVision3Demo").

Covers:
  - CMakeLists.txt: add_executable/target_link_libraries/set_target_properties/
    install(TARGETS ...)
  - packaging/<name>.desktop: Name=/Exec= fields, AND the file itself gets
    renamed to packaging/<new-name>.desktop (the filename embeds the name too)

Deliberately NOT touched: the icon (packaging/svision3demo.png, Icon= field).
Icon names conventionally stay lowercase/hyphenated regardless of the
product name's capitalization (freedesktop icon naming), so they're not
tied to this rename.

Usage:
    ./rename_product.py                       # dry run, default demo -> SVision3Demo
    ./rename_product.py --apply                # actually write the changes
    ./rename_product.py OldName NewName --apply
"""
import argparse
import re
import sys
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parent

# Each entry: (file relative to repo root, [(regex, replacement_template), ...])
# replacement_template uses {new} for the new name.
RULES = {
    "CMakeLists.txt": [
        (r"add_executable\(demo\b", "add_executable({new}"),
        (r"target_link_libraries\(demo\b", "target_link_libraries({new}"),
        (r"set_target_properties\(demo\b", "set_target_properties({new}"),
        (r"install\(TARGETS demo\b", "install(TARGETS {new}"),
    ],
}


def handle_rules(old_name: str, new_name: str, apply: bool) -> bool:
    if old_name != "demo":
        rules = {
            path: [(pattern.replace("demo\\b", re.escape(old_name) + r"\b"), repl) for pattern, repl in entries]
            for path, entries in RULES.items()
        }
    else:
        rules = RULES

    any_changes = False
    for rel_path, entries in rules.items():
        file_path = REPO_ROOT / rel_path
        if not file_path.exists():
            print(f"-- skip (not found): {rel_path}")
            continue

        original = file_path.read_text()
        updated = original
        file_changes = []
        for pattern, template in entries:
            replacement = template.format(new=new_name)
            updated, count = re.subn(pattern, replacement, updated)
            if count:
                file_changes.append((pattern, count))

        if not file_changes:
            print(f"-- no matches: {rel_path}")
            continue

        any_changes = True
        print(f"== {rel_path} ==")
        for pattern, count in file_changes:
            print(f"   {count}x  {pattern}")

        if apply:
            file_path.write_text(updated)
        else:
            for line_no, (old_line, new_line) in enumerate(zip(original.splitlines(), updated.splitlines()), 1):
                if old_line != new_line:
                    print(f"   L{line_no}: -{old_line.strip()}")
                    print(f"   L{line_no}: +{new_line.strip()}")

    return any_changes


def handle_desktop_file(old_name: str, new_name: str, apply: bool) -> bool:
    # Handled separately from RULES: the desktop file's own PATH embeds the
    # name (packaging/<name>.desktop), so this needs to rename the file
    # itself, not just edit its content.
    old_path = REPO_ROOT / "packaging" / f"{old_name}.desktop"
    new_path = REPO_ROOT / "packaging" / f"{new_name}.desktop"

    if not old_path.exists():
        print(f"-- skip (not found): packaging/{old_name}.desktop")
        return False

    content = old_path.read_text()
    updated = content
    field_changes = []
    for field in ("Name", "Exec"):
        pattern = rf"^{field}={re.escape(old_name)}$"
        updated, count = re.subn(pattern, f"{field}={new_name}", updated, flags=re.MULTILINE)
        if count:
            field_changes.append((field, count))

    renaming = old_path != new_path
    if not field_changes and not renaming:
        print(f"-- no matches: packaging/{old_name}.desktop")
        return False

    print(f"== packaging/{old_name}.desktop ==")
    for field, count in field_changes:
        print(f"   {count}x  {field}={old_name}")
    if renaming:
        print(f"   rename -> packaging/{new_name}.desktop")

    if apply:
        old_path.write_text(updated)
        if renaming:
            old_path.rename(new_path)
    else:
        for line_no, (old_line, new_line) in enumerate(zip(content.splitlines(), updated.splitlines()), 1):
            if old_line != new_line:
                print(f"   L{line_no}: -{old_line.strip()}")
                print(f"   L{line_no}: +{new_line.strip()}")

    return True


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("old_name", nargs="?", default="demo", help="Current executable/target name (default: demo)")
    parser.add_argument("new_name", nargs="?", default="SVision3Demo", help="Name to use everywhere (default: SVision3Demo)")
    parser.add_argument("--apply", action="store_true", help="Write changes (default is dry-run/preview only)")
    args = parser.parse_args()

    any_changes = handle_rules(args.old_name, args.new_name, args.apply)
    any_changes = handle_desktop_file(args.old_name, args.new_name, args.apply) or any_changes

    if not any_changes:
        print("Nothing to change.")
    elif not args.apply:
        print("\nDry run only -- re-run with --apply to write these changes.")
    else:
        print("\nDone.")

    return 0


if __name__ == "__main__":
    sys.exit(main())
