#' plot.rcat_series: plot the rcat_series
#'
#' "rcat_series" 클래스 자료를 그래프 형태로 보여주는 함수.
#' @param x rcat 모형에서 사용하는 "rcat_series" 클래스의 변수
#' @return NULL
#' @author 박희성 \email{hspark90@@i-fam.net}
#' @import xts
#' @encoding UTF-8
#' @export
#' @examples
#' download.file("http://r.prj.kr/data/rcat/rcat_ex.zip", "rcat_ex.zip")
#' unzip("rcat_ex.zip")
#' iseri <- readBinCATSerieses("P_climate_1209.dat")
#' plot(iseri[[1]])
plot.rcat_series <- function(x, ...)
{
  plot_type = getOutputNodeType(x)

  bak <- par("mfrow")
  st <- do.call("ISOdatetime", as.list(c(attr(x, "StartTime"), 0)))
  tseq <- seq(st, by = paste(attr(x, "Interval"), "min"), length.out = nrow(x))
  if(length(plot_type) == 0)
  {
    n <- ncol(x)
    c <- ceiling(sqrt(n))
    r <- ceiling(n / c)
    par(mfrow = c(c, r))
    for (i in 1:n)
    {
      plot(
        tseq, x[, i], main = names(x)[i], xlab = "Time", ylab = names(x)[i],
        type = "l", ...
      )
    }
  } else {
    switch(
      plot_type,
      Junc =  {
        n <- ncol(x)
        c <- ceiling(sqrt(n))
        r <- ceiling(n / c)
        par(mfrow = c(c, r))
        for (i in 1:n)
        {
          plot(
            tseq, x[, i], main = names(x)[i], xlab = "Time", ylab = names(x)[i],
            type = "l", ...
          )
        }
      },
      Wetland = {
        n <- ncol(x)
        c <- ceiling(sqrt(n))
        r <- ceiling(n / c)
        par(mfrow = c(c, r))
        for (i in 1:n)
        {
          plot(
            tseq, x[, i], main = names(x)[i], xlab = "Time", ylab = names(x)[i],
            type = "l", ...
          )
        }
      },
      Pond = {
        n <- ncol(x)
        c <- ceiling(sqrt(n))
        r <- ceiling(n / c)
        par(mfrow = c(c, r))
        for (i in 1:n)
        {
          plot(
            tseq, x[, i], main = names(x)[i], xlab = "Time", ylab = names(x)[i],
            type = "l", ...
          )
        }
      },
      Recycle = {
        n <- ncol(x)
        c <- ceiling(sqrt(n))
        r <- ceiling(n / c)
        par(mfrow = c(c, r))
        for (i in 1:n)
        {
          plot(
            tseq, x[, i], main = names(x)[i], xlab = "Time", ylab = names(x)[i],
            type = "l", ...
          )
        }
      },
      RainTank =  {
        n <- ncol(x)
        c <- ceiling(sqrt(n))
        r <- ceiling(n / c)
        par(mfrow = c(c, r))
        for (i in 1:n)
        {
          plot(
            tseq, x[, i], main = names(x)[i], xlab = "Time", ylab = names(x)[i],
            type = "l", ...
          )
        }
      },
      Infiltro = {
        n <- ncol(x)
        c <- ceiling(sqrt(n))
        r <- ceiling(n / c)
        par(mfrow = c(c, r))
        for (i in 1:n)
        {
          plot(
            tseq, x[, i], main = names(x)[i], xlab = "Time", ylab = names(x)[i],
            type = "l", ...
          )
        }
      },
      Link =  {
        n <- ncol(x)
        c <- ceiling(sqrt(n))
        r <- ceiling(n / c)
        par(mfrow = c(c, r))
        for (i in 1:n)
        {
          plot(
            tseq, x[, i], main = names(x)[i], xlab = "Time", ylab = names(x)[i],
            type = "l", ...
          )
        }
      },
      Forest =  {
        n <- ncol(x)
        c <- ceiling(sqrt(n))
        r <- ceiling(n / c)
        par(mfrow = c(c, r))
        for (i in 1:n)
        {
          plot(
            tseq, x[, i], main = names(x)[i], xlab = "Time", ylab = names(x)[i],
            type = "l", ...
          )
        }
      },
      Paddy = {
        n <- ncol(x)
        c <- ceiling(sqrt(n))
        r <- ceiling(n / c)
        par(mfrow = c(c, r))
        for (i in 1:n)
        {
          plot(
            tseq, x[, i], main = names(x)[i], xlab = "Time", ylab = names(x)[i],
            type = "l", ...
          )
        }
      },
      BioRetention = {
        n <- ncol(x)
        c <- ceiling(sqrt(n))
        r <- ceiling(n / c)
        par(mfrow = c(c, r))
        for (i in 1:n)
        {
          plot(
            tseq, x[, i], main = names(x)[i], xlab = "Time", ylab = names(x)[i],
            type = "l", ...
          )
        }
      },
      Import = {
        n <- ncol(x)
        c <- ceiling(sqrt(n))
        r <- ceiling(n / c)
        par(mfrow = c(c, r))
        for (i in 1:n)
        {
          plot(
            tseq, x[, i], main = names(x)[i], xlab = "Time", ylab = names(x)[i],
            type = "l", ...
          )
        }
      },
      Urban = {
        n <- ncol(x)
        c <- ceiling(sqrt(n))
        r <- ceiling(n / c)
        par(mfrow = c(c, r))
        for (i in 1:n)
        {
          plot(
            tseq, x[, i], main = names(x)[i], xlab = "Time", ylab = names(x)[i],
            type = "l", ...
          )
        }
      }
    )
  }
  par(mfrow = bak)
}

getOutputNodeType <- function (x) {
  names(which(sapply(reflst[which(sapply(reflst, length) == length(x))],
                     function(y) all(y == names(x)))))
}

reflst <-
  list(
    Junc = c(
      "flow_total(㎥/s)", "flow_sf(㎥/s)", "flow_inter(㎥/s)",
      "flow_gw(㎥/s)", "flow_drain(㎥/s)"
    ),
    Link = c("flow_in(㎥/s)", "flow_out(㎥/s)"),
    Wetland = c(
      "Inflow(㎥)", "Rainfall(mm)", "Evaporation Water surface(mm)",
      "Bypass Volume(㎥)", "Spillway Outflow(㎥)", "Pipe Outflow(㎥)",
      "Aquifer Loss(㎥)", "Volume(㎥)", "Water Level(m)"
    ),
    Pond = c(
      "Inflow(㎥)", "Rainfall(mm)", "Evaporation Water surface(mm)",
      "Intake(㎥)", "Downstream Outflow(㎥)", "Spillway Outflow(㎥)",
      "Pipe Outflow(㎥)", "Aquifer Loss(㎥)", "Volume(㎥)",
      "Water Level(m)"
    ),
    Recycle = c(
      "Inflow(㎥)", "Intake(㎥)", "Downstream Outflow(㎥)",
      "Urban 5(㎥)", "Paddy(㎥)"
    ),
    RainTank = c(
      "Inflow(㎥)", "Rain Tank Inflow(㎥)", "Water Use(㎥)",
      "Spillway Outflow(㎥)", "Downstream Outflow(㎥)", "Volume(㎥)"
    ),
    Infiltro = c("Inflow(㎥)", "Import(㎥)", "Infiltrate(㎥)",
                 "Outflow(㎥)"),
    Forest = c(
      "Inflow(㎥)", "Rainfall(mm)", "Actual Evapotranspiration(mm)",
      "Potential Evapotranspiration(mm)", "Flow_Surface(mm)", "Flow_Inter(mm)",
      "Flow_Groundwater(mm)", "Flow_Total(mm)", "Infiltrate(mm)",
      "Recharge(mm)", "Groundwater Movement(mm)", "Soil Moisture Content",
      "Groundwater Elevation(EL.m)", "dep_imp(mm)", "dep_per(mm)"
    ),
    Paddy = c(
      "Inflow(㎥)", "Rainfall(mm)", "Actual Evapotranspiration(mm)",
      "Potential Evapotranspiration(mm)", "Flow_Surface(mm)", "Flow_Inter(mm)",
      "Flow_Groundwater(mm)", "Surf Drainage Culvert(mm)", "Surf Drainage Pipe(mm)",
      "Flow_Total(mm)", "Infiltrate(mm)", "Recharge(mm)",
      "Groundwater Movement(mm)", "Soil Moisture Content", "Ponding Depth(mm)",
      "Groundwater Elevation(EL.m)"
    ),
    BioRetention = c(
      "Inflow(㎥)", "Rainfall(mm)", "Total Inflow(㎥)",
      "Evapotranspiration(mm)", "Infiltrate(㎥)", "Outflow(㎥)"
    ),
    Import = c("Water Supply(㎥)"),
    Urban = c(
      "Inflow(㎥)", "Rainfall(mm)", "Actual Evapotranspiration(mm)",
      "Potential Evapotranspiration(mm)", "Flow_Surface(mm)", "Flow_Inter(mm)",
      "Flow_Groundwater(mm)", "Flow_Total(mm)", "Infiltrate(mm)",
      "Recharge(mm)", "Groundwater Movement(mm)", "Soil Moisture Content",
      "Groundwater Elevation(EL.m)", "dep_imp(mm)", "dep_per(mm)"
    )
  )
