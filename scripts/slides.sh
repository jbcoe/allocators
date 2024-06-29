!#/usr/bin/env bash
cd $(dirname $(dirname "$0"))
marp TALK.md --allow-local-files --html -o TALK.pdf
