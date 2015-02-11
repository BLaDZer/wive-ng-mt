#!/bin/sh

VERSION="3.10"

OUT="$1"

v="$VERSION"

echo '#include "iw.h"' > "$OUT"
echo "const char iw_version[] = \"$v\";" >> "$OUT"
