#!/usr/bin/env bash
set -euo pipefail

# download makerom & bannertool
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "${SCRIPT_DIR}")"

MAKEROM_URL="https://github.com/3DSGuy/Project_CTR/releases/download/makerom-v0.18.4/makerom-v0.18.4-ubuntu_x86_64.zip"
MAKEROM_ZIP_SHA256="dd596854718c195c6e3229286be485b122921715555af8ae5cf8e9a465d9f970"
MAKEROM_ZIP_ENTRY="makerom"
MAKEROM_BIN_SHA256="38051b5d31f6fcec10fc073f2d610c1e95922e4ebb84850601ac3ea4dc591fa2"

BANNERTOOL_URL="https://github.com/diasurgical/bannertool/releases/download/1.2.0/bannertool.zip"
BANNERTOOL_ZIP_SHA256="69768596f836acb3e3aeaa66e47c6ba560dde813c6dfcd33c8afc25fe29b7524"
BANNERTOOL_ZIP_ENTRY="linux-x86_64/bannertool"
BANNERTOOL_BIN_SHA256="f1f73965c5154392fe29868d0fbad5da74c8f53fd30eeeca04c803f00b083125"

log()  { echo "[INFO]  $*"; }
warn() { echo "[WARN]  $*" >&2; }
die()  { echo "[ERROR] $*" >&2; exit 1; }

for cmd in curl sha256sum unzip awk; do
    command -v "${cmd}" >/dev/null 2>&1 || die "Missing dependency: '${cmd}'"
done

TMP_DIR="$(mktemp -d)"
cleanup() { rm -rf "${TMP_DIR}"; }
trap cleanup EXIT

sha256_of() {
    sha256sum "$1" | awk '{print $1}'
}

fetch_tool() {
    local name="$1" url="$2" zip_sha256="$3" zip_entry="$4" bin_sha256="$5"
    local dest="${PROJECT_ROOT}/${name}"
    local zip_path="${TMP_DIR}/${name}.zip"

    if [[ -f "${dest}" ]] && [[ "$(sha256_of "${dest}")" == "${bin_sha256}" ]]; then
        log "${name} already present and up to date, skipping download."
        return
    fi

    log "Downloading ${name} from ${url}..."
    curl \
        --silent \
        --show-error \
        --fail \
        --location \
        --proto '=https' \
        --tlsv1.2 \
        --max-time 60 \
        --retry 3 \
        --retry-delay 2 \
        --output "${zip_path}" \
        "${url}" \
        || die "Download failed from ${url}"

    local actual_zip_sha256
    actual_zip_sha256="$(sha256_of "${zip_path}")"
    [[ "${actual_zip_sha256}" == "${zip_sha256}" ]] \
        || die "SHA256 mismatch for ${name}.zip! Expected: ${zip_sha256} | Got: ${actual_zip_sha256}"
    log "${name}.zip SHA256 verified OK."

    unzip -p "${zip_path}" "${zip_entry}" > "${dest}.tmp" \
        || die "Failed to extract '${zip_entry}' from ${name}.zip"

    local actual_bin_sha256
    actual_bin_sha256="$(sha256_of "${dest}.tmp")"
    [[ "${actual_bin_sha256}" == "${bin_sha256}" ]] \
        || die "SHA256 mismatch for extracted ${name}! Expected: ${bin_sha256} | Got: ${actual_bin_sha256}"

    mv "${dest}.tmp" "${dest}"
    chmod +x "${dest}"
    log "${name} installed to '${dest}'."
}

fetch_tool "makerom" "${MAKEROM_URL}" "${MAKEROM_ZIP_SHA256}" "${MAKEROM_ZIP_ENTRY}" "${MAKEROM_BIN_SHA256}"
fetch_tool "bannertool" "${BANNERTOOL_URL}" "${BANNERTOOL_ZIP_SHA256}" "${BANNERTOOL_ZIP_ENTRY}" "${BANNERTOOL_BIN_SHA256}"
