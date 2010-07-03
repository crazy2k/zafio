#!/bin/bash

cd ..

TEMPDIR=`mktemp`
OLDBRANCH=`git branch | egrep '^\*' | awk '{print $2}'`

cp -r doc/_build/html/* "$TEMPDIR/"
git stash
git checkout gh-pages
cp -rf "$TEMPDIR/*" ./
git add .
git commit -a -m "Documentación actualizada"
git push origin gh-pages
git checkout $BRANCH
git stash pop --index
