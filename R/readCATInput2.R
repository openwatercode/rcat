#' readCATInput2: read from CAT input file to r varialble which is rcat_input class
#'
#' 텍스트로 구성된 CAT 모형을 읽어 "rcat_input" 클래스의 변수로 반환
#' @param filename CAT 모형의 텍스트형 입력자료
#' @return 입력파일의 내용이 들어 있는 \code{\link{rcat_input}} 클래스의 변수
#' @author 박희성 \email{hspark90@@i-fam.net}
#' @import plyr
#' @encoding UTF-8
#' @export
#' @examples
#' download.file("http://r.prj.kr/data/rcat/rcat_ex.zip", "rcat_ex.zip")
#' unzip("rcat_ex.zip")
#'
#' model <- readCATInput2("dist_101208.txt")
readCATInput2 <- function(file, ...)
{
  f <- file(file, ...)
  l <- readLines(f)
  close(f)
  l1 <- str_trim(l)
  l2 <- l1[!str_detect(l, "^#") & nchar(l) != 0]
  ns <- which(str_detect(l2, "^Node[ =]"))
  ne <- which(str_detect(l2, "^EndNode"))
  n <- as.list(as.data.frame(t(cbind(ns, ne))))
  n1 <- lapply(n,
               function(x) {
                 nl <- str_split(l2[x[1]:x[2]], "[ ]*=[ ]*")
                 cls <- paste0("cat_", nl[[1]][2])
                 nl1 <- nl[2:(length(nl) - 1)]
                 ret <- mapply(function(c, v) {
                   switch(v[1],
                    NodeID = as.integer(v[2]),
                    Name = v[2],
                    Desc = v[2],
                    Position = as.integer(comma_split(v[2])),
                    cat_input_parse(c, v[1], v[2])
                   )
                 }, c = cls, v = nl1, SIMPLIFY = F)
                 names(ret) <- unlist(lapply(nl1, function(y) y[1]))
                 ret$Desc <- ""
                 ret <- plyr::compact(ret)
                 class(ret) <- paste0("cat_", nl[[1]][2])
                 ret
               })
  lapply(str_split(l2[1:(ns[1] - 1)], "[ ]*=[ ]*"),
         function(x) attr(n1, x[1]) <<- switch(x[1],
                          Version = x[2],
                          Title = x[2],
                          as.integer(comma_split(x[2]))))
  names(n1) <- 1:length(n1)
  class(n1) <- "rcat_input"
  n1
}

comma_split <- function(x) str_trim(str_split(x, ",")[[1]])

cat_input_parse <- function(c, k, x) {
  type <- switch(
    c,
    cat_Climate = switch(
      k,
      Calculation = "numeric",
      Evaporation = "character",
      Rainfall = "character"
    ),
    cat_WetLand = switch(
      k,
      AREA = "numeric",
      Base = "numeric",
      EVA = "numeric",
      Pipe = "numeric",
      Rainfall = "numeric",
      RateCount = "integer",
      Recharge = "integer",
      VOL = "numeric",
      WL = "numeric"
    ),
    cat_Pond = switch(
      k,
      AREA = "numeric",
      Base = "numeric",
      EVA = "integer",
      Intake = "numeric",
      Offline = "numeric",
      Pipe = "numeric",
      Rainfall = "integer",
      RateCount = "integer",
      Recharge = "integer",
      Series = "character",
      Spill = "numeric",
      Supply = "integer",
      VOL = "numeric",
      WL = "numeric"
    ),
    cat_Recycle = switch(k,
                         Intake = "numeric",
                         Nodes = "data.frame"),
    cat_RainTank = switch(
      k,
      Series = "character",
      Supply = "integer",
      Use = "numeric",
      Volume = "numeric"
    ),
    cat_Infiltro = switch(k,
                          Aquifer = "numeric",
                          GWMove = "numeric"),
    cat_Link = switch(
      k,
      Connect = "integer",
      Cunge = "numeric",
      Kinematic = "numeric",
      Method = "integer",
      Muskingum = "numeric"
    ),
    cat_Forest = switch(
      k,
      Evaporation = "numeric",
      GWout = "numeric",
      Infiltro = "numeric",
      Intake = "numeric",
      River = "numeric",
      Soil = "numeric",
      Topology = "numeric",
      Weather = "data.frame"
    ),
    cat_Paddy = switch(
      k,
      Coefficient = "numeric",
      Drain = "numeric",
      Evaporation = "numeric",
      GWout = "numeric",
      Intake = "numeric",
      Irrigation = "integer",
      River = "numeric",
      Soil = "numeric",
      Topology = "numeric",
      Weather = "data.frame"
    ),
    cat_BioRetention = switch(
      k,
      Aquifer = "numeric",
      EVA = "numeric",
      Evaporation = "numeric",
      GWMove = "numeric",
      Rainfall = "numeric"
    ),
    cat_Import = switch(
      k,
      Constant = "numeric",
      Leakage = "numeric",
      Series = "character",
      Table = "integer",
      Type = "integer"
    ),
    cat_Urban = switch(
      k,
      Evaporation = "numeric",
      GWout = "numeric",
      Infiltro = "numeric",
      Intake = "numeric",
      River = "numeric",
      Soil = "numeric",
      Topology = "numeric",
      Weather = "data.frame"
    ),
    NULL
  )

  if(is.null(type)) return(NULL);

  r <- switch(
    type,
    character = x,
    integer = as.integer(comma_split(x)),
    numeric = as.numeric(comma_split(x)),
    data.frame = switch(k,
                        Nodes = {
                          df <- parse2df(x)
                          colnames(df) <- c("nID", "nRain")
                          df$nID <- as.integer(df$nID)
                          df$nRain <- as.numeric(df$nRain)
                          df
                        },
                        Weather = {
                          df <- parse2df(x)
                          colnames(df) <-
                            c("nID", "nRain", "nEva")
                          df$nID <- as.integer(df$nID)
                          df$nRain <- as.numeric(df$nRain)
                          df$nEva <- as.numeric(df$nEva)
                          df
                        },
                        x)
  )
  if ((length(r) == 1 && is.na(r)) || is.null(r))
    x
  else
    r
}

parse2df <- function(x)
{
  plyr:::list_to_dataframe(
    str_split(
      str_split(x, ",")[[1]], "[:;]")[-1])
}


