#!/bin/bash

set -e

TEMPDIR=`mktemp -d`
OLDBRANCH=`git branch | egrep '^\*' | awk '{print $2}'`

cp ./doc/_build/html/* "${TEMPDIR}/"
rm -rf doc/_build/html
git stash
git checkout gh-pages
cp -rf "${TEMPDIR}"/* ./
git add .
git commit -m "Documentación actualizada"
git push origin gh-pages
git checkout $OLDBRANCH
git stash pop --index
