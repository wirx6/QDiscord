#!/bin/bash

git checkout master;
mkdir ../QDiscordDocs;
set -e;
rm -r ../QDiscordDocs/*;
cp -r docs/html/* ../QDiscordDocs/;
git checkout gh-pages;
rm -r $(git ls-files);
rm -r search;
git checkout -- .gitignore;
git commit --all -m "Update documentation.";
git checkout master;
