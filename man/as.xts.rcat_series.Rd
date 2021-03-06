% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/as.xts.rcat_series.R
\encoding{UTF-8}
\name{as.xts.rcat_series}
\alias{as.xts.rcat_series}
\title{as.xts.rcat_series: rcat_sereis class data convert to xts calss}
\usage{
\method{as.xts}{rcat_series}(x, ...)
}
\arguments{
\item{x}{rcat 모형에서 사용하는 "rcat_series" 클래스의 변수}

\item{...}{\code{\link{xts}}에 사용되는 기타 매개변수}
}
\value{
변환된 \code{\link{xts}} 변수
}
\description{
"rcat_series" 클래스 자료를 xts로 변환하는 함수.
}
\examples{
download.file("http://r.prj.kr/data/rcat/rcat_ex.zip", "rcat_ex.zip")
unzip("rcat_ex.zip")

climate_data <- readBinCATSerieses("P_climate_1209.dat")
clxts <- as.xts(climate_data[[1]])
plot(clxts[,1])
}
\author{
Heeseong Park \email{hspark90@i-fam.net}
}
