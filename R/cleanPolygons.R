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
    polys = raster::aggregate(polys, by = aggregateOn, dissolve = F)
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
            if (class(res) == "try-error") {
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
