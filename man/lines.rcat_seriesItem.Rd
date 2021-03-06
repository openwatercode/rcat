% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/lines.rcat_seriesItem.R
\encoding{UTF-8}
\name{lines.rcat_seriesItem}
\alias{lines.rcat_seriesItem}
\title{lines.rcat_seriesItem: plot the rcat_seriesItem}
\usage{
\method{lines}{rcat_seriesItem}(x, ...)
}
\arguments{
\item{x}{rcat 모형에서 사용하는 \code{\link{rcat_seriesItem}} 클래스의 변수}

\item{...}{\code{\link{lines}} 함수의 매개변수를 참조}
}
\description{
\code{\link{rcat_seriesItem}} 클래스 자료를 그래프 형태로 보여주는 함수.
}
\examples{
download.file("http://r.prj.kr/data/rcat/rcat_ex.zip", "rcat_ex.zip")
unzip("rcat_ex.zip")
iseri <- readBinCATSerieses("P_climate_1209.dat")
plot(iseri[[1]]$t)
lines(iseri[[1]]$t, col = "blue")
}
\seealso{
\code{\link{plot.rcat_seriesItem}}
}
\author{
Heeseong Park \email{hspark90@i-fam.net}
}
