#' rcat_series class
#'
#' rcat_serieses class 를 구성하는 세부 클래스입니다.
#' @docType class
#' @name rcat_series
#' @encoding utf-8
#' @author \href{http://www.i-fam.net/water/}{박희성} \email{hspark90@@i-fam.net}
#' @keywords class
NULL

#' is.rcat_series
#'
#' \code{\link{rcat_series}} 클래스인지 아닌지 알려주는 함수
#' @param x \code{\link{rcat_series}} 클래스인지 확인 하고자하는 임의의 변수
#' @return \code{\link{rcat_series}} 클래스 자료여부를 알려주는 \code{\link{logical}} 변수
#' @author 박희성 \email{hspark90@@i-fam.net}
#' @encoding UTF-8
#' @export
is.rcat_series <- function(x) inherits(x, "rcat_series")


#' seriesItemTypes:  kinds of seriesItem
#'
#' seriesItem의 종류를 구분하는 문자열
#' @author 박희성 \email{hspark90@@i-fam.net}
#' @encoding UTF-8
#' @docType data
#' @export
seriesItemTypes <- c("precip", "eva", "rhumi", "solar",
                     "tempavg", "wind", "user",
                     "tempmin", "tempmax", "evacalc",
                     "date", "observe")

#' @export
as.rcat_series <- function(x, ...) UseMethod("as.rcat_series")

#' getSeriesItemTypeVal: convert seriesItem type string to seriesItem type number.
#'
#' @param type_str seriesItemType을 구분하는 문자열
#' @return 주어진 seriesItemType에 해당하는 정수
#' @author 박희성 \email{hspark90@@i-fam.net}
#' @encoding UTF-8
#' @noRd
getSeriesItemTypeVal <-
  function(type_str = seriesItemTypes)
{
    type_str = match.arg(type_str)
    switch(type,
           precip = 1,
           eva = 2,
           rhumi = 4,
           solar = 8,
           tempavg = 16,
           wind = 32,
           user = 64,
           tempmin = 128,
           tempmax = 256,
           evacalc = 512,
           date = 1024,
           observe = 2048)
}

#' getSeriesItemTypeVals: convert seriesItem type string to seriesItem type number.
#'
#' @param type_str seriesItemType을 구분하는 문자열 배열
#' @return 주어진 seriesItemType에 해당하는 정수
#' @author 박희성 \email{hspark90@@i-fam.net}
#' @encoding UTF-8
#' @export
getSeriesItemTypeVals <- function(type_strs)
{
  sapply(type_strs, getSeriesItemTypeVal)
}

#' as.rcat_series.data.frame: convert  data.frame to rcat_series
#'
#' \code{\link{data.frame}} 클래스에 부가적인 사항들을 덧붙여
#' \code{\link{rcat_series}} 클래스로 변환한다.
#' @param x \code{\link{data.frame}} 클래스의 변수
#' @return \code{\link{POSIXt}} 형태의 시간 변수
#' @author 박희성 \email{hspark90@@i-fam.net}
#' @encoding UTF-8
#' @export
as.rcat_series.data.frame <-
  function(x, start_time, interval = c(1440, 60, 30, 10, 1),
           data_type_vals, ...)
{
    if(missing(start_time))
      stop("Error: start_time has no default!!")
    interval = match.arg(interval)
    if(missing(data_type_vals) || !is.integer(data_type_vals) ||
       length(data_type_vals) != nrow(x))
      stop("Error: data_type_vals mismatch!!")

    if(is.timeBased(start_time)) t <- as.POSIXlt(start_time)
    st <- c(1900 + t$year, 1+t$mon, t$mday, t$hour, t$min)
    ts <- interval
    for(i in 1:ncol(x))
    {
      attr(x[[i]], "StartTime") <- st
      attr(x[[i]], "Interval") <- ts
      attr(x[[i]], "DataType") <- data_type_vals[i]
      class(x[[i]]) <- c("rcat_seriesItem", "numeric")
    }
    attr(x, "StartTime") <- st
    attr(x, "Interval") <- ts
    attr(x, "DataType") <- sum(data_types)
    class(x) <- c("rcat_series", "data.frame")
    x
}


#' as.rcat_series.xts: get rcat_input StartTime
#'
#' \code{\link{rcat_input}} 클래스의 StartTime 속성을 가져온다.
#' @param x \code{\link{rcat_input}} 클래스의 변수
#' @return \code{\link{POSIXt}} 형태의 시간 변수
#' @author 박희성 \email{hspark90@@i-fam.net}
#' @encoding UTF-8
#' @export
as.rcat_series.xts <- function(x, ...)
{
}

#' as.rcat_series.matrix: get rcat_input StartTime
#'
#' \code{\link{rcat_input}} 클래스의 StartTime 속성을 가져온다.
#' @param x \code{\link{rcat_input}} 클래스의 변수
#' @return \code{\link{POSIXt}} 형태의 시간 변수
#' @author 박희성 \email{hspark90@@i-fam.net}
#' @encoding UTF-8
#' @export
as.rcat_series.matrix <- function(x, ...)
{
}
