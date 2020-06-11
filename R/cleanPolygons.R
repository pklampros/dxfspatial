# Copyright 2018-2019 Petros Koutsolampros
# This file is part of dxfspatial.
#
# dxfspatial is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# dxfspatial is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with dxfspatial.  If not, see <https://www.gnu.org/licenses/>.

cleanPolygons = function(polys, aggregateOn) {
    polys@polygons = lapply(polys@polygons, function(polygons) {
        Polygons(lapply(polygons@Polygons, function(polygon) {
            Polygon(polygon@coords)
        }), ID = polygons@ID);
    })
    polys = rgeos::gBuffer(polys, byid = TRUE, width = 0)
    polys = rasterAggregatePolygonGroups(polys, aggregateOn, dissolve = F)
    polys@polygons = lapply(polys@polygons, removePolygonDupes)
    polys@polygons = lapply(polys@polygons, function(polygons) {
        checkPolygonsHoles(polygons, properly = T, useSTRtree = T)
    })
    raster::disaggregate(polys)
}

removePolygonDupes <- function(obj, properly=TRUE, force=TRUE) {
    # This function is a modified maptools::checkPolygonsHoles function
    # that properly removes duplicates
    if (!is(obj, "Polygons"))
        stop("not a Polygons object")
    if (!requireNamespace("rgeos", quietly = TRUE))
        stop("package rgeos required for checkPolygonsGEOS")
    comm <- try(rgeos::createPolygonsComment(obj), silent = TRUE)
    #    isVal <- try(gIsValid(SpatialPolygons(list(obj))), silent=TRUE)
    #    if (class(isVal) == "try-error") isVal <- FALSE
    if (class(comm) != "try-error" && !force) {
        comment(obj) <- comm
        return(obj)
    }
    pls <- slot(obj, "Polygons")
    IDs <- slot(obj, "ID")
    n <- length(pls)
    if (n < 1) stop("Polygon list of zero length")
    uniqs <- rep(TRUE, n)
    if (n > 1) {
        SP <- SpatialPolygons(lapply(1:n, function(i)
            Polygons(list(pls[[i]]), ID = i)))
        for (i in 1:(n - 1)) {
            res <- try(rgeos::gEquals(SP[i,], SP, byid = TRUE), silent = TRUE)
            if ("try-error" %in% class(res)) {
                warning("Polygons object ", IDs, ", Polygon ",
                        i, ": ", res)
                next
            }
            res[i] <- FALSE
            if (any(res)) {
                wres <- which(res)
                uniqs[wres[wres > i]] <- FALSE
            }
        }
    }
    if (any(!uniqs)) warning(paste("Duplicate Polygon objects dropped:",
                                   paste(wres, collapse = " ")))
    pls <- pls[uniqs]
    #    IDs <- IDs[uniqs]
    n <- length(pls)
    if (n < 1) stop("Polygon list of zero length")
    if (n == 1) {
        oobj <- Polygons(pls, ID = IDs)
        comment(oobj) <- rgeos::createPolygonsComment(oobj)
        return(oobj)
    }
    areas <- sapply(pls, slot, "area")
    pls <- pls[order(areas, decreasing = TRUE)]
    oholes <- sapply(pls, function(x) slot(x, "hole"))
    holes <- rep(FALSE, n)
    SP <- SpatialPolygons(lapply(1:n, function(i)
        Polygons(list(pls[[i]]), ID = i)))
    for (i in 1:(n - 1)) {
        if (properly) res <- rgeos::gContainsProperly(SP[i,], SP[-(1:i),],
                                                      byid = TRUE)
        else res <- rgeos::gContains(SP[i,], SP[-(1:i),], byid = TRUE)
        wres <- which(res)
        if (length(wres) > 0L) {
            nres <- as.integer(rownames(res))
            holes[nres[wres]] <- !holes[nres[wres]]
        }
    }
    for (i in 1:n) {
        if (oholes[i] != holes[i])
            pls[[i]] <- Polygon(slot(pls[[i]], "coords"), hole = holes[i])
    }
    oobj <- Polygons(pls, ID = IDs)
    comment(oobj) <- rgeos::createPolygonsComment(oobj)
    oobj
}

rasterAggregatePolygons = function(x, dissolve = F) {
    # this function comes from the raster package but modified to ignore the proj4string
    if (dissolve) {
        if (!requireNamespace("rgeos")) {
            warning('Cannot dissolve because the rgeos package is not available')
            dissolve <- FALSE
        }
        if (rgeos::version_GEOS() < "3.3.0") {
            x <- rgeos::gUnionCascaded(x)
        } else {
            x <- rgeos::gUnaryUnion(x)
        }
    } else {
        p <- list()
        for (i in 1:length(x)) {
            nsubobs <- length(x@polygons[[i]]@Polygons)
            p <- c(p, lapply(1:nsubobs, function(j) x@polygons[[i]]@Polygons[[j]]))
        }
        x <- SpatialPolygons(list(Polygons(p, '1')))
    }
}

rasterAggregatePolygonGroups = function(x, aggregateOn, dissolve = F) {
    # this function comes from the raster package but modified to ignore the proj4string
    if (dissolve) {
        if (!requireNamespace("rgeos")) {
            warning('Cannot dissolve because the rgeos package is not available')
            dissolve <- FALSE
        }
    }
    .getVars <- function(v, cn, nc) {
        vl <- length(v)
        v <- unique(v)
        if (is.numeric(v)) {
            v <- round(v)
            v <- v[v>0 & v <= nc]
            if (length(v) < 1) {
                stop('invalid column numbers')
            }
        } else if (is.character(v)) {
            v <- v[v %in% cn]
            if (length(v) < 1) {
                stop('invalid column names')
            }
        }
        v
    }
    hd <- .hasSlot(x, 'data');
    dat <- x@data
    cn <- colnames(dat)
    v <- .getVars(aggregateOn, cn)

    dat <- dat[,v, drop=FALSE]
    crs <- proj4string(x)
    dc <- apply(dat, 1, function(y) paste(as.character(y), collapse='_'))
    dc <- data.frame(oid=1:length(dc), v=as.integer(as.factor(dc)))
    id <- dc[!duplicated(dc$v), , drop=FALSE]

    if (nrow(id) == nrow(dat)) {
        # nothing to aggregate
        if (hd) {
            x@data <- dat
        } else {
            x <- as(x, 'SpatialPolygons')
        }
        return(x)
    }

    id <- id[order(id$v), ]
    dat <- dat[id[,1], ,drop=FALSE]


    if (hd) {
        x <- as(x, 'SpatialPolygons')
    }

    if (dissolve) {
        if (rgeos::version_GEOS0() < "3.3.0") {
            x <- lapply(1:nrow(id), function(y) spChFIDs(rgeos::gUnionCascaded(x[dc[dc$v==y,1],]), as.character(y)))
        } else {

            x <- lapply(1:nrow(id),
                        function(y) {
                            z <- x[dc[dc$v==y, 1], ]
                            z <- try( rgeos::gUnaryUnion(z) )
                            if (! inherits(z, "try-error")) {
                                spChFIDs(z, as.character(y))
                            }
                        }
            )
        }
    } else {
        x <- lapply(1:nrow(id), function(y) spChFIDs(rasterAggregatePolygons(x[dc[dc$v==y,1],], dissolve=FALSE), as.character(y)))
    }

    x <- do.call(rbind, x)
    crs(x) <- crs
    rownames(dat) <- NULL
    SpatialPolygonsDataFrame(x, dat, FALSE)
}
