#include <Rcpp.h>
#include <memory>

Rcpp::S4 getSpatialLinesDataFrame(std::vector<std::vector<double> > lineCoords, Rcpp::DataFrame data) {

    Rcpp::S4 dataFrame("SpatialLinesDataFrame");
    if(lineCoords.size() == 0) {
        return dataFrame;
    }
    dataFrame.slot("data") = data;

    double minX = lineCoords[0][0], minY = lineCoords[0][1], maxX = lineCoords[0][0], maxY = lineCoords[0][1];

    Rcpp::List lines(lineCoords.size());

    for(int i = 0; size_t(i) < lineCoords.size(); i++) {
        Rcpp::S4 linesWrapper("Lines");
        Rcpp::List linesWrapperList(1);
        Rcpp::S4 line("Line");
        Rcpp::NumericMatrix coords(2, 2);
        coords[0] = lineCoords[i][0];
        coords[1] = lineCoords[i][2];
        coords[2] = lineCoords[i][1];
        coords[3] = lineCoords[i][3];
        if(coords[0] < minX) minX = coords[0];
        if(coords[2] < minY) minY = coords[2];
        if(coords[0] > maxX) maxX = coords[0];
        if(coords[2] > maxY) maxY = coords[2];

        if(coords[1] < minX) minX = coords[1];
        if(coords[3] < minY) minY = coords[3];
        if(coords[1] > maxX) maxX = coords[1];
        if(coords[3] > maxY) maxY = coords[3];
        line.slot("coords") = coords;
        linesWrapperList[0] = line;

        linesWrapper.slot("Lines") = linesWrapperList;
        linesWrapper.slot("ID") = std::to_string(i);
        lines[i] = linesWrapper;
    }

    dataFrame.slot("lines") = lines;

    Rcpp::NumericMatrix bbox(2, 2);
    Rcpp::List dimnames(2);
    dimnames[0] = Rcpp::CharacterVector::create("x", "y");
    dimnames[1] = Rcpp::CharacterVector::create("min", "max");
    bbox.attr("dimnames") = dimnames;
    bbox[0] = minX;
    bbox[1] = minY;
    bbox[2] = maxX;
    bbox[3] = maxY;
    dataFrame.slot("bbox") = bbox;

    Rcpp::S4 proj4string("CRS");
    proj4string.slot("projargs") = Rcpp::CharacterVector::create(NA_STRING);
    dataFrame.slot("proj4string") = proj4string;

    return dataFrame;
}


Rcpp::S4 getSpatialPolygonsDataFrame(std::vector<std::vector<double> > polyCoords, Rcpp::DataFrame data) {

    Rcpp::S4 dataFrame("SpatialPolygonsDataFrame");
    if(polyCoords.size() == 0) {
        return dataFrame;
    }
    dataFrame.slot("data") = data;

    double minX = polyCoords[0][0], minY = polyCoords[0][1], maxX = polyCoords[0][0], maxY = polyCoords[0][1];

    Rcpp::List polygons(polyCoords.size());

    for(int i = 0; size_t(i) < polyCoords.size(); i++) {
        Rcpp::S4 polygonsWrapper("Polygons");
        Rcpp::List polygonsWrapperList(1);
        Rcpp::S4 polygon("Polygon");
        int nv = int(polyCoords[i].size()/2);
        Rcpp::NumericMatrix coords(nv, 2);
        for(int j = 0; j < nv; ++j) {
            coords[j] = polyCoords[i][j*2];
            coords[j+nv] = polyCoords[i][j*2+1];
            if(coords[j] < minX) minX = coords[j];
            if(coords[j+nv] < minY) minY = coords[j+nv];
            if(coords[j] > maxX) maxX = coords[j];
            if(coords[j+nv] > maxY) maxY = coords[j+nv];
        }
        polygon.slot("coords") = coords;
        polygonsWrapperList[0] = polygon;

        polygonsWrapper.slot("Polygons") = polygonsWrapperList;
        polygonsWrapper.slot("ID") = std::to_string(i);
        polygons[i] = polygonsWrapper;
    }

    dataFrame.slot("polygons") = polygons;

    Rcpp::NumericMatrix bbox(2, 2);
    Rcpp::List dimnames(2);
    dimnames[0] = Rcpp::CharacterVector::create("x", "y");
    dimnames[1] = Rcpp::CharacterVector::create("min", "max");
    bbox.attr("dimnames") = dimnames;
    bbox[0] = minX;
    bbox[1] = minY;
    bbox[2] = maxX;
    bbox[3] = maxY;
    dataFrame.slot("bbox") = bbox;

    dataFrame.slot("plotOrder") = Rcpp::seq(1, polyCoords.size());

    Rcpp::S4 proj4string("CRS");
    proj4string.slot("projargs") = Rcpp::CharacterVector::create(NA_STRING);
    dataFrame.slot("proj4string") = proj4string;

    return dataFrame;
}


Rcpp::S4 getSpatialPointsDataFrame(std::vector<double> pointCoords, Rcpp::DataFrame data) {

    Rcpp::S4 dataFrame("SpatialPointsDataFrame");
    if(pointCoords.size() == 0) {
        return dataFrame;
    }
    dataFrame.slot("data") = data;

    double minX = pointCoords[0], minY = pointCoords[1], maxX = pointCoords[0], maxY = pointCoords[1];

    Rcpp::List polygons(pointCoords.size());

    int nv = int(pointCoords.size()/2);
    Rcpp::NumericMatrix coords(nv, 2);
    for(int j = 0; j < nv; ++j) {
        coords[j] = pointCoords[j*2];
        coords[j+nv] = pointCoords[j*2+1];
        if(coords[j] < minX) minX = coords[j];
        if(coords[j+nv] < minY) minY = coords[j+nv];
        if(coords[j] > maxX) maxX = coords[j];
        if(coords[j+nv] > maxY) maxY = coords[j+nv];
    }
    dataFrame.slot("coords") = coords;

    Rcpp::NumericMatrix bbox(2, 2);
    Rcpp::List dimnames(2);
    dimnames[0] = Rcpp::CharacterVector::create("x", "y");
    dimnames[1] = Rcpp::CharacterVector::create("min", "max");
    bbox.attr("dimnames") = dimnames;
    bbox[0] = minX;
    bbox[1] = minY;
    bbox[2] = maxX;
    bbox[3] = maxY;
    dataFrame.slot("bbox") = bbox;

    Rcpp::S4 proj4string("CRS");
    proj4string.slot("projargs") = Rcpp::CharacterVector::create(NA_STRING);
    dataFrame.slot("proj4string") = proj4string;

    return dataFrame;
}
