#!/bin/bash

(set -e

cd ..

TEMPDIR=`mktemp -d`
OLDBRANCH=`git branch | egrep '^\*' | awk '{print $2}'`

cp -r ./doc/_build/html/* "${TEMPDIR}/"
git stash
git checkout gh-pages
cp -rf "${TEMPDIR}"/* ./
#git add .
#git commit -m "Documentación actualizada"
#git push github gh-pages
#git checkout $OLDBRANCH
#git stash pop --index

cd doc)