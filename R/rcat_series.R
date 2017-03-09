#' rcat_series class
#'
#' \code{\link{rcat_serieses}} class 를 구성하는 세부 class
#' @docType class
#' @name rcat_series
#' @encoding utf-8
#' @author Heeseong Park \email{hspark90@@i-fam.net}
#' @keywords class
NULL

#' rcat_seriesItem
#'
#' \code{\link{rcat_series}} class에 포함되는 Item class
#' @docType class
#' @name rcat_seriesItem
#' @encoding utf-8
#' @author Heeseong Park \email{hspark90@@i-fam.net}
#' @keywords class
NULL

#' is.rcat_series
#'
#' \code{\link{rcat_series}} 클래스인지 아닌지 알려주는 함수
#' @param x \code{\link{rcat_series}} 클래스인지 확인 하고자하는 임의의 변수
#' @return \code{\link{rcat_series}} 클래스 자료여부를 알려주는 \code{\link{logical}} 변수
#' @author Heeseong Park \email{hspark90@@i-fam.net}
#' @encoding UTF-8
#' @export
is.rcat_series <- function(x) inherits(x, "rcat_series")

#' seriesItemType:  kinds of seriesItem
#'
#' seriesItem의 종류를 구분하는 정수
#' @author Heeseong Park \email{hspark90@@i-fam.net}
#' @encoding UTF-8
#' @docType data
#' @export
#' @examples with(seriesItemType, c(precip, rhumi, tempavg, wind, solar))
seriesItemType <- list(precip = 1L,
                       eva = 2L,
                       rhumi = 4L,
                       solar = 8L,
                       tempavg = 16L,
                       wind = 32L,
                       user = 64L,
                       tempmin = 128L,
                       tempmax = 256L,
                       evacalc = 512L,
                       date = 1024L,
                       observe = 2048L)


#' getSeriesItemTypeVals: get seriesItem type numbers.
#'
#' series의 각 컬럼에 대한 seriesItem의 번호를 얻어오는 함수로서
#' \code{\link{as.rcat_series}}함수의 매개변수를 구하는데 사용됩니다.
#'
#' @details
#' 각 변수별로 해당 컬럼의 번호를 입력하고 각 컬럼의 번호는 중복되면 안됩니다.
#'
#' 컬럼의 번호는 배열을 사용할 수 없으며 배열을 사용하면 해당 배열을 첫번째
#' 숫자만 인식됩니다.
#' 즉, 변수들은 테이블 상에 1회씩만 포함될 수 있음에 유의해야합니다.
#' @param precip Column number of precipitation data
#' @param eva Column number of evapotranspiration data
#' @param rhumi Column number of relative humidity data
#' @param solar Column number of daylight hours data
#' @param tempavg Column number of average temperture data
#' @param wind Column number of windspeed data
#' @param user Column number of user data
#' @param tempmin Column number of minimum temperture data
#' @param tempmax Column number of maximum temperture data
#' @param evacalc Column number of calculated evapotranspiration data
#' @param date column number of date time data
#' @param observe 관측값에 대한 컬럼 번호
#' @return 주어진 seriesItemType에 해당하는 정수 배열
#' @seealso \code{\link{as.rcat_series}}
#' @author Heeseong Park \email{hspark90@@i-fam.net}
#' @encoding UTF-8
#' @export
#' @examples getSeriesItemTypeVals(precip = 1, tempavg = 2, rhumi = 3, wind = 4, solar = 5)
getSeriesItemTypeVals   <-
  function(precip, eva, rhumi, solar,
           tempavg, wind, user, tempmin,
           tempmax, evacalc, date, observe)
{
  t <- c(precip = ifelse(missing(precip), 0, precip),
    eva = ifelse(missing(eva), 0, eva),
    rhumi = ifelse(missing(rhumi), 0, rhumi),
    solar = ifelse(missing(solar), 0, solar),
    tempavg = ifelse(missing(tempavg), 0, tempavg),
    wind = ifelse(missing(wind), 0, wind),
    user = ifelse(missing(user), 0, user),
    tempmin = ifelse(missing(tempmin), 0, tempmin),
    tempmax = ifelse(missing(tempmax), 0, tempmax),
    evacalc = ifelse(missing(evacalc), 0, evacalc),
    date = ifelse(missing(date), 0, date),
    observe = ifelse(missing(observe), 0, observe))
  if(any(t[duplicated(t)] != 0))
    stop("Error: numbers should not be duplicated!!")
  i <- t > 0
  v <- (2^(0:11))[i]
  as.integer(v[order(t[i])])
}

#' as.rcat_series: convert  a object to rcat_series
#'
#' 주어진 객체에 부가적인 사항들을 덧붙여
#' \code{\link{rcat_series}} 클래스로 변환한다.
#' @param x 변환하고자 하는 변수
#' @param start_time \code{\link{rcat_series}}의 시작시각
#' @param interval \code{\link{rcat_series}}의 시간간격
#' @param data_type_vals \code{\link{data.frame}}의 각 컬럼에 대한
#' \code{\link{seriesItemType}} 정수의 배열
#' @param ... 기타 매개변수
#' @return 변환된 \code{\link{rcat_series}} 객체
#' @author Heeseong Park \email{hspark90@@i-fam.net}
#' @encoding UTF-8
#' @seealso \code{\link{seriesItemType}}
#' @export
as.rcat_series <- function(x, ...) UseMethod("as.rcat_series")

#' @describeIn as.rcat_series
#' convert data as \code{\link{data.frame}} type to \code{\link{rcat_series}} type
#' @encoding UTF-8
#' @export
as.rcat_series.data.frame <-
  function(x, start_time, interval = c(1440, 60, 30, 10, 1),
           data_type_vals, ...)
{
    if(missing(start_time))
      stop("Error: start_time has no default!!")
    #interval = match.arg(interval)
    if(missing(data_type_vals) || !is.integer(data_type_vals) ||
       length(data_type_vals) != ncol(x))
      stop("Error: data_type_vals mismatch!!")

    if(is.timeBased(start_time)) t <- as.POSIXlt(start_time)
    st <- as.integer(c(1900 + t$year, 1+t$mon, t$mday, t$hour, t$min))
    ts <- as.integer(interval)
    for(i in 1:ncol(x))
    {
      attr(x[[i]], "StartTime") <- st
      attr(x[[i]], "Interval") <- ts
      attr(x[[i]], "DataType") <- data_type_vals[i]
      class(x[[i]]) <- c("rcat_seriesItem", "numeric")
    }
    attr(x, "StartTime") <- st
    attr(x, "Interval") <- ts
    attr(x, "DataType") <- sum(data_type_vals)
    class(x) <- c("rcat_series", "data.frame")
    x
}


#' @describeIn as.rcat_series
#' convert data as \code{\link{xts}} type to \code{\link{rcat_series}} type
#' @encoding UTF-8
#' @export
as.rcat_series.xts <- function(x, ...)
{
}

#' @describeIn as.rcat_series
#' convert data as \code{\link{matrix}} type to \code{\link{rcat_series}} type
#' @encoding UTF-8
#' @export
as.rcat_series.matrix <- function(x, ...)
{
}


#' c.rcat_series: combine rcat_series
#'
#' \code{\link{rcat_series}} 클래스 자료를 더해 하나의
#' \code{\link{rcat_serieses}} 형태의 자료로 만드는 함수
#' @param ... 더하고자 하는 \code{\link{rcat_series}} 클래스 자료를 나열
#' @return \code{\link{rcat_serieses}} 형태의 자료
#' @author Heeseong Park \email{hspark90@@i-fam.net}
#' @encoding UTF-8
#' @export
`c.rcat_series` <- function(...)
{
  o <- list(...)
  n <- do.call("str_c", as.list(names(o)))
  if(length(n) == 0) n <- ""
  structure(o,
            StartTime = attr(..1, "StartTime"),
            Interval = attr(..1, "Interval"),
            Name = n,
            Description = n,
            class = "rcat_serieses")
}

