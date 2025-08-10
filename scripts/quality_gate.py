#!/usr/bin/env python3
"""
品質ゲートオーケストレーター

機能:
- 既存のカバレッジ計測システム（scripts/test_coverage.py の CoverageMeasurementSystem）を実行
- Clang-Tidy 静的解析（pio check -e native --json-output）を実行し、重要度別に件数集計
- 設定ファイル（coverage_config.json）のしきい値に基づき、統合品質ゲートを判定
- 結果を標準出力と JSON ファイル（static_analysis_report.json）に出力

使用例:
  python scripts/quality_gate.py
"""

from __future__ import annotations

import argparse
import json
import os
import subprocess
import sys
from dataclasses import dataclass
from typing import Dict, List, Optional, Tuple

PROJECT_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), os.pardir))

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

def _load_config(config_path: str) -> dict:
    with open(config_path, "r", encoding="utf-8") as f:
        return json.load(f)

def _normalize_path(path: str) -> str:
    return path.replace("\\", "/")

def run_clang_tidy_json() -> List[dict]:
    env = os.environ.copy()
    env.update({
        "CI": "1",
        "PLATFORMIO_CORE_CALLER": "github-actions",
        "PLATFORMIO_DISABLE_PROGRESSBAR": "1",
        "PLATFORMIO_NO_COLOR": "1",
        "PLATFORMIO_SETTING_ENABLE_PROMPTS": "false",
    })

    def _run(args: List[str]) -> str:
        completed = subprocess.run(
            args,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            check=False,
            cwd=PROJECT_ROOT,
            env=env,
            text=True,
            encoding="utf-8",
            errors="replace",
        )
        return (completed.stdout or "").strip()

    args = [
        "pio", "check", "-e", "native", "--json-output", "--project-dir", PROJECT_ROOT
    ]

    output = _run(args)

    # CI上で稀に空になる対策としてサイレント再試行
    if not output:
        output = _run(args + ["--silent"])

    # デバッグ用に常にRAWログを保存
    raw_path = os.path.join(PROJECT_ROOT, "static_analysis_raw.txt")
    try:
        with open(raw_path, "w", encoding="utf-8") as f:
            f.write(output or "")
    except Exception:
        pass

    if not output:
        raise RuntimeError("pio check produced empty output with --json-output")

    # JSONの前後にノイズが混入するケースへのフォールバック
    try:
        data = json.loads(output)
    except json.JSONDecodeError:
        import re
        m = re.search(r"(\[\s*\{[\s\S]*\}\s*\])", output)
        if not m:
            raise RuntimeError(f"Failed to parse pio check JSON. Head: {output[:500]}")
        data = json.loads(m.group(1))

    if isinstance(data, list):
        return data
    raise ValueError("Unexpected JSON root type")

def collect_static_analysis() -> Tuple[Dict[str, int], Dict[str, Dict[str, int]]]:
    records = run_clang_tidy_json()
    totals = {"high": 0, "medium": 0, "low": 0}
    by_file: Dict[str, Dict[str, int]] = {}
    for rec in records:
        defects = rec.get("defects", []) or []
        for d in defects:
            severity = (d.get("severity") or "").lower()
            file_path = d.get("file") or ""
            if not file_path:
                continue
            key = "low" if severity == "low" else ("high" if severity == "high" else "medium")
            totals[key] = totals.get(key, 0) + 1
            norm_path = _normalize_path(file_path)
            by_file.setdefault(norm_path, {}).setdefault(key, 0)
            by_file[norm_path][key] += 1
    return totals, by_file

def evaluate_static_analysis(
    thresholds: StaticAnalysisThresholds,
) -> StaticAnalysisResult:
    totals, by_file = collect_static_analysis()
    effective = StaticAnalysisThresholds(
        high=thresholds.high if thresholds.high is not None else 0,
        medium=thresholds.medium,
        low=thresholds.low,
    )
    high_ok = totals.get("high", 0) <= (effective.high if effective.high is not None else 0)
    medium_ok = True if effective.medium is None else totals.get("medium", 0) <= effective.medium
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
        tool="clang-tidy",
        environment="native",
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

def run_coverage(config_path: str) -> dict:
    scripts_dir = os.path.join(PROJECT_ROOT, "scripts")
    if scripts_dir not in sys.path:
        sys.path.insert(0, scripts_dir)
    try:
        from test_coverage import CoverageMeasurementSystem  # type: ignore
    except Exception as e:
        raise RuntimeError(f"Failed to import CoverageMeasurementSystem: {e}")
    system = CoverageMeasurementSystem(config_path)
    # quick のみサポート
    system.run_quick_coverage_measurement()
    qg = system.check_quality_gate()
    return qg

def load_static_analysis_config(config: dict) -> StaticAnalysisThresholds:
    sa_cfg = config.get("static_analysis", {}) or {}
    thresholds_cfg = sa_cfg.get("severity_thresholds", {}) or {}
    return StaticAnalysisThresholds(
        high=int(thresholds_cfg.get("high", 0)) if thresholds_cfg.get("high", 0) is not None else 0,
        medium=(int(thresholds_cfg.get("medium")) if thresholds_cfg.get("medium") is not None else None),
        low=(int(thresholds_cfg.get("low")) if thresholds_cfg.get("low") is not None else None),
    )

def main() -> int:
    parser = argparse.ArgumentParser(description="Quality Gate Orchestrator")
    parser.add_argument("--config", default=os.path.join(PROJECT_ROOT, "coverage_config.json"))
    args = parser.parse_args()

    config_path = args.config
    config = _load_config(config_path)

    coverage_qg = run_coverage(config_path)
    thresholds = load_static_analysis_config(config)
    sa_result = evaluate_static_analysis(thresholds=thresholds)
    passed = bool(coverage_qg.get("passed")) and sa_result.passed

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

    report_path = os.path.join(PROJECT_ROOT, "static_analysis_report.json")
    write_static_analysis_report(sa_result, report_path)

    return 0 if passed else 1

if __name__ == "__main__":
    sys.exit(main())
