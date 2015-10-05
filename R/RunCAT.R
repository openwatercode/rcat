#' RunCAT: run the cat model
#'
#' RunCAT 모형 구동 함수.
#' @param infile 입력파일의 경로
#' @param outfile 출력파일의 경로, 생략하면 리스트로 결과가 반환됨
#' @param format 포맷, "" 일 경우 바이너리 형태로 저장되며,
#' "[노드명:필드번호1,필드번호2...]" 노드명의 지정필드를 텍스트파일로 출력
#' "[*|*]" 일 경우 모든 노드의 결과가 텍스트 형태로 저장됨
#'
#' 출력파일이 생략된 경우 선택적으로 결과를 반환받을 수 있음
#' @return 파일로 출력할 경우 보이지 않는 정수
#'
#' 파일이 아닌 R 변수로 받을 경우 각 노드의 계산결과가 \code{\link{data.frame}}
#' 형태로 저장된 리스트 객체
#' \itemize{
#'   \item{유역노드}
#'   \tabular{llll}{
#'     노드형태\tab 출력항목\tab 단위\tab 설명\cr
#'     ---\tab---\tab---\tab---\cr
#'     Urban & Forest\tab Inflow\tab (㎥)\tab 외부유입량\cr
#'     \tab Rainfall\tab (mm)\tab 강우량\cr
#'     \tab Actual Evapotranspiration\tab (mm)\tab 실제증발산량\cr
#'     \tab Potential Evapotranspiration\tab (mm)\tab 잠재증발산량\cr
#'     \tab Infiltrate\tab (mm)\tab침투량\cr
#'     \tab Soil moisture content\tab \tab 토양수분\cr
#'     \tab Flow surface\tab (mm)\tab 직접유출량\cr
#'     \tab Flow inter\tab (mm)\tab 중간유출량\cr
#'     \tab Flow groudwater\tab (mm)\tab 지하수유출량\cr
#'     \tab Recharge\tab (mm)\tab 지하수함양량\cr
#'     \tab Flow total\tab (mm)\tab 총 유출량\cr
#'     \tab Groundwater Movement\tab (mm)\tab 지하수 유동량\cr
#'     \tab Groundwater Elevation\tab (El. m)\tab 지하수위\cr
#'     ---\tab---\tab---\tab---\cr
#'     Paddy \tab Inflow\tab (㎥)\tab 유입량\cr
#'     \tab Rainfall\tab (mm)\tab 강우량\cr
#'     \tab Actual Evapotranspiration\tab (mm)\tab 실제증발산량\cr
#'     \tab Potential Evapotranspiration\tab (mm)\tab 잠재증발산량\cr
#'     \tab Infiltrate\tab (mm)\tab 침투량\cr
#'     \tab Soil moisture content\tab \tab 토양수분\cr
#'     \tab Flow surface\tab (mm)\tab 직접유출량\cr
#'     \tab Surf drainage culvert\tab (mm)\tab 논에서의 암거 배수량\cr
#'     \tab Surf drainage pipe\tab (mm)\tab 논에서의 파이프 배수량\cr
#'     \tab Ponding depth\tab (mm)\tab 논의 담수심\cr
#'     \tab Flow inter\tab (mm)\tab 중간유출량\cr
#'     \tab Flow groundwater\tab (mm)\tab 지하수유출량\cr
#'     \tab Recharge\tab (mm)\tab 지하수함양량\cr
#'     \tab Flow total\tab (mm)\tab 총 유출량\cr
#'     \tab Groundwater Movement\tab (mm)\tab 지하수 유동량\cr
#'     \tab Groundwater Elevation\tab (El. m)\tab 지하수위\cr
#'     \tab pipe out\tab (mm)\tab 파이프 배수량(개량물꼬)\cr
#'   }
#'   \item{물순환 개선시설 노드}
#'   \tabular{llll}{
#'     노드형태\tab 출력항목\tab 단위\tab 설명\cr
#'     ---\tab---\tab---\tab---\cr
#'     Infiltro\tab Inflow\tab (㎥)\tab 상류 유입량\cr
#'     \tab Outflow\tab (㎥)\tab 지표면 유출량\cr
#'     \tab Infiltro\tab (㎥)\tab 침투량\cr
#'     ---\tab---\tab---\tab---\cr
#'     Bioretention\tab Inflow\tab (㎥)\tab 상류 유입량\cr
#'     \tab Total inflow\tab (㎥)\tab 총 유입량(단위면적당 강우량 포함)\cr
#'     \tab Outflow\tab (㎥)\tab 지표면 유출량\cr
#'     \tab Evapotranspiration\tab (mm)\tab 증발산량\cr
#'     \tab Infiltrate\tab (㎥)\tab 침투량\cr
#'     \tab Rainfall\tab (mm)\tab 강우량\cr
#'     ---\tab---\tab---\tab---\cr
#'     Pond\tab Inflow\tab (㎥)\tab 상류단 유입량\cr
#'     \tab Downstream outflow\tab (㎥)\tab 저류시설로 유입되지 않은 본류 유량\cr
#'     \tab Rainfall\tab (mm)\tab 강우량\cr
#'     \tab Intake\tab (㎥)\tab 취수량\cr
#'     \tab Evapotranspiration water surface\tab (mm)\tab 수면증발량\cr
#'     \tab Spilway outflow\tab (㎥)\tab 물넘이 방류량\cr
#'     \tab Pipe outflow\tab (㎥)\tab 방류구 방류량\cr
#'     \tab Aquifer loss\tab (㎥)\tab 바닥면 손실량\cr
#'     \tab Volume\tab (㎥)\tab 저류량\cr
#'     \tab Water level\tab (mm)\tab 저수위\cr
#'     ---\tab---\tab---\tab---\cr
#'     Wetland\tab Inflow\tab (㎥)\tab 상류 유입량\cr
#'     \tab Rainfall\tab (mm)\tab 강우량\cr
#'     \tab Bypass volume\tab (㎥)\tab 홍수기 우회량\cr
#'     \tab Evapotranspiration water surface\tab (mm)\tab 수면증발량\cr
#'     \tab Spilway outflow\tab (㎥)\tab 물넘이 방류량\cr
#'     \tab Pipe outflow\tab (㎥)\tab 방류구 방류량\cr
#'     \tab Aquifer loss\tab (㎥)\tab 바닥면 손실량\cr
#'     \tab Volume\tab (㎥)\tab 저류량\cr
#'     \tab Water level\tab (mm)\tab 저수위\cr
#'     ---\tab---\tab---\tab---\cr
#'     Rain Tank\tab Inflow\tab (㎥)\tab 상류 유입량\cr
#'     \tab Rain Tank inflow\tab (㎥)\tab 빗물저장시설의 최소유입량 조건을 충족한 유입량\cr
#'     \tab Water Use\tab (㎥)\tab 사용량\cr
#'     \tab Spilway outflow\tab (㎥)\tab 물넘이 방류량\cr
#'     \tab Downstream outflow\tab (㎥)\tab 빗물저장시설의 최소유입량 이하의 방류량\cr
#'     \tab Volume\tab (㎥)\tab 저류량\cr
#'     ---\tab---\tab---\tab---\cr
#'     Recycle\tab Inflow\tab (㎥)\tab 유입량\cr
#'     \tab Intake\tab (㎥)\tab 취수량\cr
#'     \tab Downstream outflow\tab (㎥)\tab 취수 후 하류단 유출량\cr
#'     \tab Output nodes\tab \tab 노드별 분배량(5개까지 표현 가능)\cr
#'     ---\tab---\tab---\tab---\cr
#'     Import\tab Water Supply\tab (㎥)\tab 급수 공급량\cr
#'  }
#' }
#'
#' @author 박희성 \email{hspark90@@i-fam.net}
#' @useDynLib rcat
#' @import Rcpp xts
#' @encoding UTF-8
#' @export
#' @examples
#' download.file("http://r.prj.kr/data/rcat/rcat_ex.zip", "rcat_ex.zip")
#' unzip("rcat_ex.zip")
#'
#' # save the output to file
#' RunCAT("dist_101208.txt", "dist_101208.out")
#'
#' # get the output to list of data.frame
#' result <- RunCAT("dist_101208.txt")
#' names(result)
#'
#' # create xts and plotting
#' library(xts)
#' outlet1 <- as.xts(result[["Outlet 1"]])
#' outlet1.flowtotal <- outlet1[,"flow_total(㎥/s)"]
#' plot(outlet1.flowtotal, main = "Flow Total",
#'      ylab = names(outlet1.flowtotal), xlab = "Time", type = "l")
RunCAT <- function(infile, outfile, format = "[*:*]")
{
  if (missing(infile))
    stop("File or InputData Missing!")
  if (is.rcat_input(infile)) {
    ret <-
      run_cat2(infile, format)
    msg <- ret[["ErrMsgs"]]
    wmsg <- unlist(strsplit(msg, "\r\n"))
    if(length(wmsg) > 0)
      for (i in 1:length(wmsg))
        warning(wmsg[i])
    r <- ret[["CAT_RESULT"]]
    for(i in 1:length(r))
      if(!is.null(colnames(r[[i]]))) Encoding(colnames(r[[i]])) <- "UTF-8"
    return(r)
  }
  if (!file.exists(infile))
    stop(sprintf("%s does not exists!", infile))
  if (missing(outfile)) {
    ret <-
      run_cat1(list(infile = path.expand(infile), format = format))
    msg <- ret[["msg"]]
    wmsg <- unlist(strsplit(msg, "\r\n"))
    if(length(wmsg) > 0)
      for (i in 1:length(wmsg))
        warning(wmsg[i])
    r <- ret[["ret"]]
    for(i in 1:length(r))
      if(!is.null(colnames(r[[i]]))) Encoding(colnames(r[[i]])) <- "UTF-8"
    return(r)
  } else {
    if (infile == outfile)
      stop("Output file name should not be the same to Input file name!")
    msg <-
      run_cat0(path.expand(infile), path.expand(outfile), format)
    wmsg <- unlist(strsplit(msg, "\r\n"))
    if(length(wmsg) > 0)
      for (i in 1:length(wmsg))
        warning(wmsg[i])
  }
}
