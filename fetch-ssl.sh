#!/usr/bin/env bash
set -euo pipefail

# config
DEST_DIR="resources"
DEST_FILE="${DEST_DIR}/cacert.pem"
URL="https://curl.se/ca/cacert.pem"
URL_SHA256="https://curl.se/ca/cacert.pem.sha256"
TMP_FILE="${DEST_FILE}.tmp"

# function
log()  { echo "[INFO]  $*"; }
warn() { echo "[WARN]  $*" >&2; }
die()  { echo "[ERROR] $*" >&2; exit 1; }

cleanup() {
    rm -f "${TMP_FILE}"
}
trap cleanup EXIT

# deps
for cmd in curl sha256sum awk; do
    command -v "${cmd}" >/dev/null 2>&1 || die "Missing dependency: '${cmd}'"
done

# check for write 
if [[ ! -d "${DEST_DIR}" ]]; then
    warn "Directory '${DEST_DIR}' does not exist. Creating..."
    mkdir -p "${DEST_DIR}" || die "Failed to create directory '${DEST_DIR}'"
fi

if [[ ! -w "${DEST_DIR}" ]]; then
    die "Directory '${DEST_DIR}' is not writable."
fi

# download temp file
log "Downloading cacert.pem from ${URL}..."
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
    --output "${TMP_FILE}" \
    "${URL}" \
    || die "Download failed from ${URL}"

# check the file is not empty
[[ -s "${TMP_FILE}" ]] || die "Downloaded file is empty."

# check infor from cert
if ! grep -q "BEGIN CERTIFICATE" "${TMP_FILE}"; then
    die "Downloaded file does not appear to be a valid PEM bundle."
fi

ACTUAL_SHA256="$(sha256sum "${TMP_FILE}" | awk '{print $1}')"
log "SHA256: ${ACTUAL_SHA256}"

# fetch reference hash from curl.se (HTTPS-trusted — no hardcoded value to maintain)
log "Fetching reference SHA256 from ${URL_SHA256}..."
EXPECTED_SHA256="$(curl \
    --silent --show-error --fail \
    --proto '=https' --tlsv1.2 \
    --max-time 30 \
    "${URL_SHA256}" | awk '{print $1}')" \
    || die "Failed to fetch reference SHA256 from ${URL_SHA256}"

if [[ "${ACTUAL_SHA256}" != "${EXPECTED_SHA256}" ]]; then
    die "SHA256 mismatch! Expected: ${EXPECTED_SHA256} | Got: ${ACTUAL_SHA256}"
fi
log "SHA256 hash verified OK."

# change destination file
mv "${TMP_FILE}" "${DEST_FILE}" \
    || die "Failed to move file to '${DEST_FILE}'"

# change file right
chmod 644 "${DEST_FILE}"

log "cacert.pem successfully downloaded to '${DEST_FILE}'."
