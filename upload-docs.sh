#!/bin/bash

set -e

TEMPDIR=`mktemp -d`
OLDBRANCH=`git branch | egrep '^\*' | awk '{print $2}'`

mv ./doc/_build/html/* "${TEMPDIR}/"
git stash
git checkout gh-pages
cp -rf "${TEMPDIR}"/* ./
git add .
git commit -m "Documentación actualizada"
git push origin gh-pages
git checkout $OLDBRANCH
git stash pop --index
