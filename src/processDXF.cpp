// Copyright 2018-2019 Petros Koutsolampros
// This file is part of dxfspatial.
//
// dxfspatial is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// dxfspatial is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with dxfspatial.  If not, see <https://www.gnu.org/licenses/>.

#include "getspatialdataframe.h"
#include "dxfdata.h"

#include <Rcpp.h>
#include <sp.h>

Rcpp::StringVector toStringVector(std::vector<std::string> str) {
    Rcpp::StringVector strv(str.size());
    for(size_t i = 0; i < str.size(); i++) {
        strv[i] = str[i];
    }
    return strv;
}

// [[Rcpp::export]]
Rcpp::List getDXFData(std::string file, std::string prefix, double scale) {
    std::unique_ptr<DXFData> dxf(new DXFData(file, prefix, scale));

    Rcpp::DataFrame lineData = Rcpp::DataFrame::create(
        Rcpp::Named("layer") = dxf->lineLayer,
        Rcpp::Named("layertype") = dxf->lineLayerType,
        Rcpp::Named("block") = dxf->lineBlock,
        Rcpp::Named("colour") = dxf->lineColour
    );

    Rcpp::DataFrame polyData = Rcpp::DataFrame::create(
        Rcpp::Named("layer") = dxf->polyLayer,
        Rcpp::Named("layertype") = dxf->polyLayerType,
        Rcpp::Named("block") = dxf->polyBlock,
        Rcpp::Named("colour") = dxf->polyColour
    );

    Rcpp::DataFrame textData = Rcpp::DataFrame::create(
        Rcpp::Named("layer") = dxf->textLayer,
        Rcpp::Named("layertype") = dxf->textLayerType,
        Rcpp::Named("block") = dxf->textBlock,
        Rcpp::Named("colour") = dxf->textColour,
        Rcpp::Named("text") = dxf->textText
    );

    Rcpp::DataFrame pointData = Rcpp::DataFrame::create(
        Rcpp::Named("layer") = dxf->pointLayer,
        Rcpp::Named("layertype") = dxf->pointLayerType,
        Rcpp::Named("block") = dxf->pointBlock,
        Rcpp::Named("colour") = dxf->pointColour
    );

    Rcpp::DataFrame blocks = Rcpp::DataFrame::create(
        Rcpp::Named("name") = dxf->blocks,
        Rcpp::Named("colour") = dxf->blockColour
    );

    Rcpp::DataFrame layers = Rcpp::DataFrame::create(
        Rcpp::Named("name") = dxf->layers,
        Rcpp::Named("type") = dxf->layerType,
        Rcpp::Named("colour") = dxf->layerColour
    );


    return Rcpp::List::create(
        Rcpp::Named("lines") = getSpatialLinesDataFrame(dxf->lines, lineData),
        Rcpp::Named("polys") = getSpatialPolygonsDataFrame(dxf->polys, polyData),
        Rcpp::Named("texts") = getSpatialPointsDataFrame(dxf->texts, textData),
        Rcpp::Named("points") = getSpatialPointsDataFrame(dxf->points, pointData),
        Rcpp::Named("blocks") = blocks,
        Rcpp::Named("layers") = layers
    );
}
