#!/bin/sh

# Wive-NG generator history script

# define
HISTORY="$ROOTDIR/History"

# HTTP header
#echo "Content-type: text/html"
#echo ""

# web header
echo "<html><head><title>Wive-NG-MT</title>"

echo '<META HTTP-EQUIV="Pragma" CONTENT="no-cache">'
echo '<META HTTP-EQUIV="Expires" CONTENT="-1">'
echo '<meta http-equiv="content-type" content="text/html; charset=utf-8">'
echo '<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">'
echo '</head>'

# web body
echo "<body><table class=body><tr><td>"

# parse History file
sed -e '
s_\(^RT288.*\)_<h1>\1</h1><hr />_
s_\(^Version.*\)_<h3>\1</h3>_
s_^- \(\[.*\]\)\(.*\)_- <b>\1</b>\2 <br />_
s_\(^$\)_\1 <br />_
s_\(^==.*\)__
' $HISTORY

echo "</td></tr></table>"
echo "</body></html>"
