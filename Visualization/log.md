# 시각화 과제 작업 기록

## webviz : 실패

1. node run command 호환문제 : 각 실행 명령에 `cross-env` 추가하여 해결.
2. rollup parse 에러 : `lerna.json`에서 해당 패키지 제거하고 직접 설치하여 해결.
3. 웹 앱이 안 뜨고 상대 경로만 나오는 문제 : `packages/webviz-core/public/index.html` 파일을 `docs/public/app/index.html`로 옮김.
4. react hot loader 로딩 실패 에러


## ros2d : 예제 실행 성공

1. websocket origin 'null' 에러 : `express`로 서버를 만들어 접속하여 해결.


