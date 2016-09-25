#!/bin/bash

git checkout master;
mkdir ../QDiscordDocs;
rm -r ../QDiscordDocs/*;
cp -r QDiscord/docs/html/* ../QDiscordDocs/;
git checkout gh-pages;
rm -r $(git ls-files);
rmdir search;
git checkout -- .gitignore;
cp -r ../QDiscordDocs/* .;
git add --all;
git commit -m "Update documentation.";
git checkout master;
