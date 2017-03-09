#' csv2CATBinSeriesFile
#'
#' @param csv csv file name to read.
#' @param coltype \code{\link{seriesItemType}}
#' @param file file name converted to binary
#'
#' @return NULL
#' @export
#'
#' @examples
#' csv2CATBinSeriesFile(csv = "climate3.csv",
#'   coltype = getSeriesItemTypeVals(date = 1,
#'                                   precip = 2,
#'                                   tempavg = 3,
#'                                   wind = 4,
#'                                   rhumi = 5,
#'                                   solar = 6),
#'   file = "climate3.dat")
#' @author Heeseong Park \email{hspark90@@i-fam.net}
#' @encoding UTF-8
#' @seealso \code{\link{seriesItemType}}
#' @export
#' @importFrom stats median
#' @importFrom utils read.csv
csv2CATBinSeriesFile <- function(csv, coltype, file)
{
  df <- read.csv(csv, as.is = T)
  tci <- which(coltype == 1024)
  if(length(tci) == 0) stop("Error: No data column defined!")
  t <- as.POSIXct(df[, tci])
  dt <- as.numeric(median(diff(as.POSIXct(t))), units = "mins")
  st <- first(t)
  rs <- as.rcat_series(df[, -tci], start_time = st, interval = dt, coltype[-tci])
  rss <- rcat_serieses(rs)
  writeBinCATSerieses(rss, file)
}
