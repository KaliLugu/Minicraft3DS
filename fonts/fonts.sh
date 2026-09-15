#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BIN="$SCRIPT_DIR/minicraft3ds-font-json2bin"

INPUT_DIR="."
OUTPUT_DIR="."

usage() {
    echo "Usage: $0 [-i|--input-dir DIR] [-o|--output-dir DIR]" >&2
    echo "  -i, --input-dir DIR    Directory to scan for .json files (default: current directory)" >&2
    echo "  -o, --output-dir DIR   Directory where the .bin files will be saved (default: current directory)" >&2
    exit 1
}

# Parse arguments
while [[ $# -gt 0 ]]; do
    case "$1" in
        -i|--input-dir)
            if [[ -z "${2:-}" ]]; then
                echo "Error: missing value for $1" >&2
                usage
            fi
            INPUT_DIR="$2"
            shift 2
            ;;
        -o|--output-dir)
            if [[ -z "${2:-}" ]]; then
                echo "Error: missing value for $1" >&2
                usage
            fi
            OUTPUT_DIR="$2"
            shift 2
            ;;
        -h|--help)
            usage
            ;;
        *)
            echo "Error: unknown argument: $1" >&2
            usage
            ;;
    esac
done

if [[ ! -d "$INPUT_DIR" ]]; then
    echo "Error: input directory not found: $INPUT_DIR" >&2
    exit 1
fi

if [[ ! -f "$BIN" ]]; then
    echo "Error: $BIN not found." >&2
    exit 1
fi

if [[ ! -x "$BIN" ]]; then
    echo "Error: $BIN found but not executable (run: chmod +x \"$BIN\")." >&2
    exit 1
fi

# Create the output directory if it doesn't exist
mkdir -p "$OUTPUT_DIR"

shopt -s nullglob
json_files=( "$INPUT_DIR"/*.json )

if [[ ${#json_files[@]} -eq 0 ]]; then
    echo "No .json file found in: $INPUT_DIR"
    exit 0
fi

for json_file in "${json_files[@]}"; do
    # Output name: same name as the json, .fnt extension, in the chosen directory
    base_name="$(basename "${json_file%.json}")"
    output_file="$OUTPUT_DIR/${base_name}.fnt"

    echo "Converting: $json_file -> $output_file"
    if "$BIN" "$json_file" "$output_file" --verify; then
        echo "  OK"
    else
        echo "  FAILED for $json_file" >&2
    fi
done

echo "Done."
