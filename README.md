# rcat
This project is to porting CAT Hydrology model to R package.

한국건설기술연구원에서 개발한 수문모형인 CAT 모형을 R 패키지 형태로 포팅하기 위한 프로젝트입니다.

## install rcat
본 프로젝트를 설치하여 테스트 하기 위해서는 바이너리 형태로 설치하거나 소스형태로 내려 받아 컴파일 해야할 필요가 있습니다.

프로젝트를 설치하기 위해서는 다음과 같은 프로그램들이 필요합니다.
(참고: 현재 Windows Rtools 환경에서만 컴파일이 확인됨)

  * R - https://cran.r-project.org/bin/windows/ 에서 내려 받을 수 있습니다.
  * RTools(Windows 환경인 경우) - https://cran.r-project.org/bin/windows/ 에서 내려 받을 수 있습니다.
  * Rstudio - https://www.rstudio.com/products/rstudio/download/ 에서 내려 받을 수 있습니다.
  * user_name: 프로젝트에 참가하고 있는 사용자의 이름
  * user_pat: 사용자 토큰(사용자 토큰은 https://github.com/settings/tokens 에서 발급 받을 수 있습니다.)

프로젝트를 설치하기 위해서는 Rstudio에서 다음과 같이 'devtools'를 먼저 설치하고 'install_github()' 함수를 이용해 설치합니다. 

```
install.packages("rskrpatch", repos = "http://r.prj.kr/pkg")
install.packages(Rcpp)
install.packages(devtools)
install_github("openwatercode/rcat", "user_name", auth_token = "user_pat")
```
## run example
예제를 수행하기 위해서 입력예제 파일을 다운로드 받습니다.

```
download.file("http://r.prj.kr/R/rcat/rcat.zip", "rcat.zip")
```

예제 파일의 압축을 해제합니다.

```
unzip("rcat.zip")
```

CAT을 실행합니다.

```
RunCAT("dist_101208.txt", "dist_101208.out")
```
