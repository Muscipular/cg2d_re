"""只读定位本线带原地址的C++函数定义；不判断语义完成，不生成调用图。"""

from __future__ import annotations

import argparse
import json
import re
import sys
from pathlib import Path

FUNCTION_NAME = re.compile(
    r"\b(?P<name>(?:[A-Za-z_]\w*::)*[A-Za-z_]\w*_[0-9A-Fa-f]{6})\s*\("
)
ADDRESS_TAG = re.compile(r"_([0-9A-Fa-f]{6})(?=_|$)")
SUFFIX = re.compile(r"(?:const|noexcept|override|final)\b")
RAW_STRING = re.compile(r'R"([^\s()\\]{0,16})\(')
NUMBER = re.compile(r"[0-9][0-9A-Za-z_']*(?:\.[0-9A-Za-z_']*)?")
NAMESPACE = re.compile(
    r"\bnamespace(?:\s+([A-Za-z_]\w*(?:::[A-Za-z_]\w*)*))?\s*\{"
)
BRACE = re.compile(r"[{}]")


def mask_literals_and_comments(text: str) -> str:
    """保留位置及物理换行，屏蔽指令行、注释和字面量内的假定义。"""
    chars = list(text)
    position = 0
    while position < len(text):
        end = None
        if text[position] == "#" and not "".join(
            chars[text.rfind("\n", 0, position) + 1:position]
        ).strip():
            end = text.find("\n", position + 1)
            while end >= 0 and text[end - 1] == "\\":
                end = text.find("\n", end + 1)
            if end < 0:
                end = len(text)
        elif text.startswith("//", position):
            end = text.find("\n", position + 2)
            while end >= 0 and text[end - 1] == "\\":
                end = text.find("\n", end + 1)
            if end < 0:
                end = len(text)
        elif text.startswith("/*", position):
            closing = text.find("*/", position + 2)
            if closing < 0:
                raise ValueError("块注释未闭合")
            end = closing + 2
        elif text.startswith('R"', position):
            raw = RAW_STRING.match(text, position)
            if raw:
                terminator = ")" + raw.group(1) + '"'
                closing = text.find(terminator, raw.end())
                if closing < 0:
                    raise ValueError("原始字符串未闭合")
                end = closing + len(terminator)

        # 数字内的单引号是分隔符，不能当作字符常量的起点。
        if end is None and text[position].isdigit() and (
            position == 0
            or not (text[position - 1].isalnum() or text[position - 1] == "_")
        ):
            number = NUMBER.match(text, position)
            if number:
                position = number.end()
                continue

        if end is None and text[position] in ('"', "'"):
            quote = text[position]
            closing = position + 1
            while closing < len(text):
                if text[closing] == "\\":
                    closing += 2
                    continue
                if text[closing] == quote:
                    break
                closing += 1
            if closing >= len(text):
                raise ValueError("字符串或字符常量未闭合")
            end = closing + 1

        if end is None:
            position += 1
            continue
        for index in range(position, end):
            if chars[index] != "\n":
                chars[index] = " "
        position = end
    return "".join(chars)


def after_balanced_group(code: str, start: int, left: str, right: str) -> int:
    """返回配对括号之后的位置；无法闭合时停止，不静默丢弃源码。"""
    depth = 0
    for position in range(start, len(code)):
        if code[position] == left:
            depth += 1
        elif code[position] == right:
            depth -= 1
            if depth == 0:
                return position + 1
    raise ValueError(f"{left}{right}未闭合，字符位置{start}")


def find_definitions(path: Path, project: Path) -> list[dict]:
    """仅作定义形态定位；不进行预处理、类型解析或链接判断。"""
    text = path.read_text(encoding="utf-8")
    code = mask_literals_and_comments(text)
    definitions = []
    for match in FUNCTION_NAME.finditer(code):
        position = after_balanced_group(code, match.end() - 1, "(", ")")
        while True:
            while position < len(code) and code[position].isspace():
                position += 1
            suffix = SUFFIX.match(code, position)
            if suffix is None:
                break
            position = suffix.end()
            while position < len(code) and code[position].isspace():
                position += 1
            if suffix.group() == "noexcept" and code[position:position + 1] == "(":
                position = after_balanced_group(code, position, "(", ")")
        if code[position:position + 1] != "{":
            continue
        after_balanced_group(code, position, "{", "}")
        definitions.append({
            "_position": match.start(),
            "path": path.relative_to(project).as_posix(),
            "symbol": match.group("name"),
            "definition_line": code.count("\n", 0, match.start()) + 1,
        })

    openings = {
        match.end() - 1: match.group(1) or "(匿名)"
        for match in NAMESPACE.finditer(code)
    }
    braces = iter(BRACE.finditer(code))
    current = next(braces, None)
    scopes = []
    for definition in definitions:
        position = definition.pop("_position")
        while current is not None and current.start() < position:
            if current.group() == "{":
                scopes.append(openings.get(current.start()))
            elif scopes:
                scopes.pop()
            current = next(braces, None)
        definition["namespace"] = "::".join(scope for scope in scopes if scope)
    return definitions


def locate(project: Path) -> dict:
    inventory_path = project / "doc/cg2d_ida_inventory.json"
    inventory = json.loads(inventory_path.read_text(encoding="utf-8"))
    originals = {function["va"]: function for function in inventory["functions"]}
    if len(originals) != len(inventory["functions"]):
        raise ValueError("原元数据存在重复入口，不能自动合并")
    files = sorted(
        (
            path
            for directory in ("src", "include")
            for path in (project / directory).rglob("*")
            if path.is_file() and path.suffix in (".cpp", ".hpp")
        ),
        key=lambda path: path.relative_to(project).as_posix(),
    )
    locations = {}
    shared = []
    non_inventory = []
    for path in files:
        try:
            definitions = find_definitions(path, project)
        except ValueError as error:
            raise ValueError(f"{path.relative_to(project).as_posix()}: {error}") from error
        for definition in definitions:
            tags = [int(value, 16) for value in ADDRESS_TAG.findall(definition["symbol"])]
            if len(tags) != 1:
                shared.append({
                    "tagged_entries": [f"0x{value:X}" for value in tags],
                    "definition": definition,
                })
            elif tags[0] not in originals:
                non_inventory.append({
                    "source_address": f"0x{tags[0]:X}",
                    "definition": definition,
                })
            else:
                locations.setdefault(tags[0], []).append(definition)

    records = [
        {
            "inventory_id": originals[address]["id"],
            "original_entry": f"0x{address:X}",
            "definitions": definitions,
        }
        for address, definitions in sorted(locations.items())
    ]
    without_direct = [
        function["id"] for function in inventory["functions"]
        if function["va"] not in locations
    ]
    return {
        "schema_version": 1,
        "inventory": "doc/cg2d_ida_inventory.json",
        "generator": "scripts/locate_direct_function_definitions.py",
        "scope": "仅直接函数定义的源码定位；不判断参数/返回/副作用、语义完成或链接运行。",
        "limitations": [
            "不是完整C++解析或预处理；指令行不作为定义，条件分支不展开；复杂声明、宏、汇编和无地址名入口须另行映射。",
            "多定义不自动视为重定义；多地址名不自动拆分归属。",
            "非元数据地址不生成新ID；未命中不等于缺失或范围排除。",
            "定义行仅供本次定位，后续以文件和完整符号重新查找。",
        ],
        "counts": {
            "inventory_functions": len(originals),
            "source_files": len(files),
            "direct_inventory_entries": len(records),
            "direct_definitions": sum(len(row["definitions"]) for row in records),
            "multiple_definition_entries": sum(len(row["definitions"]) > 1 for row in records),
            "shared_tag_definitions": len(shared),
            "non_inventory_definitions": len(non_inventory),
            "without_direct_definition": len(without_direct),
        },
        "records": records,
        "shared_tag_definitions": shared,
        "non_inventory_definitions": non_inventory,
        "without_direct_definition": without_direct,
    }


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--project-root", type=Path, default=Path(__file__).resolve().parents[1])
    parser.add_argument("--output", required=True, type=Path, help="显式指定本次定位JSON输出文件")
    arguments = parser.parse_args()
    result = locate(arguments.project_root.resolve())
    arguments.output.write_text(
        json.dumps(result, ensure_ascii=False, indent=2) + "\n",
        encoding="utf-8", newline="\n",
    )
    print(json.dumps(result["counts"], ensure_ascii=False))


if __name__ == "__main__":
    sys.stdout.reconfigure(encoding="utf-8")
    sys.stderr.reconfigure(encoding="utf-8")
    main()
