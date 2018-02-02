#ifndef AOI_IMPORTER_H 
#define AOI_IMPORTER_H


#include "tinyxml2/tinyxml2.h"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <stdexcept>

struct AOIElement {
    int x;
    int y;
    int width;
    int height;
    int framespanStart;
    int framespanEnd;
};

struct DynamicAOI {
    int framespanStart;
    int framespanEnd;
    
    std::string id;
    std::string name;
    std::vector<AOIElement> elements;
};

struct GenericSourceInfo {
    std::string filename;
    int width;
    int height;
    int numberOfFrames;
};

class AOIImporter {
    
public:
    AOIImporter() {}
    ~AOIImporter() {}
    /*
     * Precondition : The 'absXMLFilePath' refers to an existing 
     * XML-Document which is conform to the ViPeR-XML-Format. As well
     * it is guarenteed that the data is well formated and the data
     * is sorted ascending by time.
     *
     * Postcondition : AOI data represented in a list of structures.
     * The order of the elements in the vector is the same as the order
     * of the data in the input XML file.
     *
     */
    bool parseViperFormattedXML(const std::string absXMLFilePath, std::vector<struct DynamicAOI> &dynamicAOIs, struct GenericSourceInfo& info);
private:
    bool extractFramespanBounds(const std::string &strFramespan, int &framespanStart, int &framespanEnd);
};


#endif // AOI_IMPORTER_H

