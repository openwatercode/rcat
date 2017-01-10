# rcat
This project is to porting CAT Hydrology model to R package.

한국건설기술연구원에서 개발한 수문모형인 CAT 모형을 R 패키지 형태로 포팅하기 위한 프로젝트입니다.

현재 프로젝트 진행버전: 0.1.5

현재 버전은 Windows와 Linux에서 구동 가능합니다.

## install rcat
본 프로젝트에서 제공하는 rcat 패키지를 설치하여 테스트 하기 위해서는 소스형태로 내려 받아 컴파일(Linux) 해야할 필요가 있습니다.

패키지를 컴파일하기 위해서는 다음과 같은 프로그램들이 필요합니다.
(참고: 현재 Windows Rtools 환경에서만 컴파일이 확인됨)

  * R - https://cran.r-project.org/bin/windows/ 에서 내려 받을 수 있습니다.
  * RTools(Windows 환경인 경우) - https://cran.r-project.org/bin/windows/ 에서 내려 받을 수 있습니다.
  * Rstudio - https://www.rstudio.com/products/rstudio/download/ 에서 내려 받을 수 있습니다.

패키지를 설치하기 위해서는 Rstudio에서 다음과 같이 'devtools' 등 관련 패키지를 먼저 설치하고 'install_github()' 함수를 이용해 'rcat'을 설치합니다. 

```
install.packages("rskrpatch", repos = "http://kict-r.prj.kr/")
install.packages(Rcpp)
install.packages(devtools)
install_github("openwatercode/rcat")
```

## run example
예제를 수행하기 위해서 입력예제 파일을 다운로드 받습니다.

```
download.file("http://r.prj.kr/data/rcat/rcat_ex.zip", "rcat_ex.zip")
```

예제 파일의 압축을 해제합니다.

```
unzip("rcat_ex.zip")
```

### 수행결과를 파일로 저장하기
다음과 같이 입력파일과 출력파일 이름을 넣어 CAT을 실행합니다.

```
RunCAT("dist_101208.txt", "dist_101208.out")
```

### 수행결과를 data.frame에 대한 리스트로 받아오기
다음과 같이 입력파일만의 이름을 넣어 CAT을 실행하고 그 결과를 변수로 저장합니다.

```
result <- RunCAT("dist_101208.txt")
names(result)
```

### 수행결과 리스트에서 결과 그려보기
수행결과중 원하는 노드나 링크의 결과("Outlet 1")를 따로 변수로 받아 xts 객체로 변환하여 그래프를 그려 봅니다.

```
library(xts)
outlet1 <- as.xts(result[["Outlet 1"]])
outlet1.flowtotal <- outlet1[,1]
plot(outlet1.flowtotal, type="l")
```

## License
본 프로그램의 라이센스는 AGPL-3를 따릅니다.
상업적인 이용을 위해서는 저작권자인 한국건설기술연구원의 허가를 받아야합니다.
