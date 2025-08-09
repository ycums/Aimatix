#!/usr/bin/env bash
set -Eeuo pipefail

# GitHub CLI safe PR creation helper (Windows/MSYS2 friendly)
# - Uses --body-file only (avoids --body @-)
# - Normalizes CRLF
# - Verifies and reapplies body if empty
# - Avoids chaining commands with here-doc (no `&&` on same line)
#
# Usage examples:
#   TITLE="feat: update" bash scripts/gh_pr_safe.sh
#   TITLE="feat: update" BODY_SOURCE="./pr_body.md" bash scripts/gh_pr_safe.sh
#   TITLE="feat: update" bash scripts/gh_pr_safe.sh ./pr_body.md

BASE_BRANCH="${BASE_BRANCH:-main}"
TITLE="${TITLE:?Set TITLE env var (e.g. TITLE=\"feat: update\")}"
BODY_SOURCE="${BODY_SOURCE:-${1:-}}"
BODY_FILE="$(mktemp -t gh-pr-body.XXXXXX 2>/dev/null || echo ./pr_body.md)"

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
## 概要
変更内容の説明

## 変更内容
- 修正点1
- 修正点2

## テスト
- [ ] テスト項目1
- [ ] テスト項目2
EOF
  fi
  normalize_line_endings
}

ensure_env() {
  gh --version >/dev/null
  gh auth status >/dev/null
  git rev-parse --abbrev-ref HEAD >/dev/null
}

get_head_branch() {
  git rev-parse --abbrev-ref HEAD
}

get_open_pr_number_by_head() {
  local head_branch="$1"
  gh pr list --state open --head "${head_branch}" --json number --jq '.[0].number // empty'
}

safe_create_or_update_pr() {
  local head_branch
  head_branch="$(get_head_branch)"

  local pr_num
  pr_num="$(get_open_pr_number_by_head "${head_branch}")"

  if [ -z "${pr_num}" ]; then
    gh pr create \
      --base "${BASE_BRANCH}" \
      --head "${head_branch}" \
      --title "${TITLE}" \
      --body-file "${BODY_FILE}"
    pr_num="$(get_open_pr_number_by_head "${head_branch}")"
  fi

  # Verify and reapply body if empty/too short
  local body_len
  body_len="$(gh pr view "${pr_num}" --json body --jq '.body // ""' | wc -c | tr -d ' ')"
  if [ "${body_len}" -lt 10 ]; then
    gh pr edit "${pr_num}" --body-file "${BODY_FILE}"
  fi

  # Output summary
  gh pr view "${pr_num}" --json number,title,url --jq '{number,title,url}'
}

main() {
  ensure_env
  create_body
  safe_create_or_update_pr
}

main "$@"


