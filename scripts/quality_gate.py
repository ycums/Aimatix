#!/usr/bin/env python3
"""
品質ゲートオーケストレーター

機能:
- 既存のカバレッジ計測システム（scripts/test_coverage.py の CoverageMeasurementSystem）を実行
- Clang-Tidy 静的解析（pio check --json-output）を実行し、重要度別に件数集計
- 設定ファイル（coverage_config.json）のしきい値に基づき、統合品質ゲートを判定
- 結果を標準出力と JSON ファイル（static_analysis_report.json）に出力

使用例:
  python scripts/quality_gate.py --quick
  python scripts/quality_gate.py --full
  python scripts/quality_gate.py --release
"""

from __future__ import annotations

import argparse
import json
import os
import subprocess
import sys
from dataclasses import dataclass
from fnmatch import fnmatch
from typing import Dict, List, Optional, Tuple


PROJECT_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), os.pardir))


def _load_config(config_path: str) -> dict:
    with open(config_path, "r", encoding="utf-8") as f:
        return json.load(f)


def _normalize_path(path: str) -> str:
    # Windows/Unix 混在に強い正規化（判定は '/' ベースで行う）
    return path.replace("\\", "/")


def _is_excluded(file_path: str, exclude_patterns: List[str]) -> bool:
    normalized = _normalize_path(file_path)
    for pattern in exclude_patterns:
        # 絶対/相対どちらにもマッチするようにパターン拡張
        patterns_to_try = [pattern]
        if not pattern.startswith("**/"):
            patterns_to_try.append(f"**/{pattern}")
        for p in patterns_to_try:
            if fnmatch(normalized, p):
                return True
    return False


@dataclass
class StaticAnalysisThresholds:
    high: int = 0
    medium: Optional[int] = None
    low: Optional[int] = None


@dataclass
class StaticAnalysisResult:
    totals: Dict[str, int]
    by_file: Dict[str, Dict[str, int]]
    passed: bool
    thresholds: StaticAnalysisThresholds
    effective_thresholds: StaticAnalysisThresholds
    tool: str
    environment: str


def run_clang_tidy_json(env: str, src_filter: Optional[List[str]] = None) -> List[dict]:
    """pio check を JSON 出力で実行し、JSON配列を返す。

    失敗時は例外を投げる。
    """
    variants = [
        ["--json-output"],
        ["--json-output", "--no-ansi"],
        ["--json-output", "--silent"],
    ]
    last_output = ""
    last_rc = None
    for extra in variants:
        cmd = ["pio", "check", "-e", env] + extra
        if src_filter:
            joined = " ".join(src_filter)
            cmd.extend(["--src-filters", joined])
        completed = subprocess.run(
            cmd,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            check=False,
        )
        output = completed.stdout.decode("utf-8", errors="replace").strip()
        last_output = output
        last_rc = completed.returncode
        if not output:
            # 出力が空で rc==0 の場合は欠陥なし扱い
            if completed.returncode == 0:
                return []
            # それ以外は次のバリアントへ
            continue
        try:
            data = json.loads(output)
            if isinstance(data, list):
                return data
        except Exception:
            # 次のバリアントへ
            continue
    raise RuntimeError(
        "Failed to parse pio check JSON after retries (rc=%s)\nRaw: %s" % (last_rc, last_output[:2048])
    )


def collect_static_analysis(env: str, exclude_patterns: List[str], src_filter: Optional[List[str]] = None) -> Tuple[Dict[str, int], Dict[str, Dict[str, int]]]:
    """Clang-Tidy の重要度別件数を収集。

    Returns:
        totals: {"high": int, "medium": int, "low": int}
        by_file: { filepath: {severity: count} }
    """
    records = run_clang_tidy_json(env, src_filter=src_filter)
    totals = {"high": 0, "medium": 0, "low": 0}
    by_file: Dict[str, Dict[str, int]] = {}

    # records: [{ env, tool, duration, defects: [ {severity, file, ...}, ... ] }]
    for rec in records:
        defects = rec.get("defects", []) or []
        for d in defects:
            severity = (d.get("severity") or "").lower()
            file_path = d.get("file") or ""
            if not file_path:
                continue
            if _is_excluded(file_path, exclude_patterns):
                continue
            key = "low" if severity == "low" else ("high" if severity == "high" else "medium")
            totals[key] = totals.get(key, 0) + 1
            norm_path = _normalize_path(file_path)
            by_file.setdefault(norm_path, {}).setdefault(key, 0)
            by_file[norm_path][key] += 1

    return totals, by_file


def evaluate_static_analysis(
    env: str,
    tool: str,
    thresholds: StaticAnalysisThresholds,
    exclude_patterns: List[str],
    src_filter: Optional[List[str]] = None,
    auto_medium_margin: int = 5,
) -> StaticAnalysisResult:
    totals, by_file = collect_static_analysis(env, exclude_patterns, src_filter=src_filter)

    # 有効なしきい値（None の場合は自動算出: current + auto_medium_margin）
    effective = StaticAnalysisThresholds(
        high=thresholds.high if thresholds.high is not None else 0,
        medium=(
            thresholds.medium
            if thresholds.medium is not None
            else totals.get("medium", 0) + max(0, int(auto_medium_margin))
        ),
        low=thresholds.low,
    )

    high_ok = totals.get("high", 0) <= (effective.high if effective.high is not None else 0)
    medium_ok = totals.get("medium", 0) <= (effective.medium if effective.medium is not None else float("inf"))
    low_ok = True
    if effective.low is not None:
        low_ok = totals.get("low", 0) <= effective.low

    passed = high_ok and medium_ok and low_ok

    return StaticAnalysisResult(
        totals=totals,
        by_file=by_file,
        passed=passed,
        thresholds=thresholds,
        effective_thresholds=effective,
        tool=tool,
        environment=env,
    )


def write_static_analysis_report(result: StaticAnalysisResult, output_path: str) -> None:
    data = {
        "tool": result.tool,
        "environment": result.environment,
        "totals": result.totals,
        "thresholds": {
            "high": result.thresholds.high,
            "medium": result.thresholds.medium,
            "low": result.thresholds.low,
        },
        "effective_thresholds": {
            "high": result.effective_thresholds.high,
            "medium": result.effective_thresholds.medium,
            "low": result.effective_thresholds.low,
        },
        "passed": result.passed,
        "by_file": result.by_file,
    }
    with open(output_path, "w", encoding="utf-8") as f:
        json.dump(data, f, ensure_ascii=False, indent=2)


def run_coverage(mode: str, config_path: str) -> dict:
    """既存のカバレッジ計測を実行し、品質ゲート判定結果を返す。"""
    # import パス調整（scripts ディレクトリを import path に追加）
    scripts_dir = os.path.join(PROJECT_ROOT, "scripts")
    if scripts_dir not in sys.path:
        sys.path.insert(0, scripts_dir)

    try:
        from test_coverage import CoverageMeasurementSystem  # type: ignore
    except Exception as e:
        raise RuntimeError(f"Failed to import CoverageMeasurementSystem: {e}")

    system = CoverageMeasurementSystem(config_path)
    if mode == "quick":
        system.run_quick_coverage_measurement()
    elif mode == "full":
        system.run_full_coverage_measurement()
    elif mode == "release":
        system.run_release_coverage_measurement()
    else:
        system.run_quick_coverage_measurement()

    qg = system.check_quality_gate()
    return qg


def load_static_analysis_config(config: dict) -> Tuple[str, List[str], StaticAnalysisThresholds, List[str], int]:
    sa_cfg = config.get("static_analysis", {}) or {}
    tool = sa_cfg.get("tool", "clang-tidy")
    envs = sa_cfg.get("environments", ["native"]) or ["native"]
    environment = envs[0]
    exclude_patterns = sa_cfg.get("exclude_patterns", ["lib/**", "test/**"]) or []
    src_filter = sa_cfg.get("src_filter", ["+<lib/libaimatix/src/>", "+<src/>", "-<src/spikes/>"]) or []

    thresholds_cfg = sa_cfg.get("severity_thresholds", {}) or {}
    thresholds = StaticAnalysisThresholds(
        high=int(thresholds_cfg.get("high", 0)) if thresholds_cfg.get("high", 0) is not None else 0,
        medium=(
            int(thresholds_cfg.get("medium")) if thresholds_cfg.get("medium") is not None else None
        ),
        low=(int(thresholds_cfg.get("low")) if thresholds_cfg.get("low") is not None else None),
    )
    auto_medium_margin = int(sa_cfg.get("auto_medium_margin", 5))

    return environment, exclude_patterns, thresholds, src_filter, auto_medium_margin


def main() -> int:
    parser = argparse.ArgumentParser(description="Quality Gate Orchestrator")
    parser.add_argument("--config", default=os.path.join(PROJECT_ROOT, "coverage_config.json"))
    parser.add_argument(
        "--quick", action="store_true", help="Use quick coverage mode (default)"
    )
    parser.add_argument("--full", action="store_true", help="Use full coverage mode")
    parser.add_argument("--release", action="store_true", help="Use release coverage mode")
    args = parser.parse_args()

    mode = "quick"
    if args.full:
        mode = "full"
    if args.release:
        mode = "release"

    config_path = args.config
    config = _load_config(config_path)

    # 1) カバレッジ実行 + 判定
    coverage_qg = run_coverage(mode, config_path)

    # 2) 静的解析 実行 + 判定
    env, exclude_patterns, thresholds, src_filter, auto_medium_margin = load_static_analysis_config(config)
    sa_result = evaluate_static_analysis(
        env,
        tool="clang-tidy",
        thresholds=thresholds,
        exclude_patterns=exclude_patterns,
        src_filter=src_filter,
        auto_medium_margin=auto_medium_margin,
    )

    # 3) 統合判定
    passed = bool(coverage_qg.get("passed")) and sa_result.passed

    # 出力（サマリ）
    print("=== Quality Gate Summary ===")
    print(f"Coverage: {'PASS' if coverage_qg.get('passed') else 'FAIL'} | actual={coverage_qg.get('actual_coverage')} threshold={coverage_qg.get('threshold')} strict={coverage_qg.get('strict_mode')}")
    print(
        "Static Analysis: {status} | high={h}/{ht}, medium={m}/{mt}, low={l}/{lt}".format(
            status="PASS" if sa_result.passed else "FAIL",
            h=sa_result.totals.get("high", 0),
            ht=sa_result.effective_thresholds.high if sa_result.effective_thresholds.high is not None else "-",
            m=sa_result.totals.get("medium", 0),
            mt=sa_result.effective_thresholds.medium if sa_result.effective_thresholds.medium is not None else "-",
            l=sa_result.totals.get("low", 0),
            lt=sa_result.effective_thresholds.low if sa_result.effective_thresholds.low is not None else "-",
        )
    )
    print(f"Overall: {'PASS' if passed else 'FAIL'}")

    # 付帯ファイル出力（Step Summary からも読みやすい JSON）
    report_path = os.path.join(PROJECT_ROOT, "static_analysis_report.json")
    write_static_analysis_report(sa_result, report_path)

    return 0 if passed else 1


if __name__ == "__main__":
    sys.exit(main())


