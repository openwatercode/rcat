#' readCATInput: read from CAT input file to r varialble which is rcat_input class
#'
#' 텍스트로 구성된 CAT 모형을 읽어 "rcat_input" 클래스의 변수로 반환
#' @param filename CAT 모형의 텍스트형 입력자료
#' @param ... \code{\link{file}} 함수 참조
#' @return 입력파일의 내용이 들어 있는 \code{\link{rcat_input}} 클래스의 변수
#' @author 박희성 \email{hspark90@@i-fam.net}
#' @import plyr
#' @importFrom stringr str_split
#' @importFrom stringr str_detect
#' @encoding UTF-8
#' @export
#' @examples
#' download.file("http://r.prj.kr/data/rcat/rcat_ex.zip", "rcat_ex.zip")
#' unzip("rcat_ex.zip")
#'
#' model <- readCATInput("dist_101208.txt")
readCATInput <- function(filename, ...)
{
  f <- file(filename, ...)
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
                 ret <- mapply(function(c, v) cat_input_parse(c, v[1], v[2]),
                               c = cls, v = nl1, SIMPLIFY = F)
                 names(ret) <- unlist(lapply(nl1, function(y) y[1]))
                 ret <- plyr::compact(ret)
                 class(ret) <- paste0("cat_", nl[[1]][2])
                 ret
               })
  lapply(str_split(l2[1:(ns[1] - 1)], "[ ]*=[ ]*"),
         function(x) attr(n1, x[1]) <<- switch(x[1],
                          Version = x[2],
                          Title = x[2],
                          Background = x[2],
                          as.integer(comma_split(x[2]))))
  names(n1) <- 1:length(n1)
  class(n1) <- "rcat_input"
  n1
}


comma_split <- function(x) str_trim(str_split(x, ",")[[1]])

cat_input_parse <- function(c, k, x) {
  if(is.null(type <- getInputNodeType(c, k)))
    return(NULL);
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

getInputNodeType <- function (c, k) {
  switch(k,
    NodeID = "integer",
    Name = "character",
    Desc = "character",
    switch(
      c,
      cat_Junction = switch(
        k,
        Position = "integer",
        NULL
      ),
      cat_Outlet = switch(
        k,
        Position = "integer",
        NULL
      ),
      cat_Climate = switch(
        k,
        Calculation = "numeric",
        Evaporation = "character",
        Rainfall = "character",
        Position = "integer",
        NULL
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
        WL = "numeric",
        Position = "integer",
        NULL
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
        WL = "numeric",
        Position = "integer",
        NULL
      ),
      cat_Recycle = switch(k,
        Intake = "numeric",
        Nodes = "data.frame",
        Position = "integer",
        NULL
      ),
      cat_RainTank = switch(
        k,
        Series = "character",
        Supply = "integer",
        Use = "numeric",
        Volume = "numeric",
        Position = "integer",
        NULL
      ),
      cat_Infiltro = switch(k,
        Aquifer = "numeric",
        GWMove = "numeric",
        Position = "integer",
        NULL
      ),
      cat_Link = switch(
        k,
        Connect = "integer",
        Cunge = "numeric",
        Kinematic = "numeric",
        Method = "integer",
        Muskingum = "numeric",
        NULL
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
        Weather = "data.frame",
        Position = "integer",
        NULL
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
        Weather = "data.frame",
        Position = "integer",
        NULL
      ),
      cat_BioRetention = switch(
        k,
        Aquifer = "numeric",
        EVA = "numeric",
        Evaporation = "numeric",
        GWMove = "numeric",
        Rainfall = "numeric",
        Position = "integer",
        NULL
      ),
      cat_Import = switch(
        k,
        Constant = "numeric",
        Leakage = "numeric",
        Series = "character",
        Table = "integer",
        Type = "integer",
        Position = "integer",
        NULL
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
        Weather = "data.frame",
        Position = "integer",
        NULL
      ),
      NULL
    )
  )
}

parse2df <- function(x)
{
  list_to_dataframe(
    str_split(
      str_split(x, ",")[[1]], "[:;]")[-1])
}

list_to_dataframe <- function (res, labels = NULL, id_name = NULL, id_as_factor = FALSE)
{
  null <- vapply(res, is.null, logical(1))
  res <- res[!null]
  if (length(res) == 0)
    return(data.frame())
  if (!is.null(labels)) {
    stopifnot(nrow(labels) == length(null))
    labels <- labels[!null, , drop = FALSE]
  }
  names_res <- names(res)
  if (!is.null(id_name) && is.null(labels) && !is.null(names_res)) {
    stopifnot(length(id_name) == 1)
    if (id_as_factor)
      names_res <- factor(names_res, levels = unique(names_res))
    labels <- data.frame(.id = names_res, stringsAsFactors = FALSE)
    names(labels) <- id_name
  }
  atomic <- unlist(lapply(res, is.atomic))
  df <- unlist(lapply(res, is.data.frame))
  mat <- unlist(lapply(res, is.matrix))
  if (all(mat)) {
    resdf <- as.data.frame(rbind.fill.matrix(res))
    rows <- unlist(lapply(res, NROW))
  }
  else if (all(atomic)) {
    nrow <- length(res)
    ncol <- unique(unlist(lapply(res, length)))
    if (length(ncol) != 1)
      stop("Results do not have equal lengths")
    vec <- unname(do.call("c", res))
    resdf <- quickdf(unname(split(vec, rep(seq_len(ncol),
                                           nrow))))
    names(resdf) <- make_names(res[[1]], "V")
    rows <- rep(1, length(nrow))
  }
  else if (all(df)) {
    resdf <- rbind.fill(res)
    rows <- unlist(lapply(res, NROW))
  }
  else {
    stop("Results must be all atomic, or all data frames")
  }
  if (is.null(labels))
    return(unrowname(resdf))
  names(labels) <- make_names(labels, "X")
  cols <- setdiff(names(labels), names(resdf))
  labels <- labels[rep(1:nrow(labels), rows), cols, drop = FALSE]
  unrowname(cbind(labels, resdf))
}

make_names <- function (x, prefix = "X")
{
  nm <- names(x)
  if (is.null(nm)) {
    nm <- rep.int("", length(x))
  }
  n <- sum(nm == "", na.rm = TRUE)
  nm[nm == ""] <- paste(prefix, seq_len(n), sep = "")
  nm
}

