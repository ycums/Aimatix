#!/usr/bin/env bash
set -Eeuo pipefail

# GitHub CLI safe Issue creation helper (Windows/MSYS2 friendly)
# - Uses --body-file only (avoids --body @-)
# - Normalizes CRLF
# - Verifies and reapplies body if empty
# - Avoids chaining commands with here-doc (no `&&` on same line)
#
# Usage examples:
#   TITLE="bug: fix crash on settings" bash scripts/gh_issue_safe.sh
#   TITLE="feat: add dark theme" LABELS="enhancement,ui" BODY_SOURCE="./issue_body.md" bash scripts/gh_issue_safe.sh
#   TITLE="bug: ..." bash scripts/gh_issue_safe.sh ./issue_body.md

TITLE="${TITLE:?Set TITLE env var (e.g. TITLE=\"bug: cannot save settings\")}"
LABELS="${LABELS:-}"
BODY_SOURCE="${BODY_SOURCE:-${1:-}}"
BODY_FILE="$(mktemp -t gh-issue-body.XXXXXX 2>/dev/null || echo ./issue_body.md)"

normalize_line_endings() {
  if command -v dos2unix >/dev/null 2>&1; then
    dos2unix "${BODY_FILE}" || true
  else
    tr -d '\r' < "${BODY_FILE}" > "${BODY_FILE}.tmp" && mv "${BODY_FILE}.tmp" "${BODY_FILE}"
  fi
}

create_body() {
  if [ -n "${BODY_SOURCE}" ] && [ -f "${BODY_SOURCE}" ]; then
    cp "${BODY_SOURCE}" "${BODY_FILE}"
  else
    cat > "${BODY_FILE}" << 'EOF'
## バグの概要
簡潔なバグの説明

## 再現手順
1. 手順1
2. 手順2

## 期待される動作
期待する結果

## 実際の動作
実際の結果

## 環境情報
- OS:
- PlatformIO:
- Python:

## 修正方針
- 方針1

## 完了条件
- [ ] 条件1

## 参照
- リンク
EOF
  fi
  normalize_line_endings
}

ensure_env() {
  gh --version >/dev/null
  gh auth status >/dev/null
}

extract_issue_number_from_url() {
  # expects URL like https://github.com/owner/repo/issues/1234
  local url="$1"
  printf '%s\n' "${url##*/}"
}

safe_create_issue() {
  local create_cmd=(gh issue create --title "${TITLE}" --body-file "${BODY_FILE}" --json url --jq .url)
  if [ -n "${LABELS}" ]; then
    create_cmd+=(--label "${LABELS}")
  fi

  local issue_url
  issue_url="$(${create_cmd[@]})"
  local issue_num
  issue_num="$(extract_issue_number_from_url "${issue_url}")"

  # Verify and reapply body if empty/too short
  local body_len
  body_len="$(gh issue view "${issue_num}" --json body --jq '.body // ""' | wc -c | tr -d ' ')"
  if [ "${body_len}" -lt 10 ]; then
    gh issue edit "${issue_num}" --body-file "${BODY_FILE}"
  fi

  # Output summary
  gh issue view "${issue_num}" --json number,title,url --jq '{number,title,url}'
}

main() {
  ensure_env
  create_body
  safe_create_issue
}

main "$@"


