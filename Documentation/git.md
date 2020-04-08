## 간단 git 사용법

현재 브랜치의 최신 헤드로부터 새 브랜치를 만들어서, 작업을 한 후 다시 원래 브랜치에 합치는 과정.

0. 작업 시작 전 원격 저장소 정보 가져와서 

```shell
git fetch
git pull
```

만약 병합 충돌 발생시 `git merge <target branchname>` 명령으로 해결. 현재 브랜치와 target 브랜치를 병합시킴

1. 브랜치 만들기 
make new branch

```shell
git branch <branchname>
```

2. 브랜치 이동
checkout the branch

```shell
git checkout <branchname>
```

* you can use `git checkout -b <branchname>` to make a new branch and checkout to that branch

3. 파일 수정하기
make some modification on workspace

4. 로컬 변경 스테이지 (파일단위 또는 줄단위로 commit에 들어갈 변경사항을 등록)
stage local changes

```shell
git add <filename>
```

use `git add .` to stage all files

5. 스테이지된 변경 커밋하기
commit staged changes

```shell
git commit -m '<present tense commit message>'
```

6. 로컬 변경을 원격 저장소에 업로드
upload local change to remote repository

```shell
git push
```

* 만약 이 단계에서 `--set-upstream` 관련 에러가 난다면, 원격지에 해당 브랜치가 만들어지지 않아서 경고하는 것이므로 터미널에서 시키는 대로 명령을 따라 치면 됨.
`git push --set-upstream origin <branchname>` 대략 이럴 것임.

* 만약 이 단계에서 병합 충돌이 난다면 누군가 같은 브랜치 같은 파일을 수정해서 먼저 `git push`를 한 것.

7. 원래 브랜치로 합치기

github 페이지에서 make a pull request 버튼을 써서 해도 되고, 다음 명령으로 해도 된다.

```shell
git checkout <original branchname>
git merge <branchname to merge with>
```


추가 git 명령

```shell
git log --oneline # commit 히스토리를 보여준다.

git branch -D <branchname> # 해당 브랜치를 삭제

git pull --force # 로컬 변경을 원격지 최신 버전으로 덮어씌움 (주의)

git reset HEAD^ # 마지막 커밋을 되돌림 (변경사항은 손실되지 않고 다시 커밋되지 않은 로컬 변경사항으로 바뀜.)

git reset HEAD~3 # 세개 커밋을 되돌림.
```