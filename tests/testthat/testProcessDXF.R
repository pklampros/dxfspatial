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

context("Gallery tests")

library(dxfspatial)

dataToDataFrame = function(data, colnames, coltypes = c()) {
    if(length(coltypes) == 0) coltypes = rep("character", length(colnames))
    df = as.data.frame(t(matrix(data, nrow=length(colnames))), stringsAsFactors = F)
    names(df) = colnames;
    for(i in 1:length(colnames)) {
        if(coltypes[i] == "factor") {
            df[,colnames[i]] = factor(df[,colnames[i]]);
        } else {
            df[,colnames[i]] = as(df[,colnames[i]], coltypes[i]);
        }
    }
    return(df)
}

test_that("Load plan from gallery", {
    galleryPlan = processDXF("gallery.dxf", 1, "@_");
    expect_equal(nrow(galleryPlan$lines), 562)
    expect_equal(nrow(galleryPlan$polys), 2)
    expect_equal(nrow(galleryPlan$texts), 3)
    expect_equal(nrow(galleryPlan$points), 8)

    expectedBlocks = dataToDataFrame(data = c(
        "floor1", "7"),
        colnames=c("name","colour"),
        coltypes=c("character", "factor"));

    expect_equal(galleryPlan$blocks, expectedBlocks)

    expectedLayers = dataToDataFrame(data = c(
        "acc-gallery1", "poly", "3",
        "acc-gallery2", "poly", "4",
        "gallery",      "line", "2",
        "acc-gallery1", "text", "3",
        "acc-gallery2", "text", "4",
        "info",         "text", "6",
        "infopoints",   "point", "5"),
        colnames=c("name","type","colour"),
        coltypes=c("character", "factor", "factor"));

    expect_equal(galleryPlan$layers, expectedLayers)
})

