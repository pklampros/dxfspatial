context("Gallery tests")

library(dxfspatial)
library(Rcpp)

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
    expect_equal(nrow(galleryPlan@lines), 562)
    expect_equal(nrow(galleryPlan@polys), 2)
    expect_equal(nrow(galleryPlan@texts), 3)
    expect_equal(nrow(galleryPlan@points), 8)

    expectedBlocks = dataToDataFrame(data = c(
        "floor1", "7"),
        colnames=c("name","colour"),
        coltypes=c("character", "factor"));

    expect_equal(galleryPlan@blocks, expectedBlocks)

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

    expect_equal(galleryPlan@layers, expectedLayers)
})

