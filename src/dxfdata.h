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

#pragma once

#include "stringutils.h"

#include <vector>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <iostream>
#include <algorithm>
#include <map>
#include <math.h>

class DXFData {

public:

    std::vector<std::vector<double> > lines;
    std::vector<std::string> lineLayer;
    std::vector<std::string> lineLayerType;
    std::vector<std::string> lineBlock;
    std::vector<std::string> lineColour;

    std::vector<std::vector<double> > polys;
    std::vector<std::string> polyLayer;
    std::vector<std::string> polyLayerType;
    std::vector<std::string> polyBlock;
    std::vector<std::string> polyColour;

    std::vector<double> texts;
    std::vector<std::string> textLayer;
    std::vector<std::string> textLayerType;
    std::vector<std::string> textBlock;
    std::vector<std::string> textText;
    std::vector<std::string> textColour;

    std::vector<double> points;
    std::vector<std::string> pointLayer;
    std::vector<std::string> pointLayerType;
    std::vector<std::string> pointBlock;
    std::vector<std::string> pointColour;

    std::vector<std::string> blocks;
    std::vector<std::string> blockColour;
    std::vector<std::string> layers;
    std::vector<std::string> layerColour;
    std::vector<std::string> layerType;


    void addOffsetToCoords(std::map<std::string, std::vector<double> > &offsets,
                           std::vector<std::string> &keys,
                           std::vector<std::vector<double> > &coordsToBeOffset) {
        for(size_t i = 0; i < keys.size(); i++) {
            if (offsets.count(keys[i]) == 0 || offsets[keys[i]].size() == 0) {
                Rcpp::Rcerr << keys[i] << " block found but not inserted" << std::endl;
                continue;
            }
            std::vector<double> &offsetCoords = offsets[keys[i]];

            for (size_t j = 0; j < coordsToBeOffset[i].size(); j+=2) {
                coordsToBeOffset[i][j] += offsetCoords[0];
                coordsToBeOffset[i][j+1] += offsetCoords[1];
            }
        }
    }

    void addOffsetToCoords(std::map<std::string, std::vector<double> > &offsets,
                           std::vector<std::string> &keys,
                           std::vector<double> &coordsToBeOffset) {
        for(size_t i = 0; i < keys.size(); i++) {
            if (offsets.count(keys[i]) == 0 || offsets[keys[i]].size() == 0) {
                Rcpp::Rcerr << keys[i] << " block found but not inserted" << std::endl;
                continue;
            }
            std::vector<double> &offsetCoords = offsets[keys[i]];

            coordsToBeOffset[i*2] += offsetCoords[0];
            coordsToBeOffset[i*2+1] += offsetCoords[1];
        }
    }

    DXFData(const std::string & file, const std::string & prefix, double scale) {

        std::ifstream infile(file);

        std::string defaultColour = "7";
        std::string defaultLayerName = "0";

        bool isblock = false;
        bool islwpoly = false;
        bool iscircle = false;
        bool isarc = false;
        bool isline = false;
        bool ismtext = false;
        bool ispoint = false;
        bool ispoly = false;
        bool ispolyfinished = false;
        bool isvertex = false;
        bool isinsert = false;
        bool islayer = false;

        int circleSides = 32;
        double radius, startangle, endangle;

        // at the moment the way this is used is to flip the lwpolys, circles and arcs by multiplying
        // extrudeZ with them though there is certainly a better way.
        double extrudeX = 1, extrudeY = 1, extrudeZ = 1;

        std::vector<double> coordsX;
        std::vector<double> coordsY;
        std::string layer = defaultLayerName;
        std::string colour = defaultColour;

        std::string blockname = "";
        std::string blockcolour = "";
        std::string currtext;
        bool polyclosed = false;

        std::string lineLayerIdentifier = "line";
        std::string polyLayerIdentifier = "poly";
        std::string textLayerIdentifier = "text";
        std::string pointLayerIdentifier = "point";

        std::string formattedLineLayerIdentifier = prefix + lineLayerIdentifier + "-";
        std::string formattedPolyLayerIdentifier = prefix + polyLayerIdentifier + "-";
        std::string formattedTextLayerIdentifier = prefix + textLayerIdentifier + "-";
        std::string formattedPointLayerIdentifier = prefix + pointLayerIdentifier + "-";

        std::vector<std::string> lineBlockName;
        std::vector<std::string> polyBlockName;
        std::vector<std::string> textBlockName;
        std::vector<std::string> pointBlockName;

        std::map<std::string, std::vector<double>> blockInsertCoords;

        std::string keystr;
        std::string value;
        while (stringutils::safeGetline(infile, keystr)) {
            stringutils::ltrim(keystr, ' ');
            stringutils::rtrim(keystr, ' ');
            if(keystr.length() == 0) continue;
            stringutils::safeGetline(infile, value);
            stringutils::ltrim(value, ' ');
            stringutils::rtrim(value, ' ');
            int key = std::stoi(keystr);
            if(key == 0) {
                if(islayer) {
                    if(currtext.substr(0, prefix.length()) == prefix) {
                        if(stringutils::startsWith(currtext, formattedLineLayerIdentifier)) {
                            layers.push_back(currtext.substr(formattedLineLayerIdentifier.length(), currtext.length()));
                            layerType.push_back(lineLayerIdentifier);
                            layerColour.push_back(colour);
                        }
                        if(stringutils::startsWith(currtext, formattedPolyLayerIdentifier)) {
                            layers.push_back(currtext.substr(formattedPolyLayerIdentifier.length(), currtext.length()));
                            layerType.push_back(polyLayerIdentifier);
                            layerColour.push_back(colour);
                        }
                        if(stringutils::startsWith(currtext, formattedTextLayerIdentifier)) {
                            layers.push_back(currtext.substr(formattedTextLayerIdentifier.length(), currtext.length()));
                            layerType.push_back(textLayerIdentifier);
                            layerColour.push_back(colour);
                        }
                        if(stringutils::startsWith(currtext, formattedPointLayerIdentifier)) {
                            layers.push_back(currtext.substr(formattedPointLayerIdentifier.length(), currtext.length()));
                            layerType.push_back(pointLayerIdentifier);
                            layerColour.push_back(colour);
                        }
                    }
                    islayer = false;
                } else if(isblock && (islwpoly || ispolyfinished)) {
                    if(stringutils::startsWith(layer, prefix) && stringutils::startsWith(blockname, prefix)) {

                        // first check if polygon, if not treat as line

                        if(stringutils::startsWith(layer, formattedPolyLayerIdentifier)) {
                            // ignore polygons with less than 3 points
                            if(coordsX.size() > 2 && coordsY.size() > 2) {
                                std::string layername = layer.substr(formattedPolyLayerIdentifier.length(), layer.length());
                                std::string layertype = polyLayerIdentifier;
                                std::string spacename = blockname.substr(prefix.length(), blockname.length());
                                polyLayer.push_back(layername);
                                polyLayerType.push_back(layertype);
                                polyBlock.push_back(spacename);
                                polyBlockName.push_back(blockname);
                                polyColour.push_back(colour);
                                std::vector<double> coords;
                                for(size_t i = 0; i < coordsX.size(); i++) {
                                    coords.push_back(extrudeZ * coordsX[i]);
                                    coords.push_back(coordsY[i]);
                                }
                                polys.push_back(coords);
                             }
                             coordsX.clear();
                             coordsY.clear();
                         } else if (stringutils::startsWith(layer, formattedLineLayerIdentifier)) {
                             std::string layername = layer.substr(formattedLineLayerIdentifier.length(), layer.length());
                             std::string layertype = lineLayerIdentifier;
                             std::string spacename = blockname.substr(prefix.length(), blockname.length());

                             for(size_t i = 0; i < coordsX.size()-1; i++) {
                                 std::vector<double> coords;
                                 coords.push_back(extrudeZ * coordsX[i]);
                                 coords.push_back(coordsY[i]);
                                 coords.push_back(extrudeZ * coordsX[i+1]);
                                 coords.push_back(coordsY[i+1]);
                                 lines.push_back(coords);
                                 lineLayer.push_back(layername);
                                 lineLayerType.push_back(layertype);
                                 lineBlock.push_back(spacename);
                                 lineBlockName.push_back(blockname);
                                 lineColour.push_back(colour);
                             }
                             if(polyclosed) {
                                 std::vector<double> coords;
                                 coords.push_back(extrudeZ * coordsX[coordsX.size()-1]);
                                 coords.push_back(coordsY[coordsX.size()-1]);
                                 coords.push_back(extrudeZ * coordsX[0]);
                                 coords.push_back(coordsY[0]);
                                 lines.push_back(coords);
                                 lineLayer.push_back(layername);
                                 lineLayerType.push_back(layertype);
                                 lineBlock.push_back(spacename);
                                 lineBlockName.push_back(blockname);
                                 lineColour.push_back(colour);
                             }
                             coordsX.clear();
                             coordsY.clear();
                          }
                    }
                    islwpoly = false;
                    ispolyfinished = false;
                    polyclosed = false;
                    layer = defaultLayerName;
                    colour = defaultColour;
                    extrudeX = 1;
                    extrudeY = 1;
                    extrudeZ = 1;
                }
                if(isblock && ismtext) {
                    if(stringutils::startsWith(layer, formattedTextLayerIdentifier)
                           && stringutils::startsWith(blockname, prefix)) {
                        std::string layername = layer.substr(formattedTextLayerIdentifier.length(), layer.length());
                        std::string layertype = textLayerIdentifier;
                        std::string spacename = blockname.substr(prefix.length(), blockname.length());
                        texts.push_back(coordsX[0]);
                        texts.push_back(coordsY[0]);
                        textLayer.push_back(layername);
                        textLayerType.push_back(layertype);
                        textBlock.push_back(spacename);
                        textText.push_back(currtext);
                        textBlockName.push_back(blockname);
                        textColour.push_back(colour);
                        coordsX.clear();
                        coordsY.clear();
                    }
                    ismtext = false;
                    currtext = "";
                    layer = defaultLayerName;
                    colour = defaultColour;
                }
                if(isblock && ispoint) {
                    if(stringutils::startsWith(layer, formattedPointLayerIdentifier)
                           && stringutils::startsWith(blockname, prefix)) {
                        std::string layername = layer.substr(formattedPointLayerIdentifier.length(), layer.length());
                        std::string layertype = pointLayerIdentifier;
                        std::string spacename = blockname.substr(prefix.length(), blockname.length());
                        points.push_back(coordsX[0]);
                        points.push_back(coordsY[0]);
                        pointLayer.push_back(layername);
                        pointLayerType.push_back(layertype);
                        pointBlock.push_back(spacename);
                        pointBlockName.push_back(blockname);
                        pointColour.push_back(colour);
                        coordsX.clear();
                        coordsY.clear();
                    }
                    ispoint = false;
                    layer = defaultLayerName;
                    colour = defaultColour;
                }
                if(isblock && isline) {
                    if(stringutils::startsWith(layer, formattedLineLayerIdentifier)
                           && stringutils::startsWith(blockname, prefix)) {
                        std::string layername = layer.substr(formattedLineLayerIdentifier.length(), layer.length());
                        std::string layertype = lineLayerIdentifier;
                        std::string spacename = blockname.substr(prefix.length(), blockname.length());

                        std::vector<double> coords;
                        coords.push_back(coordsX[0]);
                        coords.push_back(coordsY[0]);
                        coords.push_back(coordsX[1]);
                        coords.push_back(coordsY[1]);
                        lines.push_back(coords);
                        lineLayer.push_back(layername);
                        lineLayerType.push_back(layertype);
                        lineBlock.push_back(spacename);
                        lineBlockName.push_back(blockname);
                        lineColour.push_back(colour);
                        coordsX.clear();
                        coordsY.clear();
                    }
                    isline = false;
                    layer = defaultLayerName;
                    colour = defaultColour;
                }
                if(isblock && iscircle) {
                    if(stringutils::startsWith(layer, formattedLineLayerIdentifier)
                           && stringutils::startsWith(blockname, prefix)) {
                        std::string layername = layer.substr(formattedLineLayerIdentifier.length(), layer.length());
                        std::string layertype = lineLayerIdentifier;
                        std::string spacename = blockname.substr(prefix.length(), blockname.length());

                        double angle = 2 * M_PI / circleSides;
                        for(int i = 0; i < circleSides; i++) {
                            std::vector<double> coords;
                            coords.push_back(extrudeZ * (coordsX[0] + radius * cos(i * angle)));
                            coords.push_back(coordsY[0] + radius * sin(i * angle));
                            coords.push_back(extrudeZ * (coordsX[0] + radius * cos((i + 1) * angle)));
                            coords.push_back(coordsY[0] + radius * sin((i + 1) * angle));
                            lines.push_back(coords);
                            lineLayer.push_back(layername);
                            lineLayerType.push_back(layertype);
                            lineBlock.push_back(spacename);
                            lineBlockName.push_back(blockname);
                            lineColour.push_back(colour);
                        }
                        coordsX.clear();
                        coordsY.clear();
                    }
                    iscircle = false;
                    layer = defaultLayerName;
                    colour = defaultColour;
                    extrudeX = 1;
                    extrudeY = 1;
                    extrudeZ = 1;
                }

                if(isblock && isarc) {
                    if(stringutils::startsWith(layer, formattedLineLayerIdentifier)
                           && stringutils::startsWith(blockname, prefix)) {
                        std::string layername = layer.substr(formattedLineLayerIdentifier.length(), layer.length());
                        std::string layertype = lineLayerIdentifier;
                        std::string spacename = blockname.substr(prefix.length(), blockname.length());

                        if (startangle > endangle) {
                            endangle += 360;
                        }
                        int arcSides = (startangle == endangle) ? circleSides : (int(endangle - startangle) * circleSides / 360);
                        if(arcSides == 0) arcSides = 1;
                        startangle = 2 * M_PI * startangle / 360;
                        endangle = 2 * M_PI * endangle / 360;
                        double angle = (endangle - startangle) / arcSides;
                        for(int i = 0; i < arcSides; i++) {
                            std::vector<double> coords;
                            coords.push_back(extrudeZ * (coordsX[0] + radius * cos(startangle + i * angle)));
                            coords.push_back(coordsY[0] + radius * sin(startangle + i * angle));
                            coords.push_back(extrudeZ * (coordsX[0] + radius * cos(startangle + (i + 1) * angle)));
                            coords.push_back(coordsY[0] + radius * sin(startangle + (i + 1) * angle));
                            lines.push_back(coords);
                            lineLayer.push_back(layername);
                            lineLayerType.push_back(layertype);
                            lineBlock.push_back(spacename);
                            lineBlockName.push_back(blockname);
                            lineColour.push_back(colour);
                        }
                        coordsX.clear();
                        coordsY.clear();
                    }
                    isarc = false;
                    layer = defaultLayerName;
                    colour = defaultColour;
                    extrudeX = 1;
                    extrudeY = 1;
                    extrudeZ = 1;
                }
                if(!isblock && isinsert && stringutils::startsWith(currtext, prefix)) {

                    std::vector<double> coords = {coordsX[0], coordsY[0]};
                    blockInsertCoords.insert(std::make_pair(currtext, coords));
                    isinsert = false;
                    currtext = "";
                    coordsX.clear();
                    coordsY.clear();
                }
                if(value == "BLOCK") {
                    isblock = true;
                } else if(value == "ENDBLK") {
                    if(blockname.substr(0, prefix.length()) == prefix) {
                        blocks.push_back(blockname.substr(prefix.length(), blockname.length()));
                        blockColour.push_back(blockcolour);
                    }
                    isblock = false;
                    blockname = "";
                    blockcolour = "";
                } else if(isblock && (value == "LWPOLYLINE" || value == "SPLINE")) {
                    islwpoly = true;
                    coordsX.clear();
                    coordsY.clear();
                    polyclosed = false;
                } else if(isblock && value == "CIRCLE") {
                    iscircle = true;
                    coordsX.clear();
                    coordsY.clear();
                } else if(isblock && value == "ARC") {
                    isarc = true;
                    coordsX.clear();
                    coordsY.clear();
                } else if(isblock && value == "POLYLINE") {
                    ispoly = true;
                    coordsX.clear();
                    coordsY.clear();
                    polyclosed = false;
                } else if(isblock && ispoly && value == "SEQEND") {
                    ispoly = false;
                    isvertex = false;
                    ispolyfinished = true;
                } else if(isblock && value == "VERTEX") {
                    isvertex = true;
                } else if(isblock && value == "LINE") {
                    isline = true;
                    coordsX.clear();
                    coordsY.clear();
                } else if(isblock && (value == "MTEXT" || value == "TEXT")) {
                    ismtext = true;
                    coordsX.clear();
                    coordsY.clear();
                } else if(isblock && value == "POINT") {
                    ispoint = true;
                    coordsX.clear();
                    coordsY.clear();
                } else if(value == "INSERT") {
                    isinsert = true;
                    coordsX.clear();
                    coordsY.clear();
                } else if(value == "LAYER") {
                    islayer = true;
                }
            } else if (islwpoly || ismtext || isline || ispoint || islayer || (!isblock && isinsert) || ispoly || iscircle || isarc) {
                if((key == 10 || (isline && key == 11)) && !(ispoly && !isvertex)) {
                    // Polylines also have coordinates in the entity that starts the sequance (POLYLINE)
                    // which should not be counted among the coordinates (unless we are looking at a vertex)
                    coordsX.push_back(std::stod(value)*scale);
                } else if ((key == 20 || (isline && key == 21)) && !(ispoly && !isvertex)) {
                    coordsY.push_back(std::stod(value)*scale);
                } else if(key == 8 && !(ispoly && isvertex)) {
                    // Sometimes it happens that a vertex is in a polyline but has a different
                    // layer than the polyline itself. It's not supposed to happen but obviously
                    // some apps are really bad at writing DXFs...
                    // Without this check here the whole polyline will end up in the layer of the
                    // last vertex and not of the polygon
                    layer = value;
                } else if(key == 62) {
                    colour = value;
                } else if(islwpoly && key == 70) {
                    if(value == "1" || value == "129") polyclosed = true;
                } else if(ismtext && key == 1) {
                    currtext = value;
                } else if(isarc && key == 50) {
                    startangle = std::stod(value);
                } else if(isarc && key == 51) {
                    endangle = std::stod(value);
                } else if((iscircle || isarc) && key == 40) {
                    radius = std::stod(value)*scale;
                } else if((iscircle || isarc || islwpoly) && key == 210) {
                    extrudeX = std::stod(value);
                } else if((iscircle || isarc || islwpoly) && key == 220) {
                    extrudeY = std::stod(value);
                } else if((iscircle || isarc || islwpoly) && key == 230) {
                    extrudeZ = std::stod(value);
                } else if(isinsert && key == 2) {
                    currtext = value;
                } else if(islayer && key == 2) {
                    currtext = value;
                }
            } else if (ispoly && key == 70 && (value == "1" || value == "129")) {
                polyclosed = true;
            } else if(isblock && key == 2 && blockname=="") {
                if(value.substr(0, prefix.length()) == prefix) {
                    blockname = value;
                } else {
                    isblock = false;
                    blockname = "";
                    blockcolour = "";
                }
            } if(isblock && key == 62 && blockcolour=="") {
                blockcolour = value;
            }
        }
        // retrieve the layers in case the table was missing from the DXF:
        for(std::string& layer: lineLayer) {
            if(std::find(layers.begin(), layers.end(), layer) == layers.end()) {
                layers.push_back(layer);
                layerType.push_back(lineLayerIdentifier);
                layerColour.push_back(defaultColour);
            }
        }
        for(std::string& layer: polyLayer) {
            if(std::find(layers.begin(), layers.end(), layer) == layers.end()) {
                layers.push_back(layer);
                layerType.push_back(polyLayerIdentifier);
                layerColour.push_back(defaultColour);
            }
        }
        for(std::string& layer: textLayer) {
            if(std::find(layers.begin(), layers.end(), layer) == layers.end()) {
                layers.push_back(layer);
                layerType.push_back(textLayerIdentifier);
                layerColour.push_back(defaultColour);
            }
        }
        for(std::string& layer: pointLayer) {
            if(std::find(layers.begin(), layers.end(), layer) == layers.end()) {
                layers.push_back(layer);
                layerType.push_back(pointLayerIdentifier);
                layerColour.push_back(defaultColour);
            }
        }
        addOffsetToCoords(blockInsertCoords, lineBlockName, lines);
        addOffsetToCoords(blockInsertCoords, polyBlockName, polys);
        addOffsetToCoords(blockInsertCoords, textBlockName, texts);
        addOffsetToCoords(blockInsertCoords, pointBlockName, points);
    }
};
