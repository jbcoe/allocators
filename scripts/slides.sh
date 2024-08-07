#!/usr/bin/env bash

# Install Marp CLI
# https://formulae.brew.sh/formula/marp-cli


# Generate slides
# ./scripts/slides.sh

cd $(dirname $(dirname "$0"))
set -x
marp talks/cpp_on_sea_2024.md --allow-local-files --html -o cpp_on_sea_2024.pdf
marp talks/cpp_on_sea_2024.md --allow-local-files --html -o cpp_on_sea_2024.html
