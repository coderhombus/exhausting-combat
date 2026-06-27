git fetch upstream
git checkout main
git merge upstream/main
git push origin main

## rebase:

git fetch upstream
git checkout main
git rebase upstream/main
git push origin main --force-with-lease

-- then:
git checkout my-mod
git rebase main


## if there are conflicts:

git add .
git rebase --continue

## abort:

git rebase --abort