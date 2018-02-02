#include "aoi-importer.h" 

//TODO : CHECK IF DATA IS SORTED !!!
//TODO : DEFINE EXCEPTION CLASSES
bool AOIImporter::parseViperFormattedXML(const std::string absXMLFilePath, std::vector<struct DynamicAOI> &dynamicAOIs, 
                            struct GenericSourceInfo &info)
{
    using namespace tinyxml2;
    
    XMLDocument doc;
    if (doc.LoadFile(absXMLFilePath.c_str()) != XMLError::XML_SUCCESS) {
        std::cerr << "Could not open " << absXMLFilePath << std::endl;
        return false;
    }
    
    //////////////////////////////////////////////////////
    //               PROCESS XML-HEADER                 //
    //////////////////////////////////////////////////////
    
    XMLElement* root = doc.FirstChildElement();    
    XMLElement* config = root->FirstChildElement("config");
    
    if (root == nullptr) {
        std::cerr << "Did not found viper element" << std::endl;
        return false;
    }
    if (config == nullptr) {
        std::cerr << "Did not found config element" << std::endl;
        return false;
    }
    
    const char* xmlnsAttribute = root->Attribute("xmlns");
    const char* xmlnsDataAttribute = root->Attribute("xmlns:data");
    
    if (xmlnsAttribute != std::string("http://lamp.cfar.umd.edu/viper")) {
        std::cerr << "Only ViPER-XML format is allowed as input !" << std::endl;
        return false;
    }
    
    if (xmlnsDataAttribute != std::string("http://lamp.cfar.umd.edu/viperdata")) {
        std::cerr << "Only ViPER-XML format is allowed as input !" << std::endl;
        return false;
    }
    
    ///////////////////////////////////////////////////////////////////////////
    //                          PROCESS DESCRIPTORS                          //
    //              <descriptor type="FILE" name="Information">              //
    ///////////////////////////////////////////////////////////////////////////
    
    XMLElement* descriptor = config->FirstChildElement("descriptor");

    // There might be data even if there is no descriptor,
    // but we it's way safer to stop at this step
    if (descriptor == nullptr) {
        std::cerr << "No descriptor found !"<< std::endl;
        return false;
    }
    
    do {
        const char* descriptorType = descriptor->Attribute("type");
        if (descriptorType == std::string("FILE")) {
            //TODO parse
        } 
        else if (descriptorType == std::string("OBJECT")) {
            XMLElement* attribute = descriptor->FirstChildElement("attribute");
            if (attribute != nullptr) {
                bool dynamic;
                if(attribute->QueryBoolAttribute("dynamic", &dynamic) == XMLError::XML_SUCCESS) {
                    if (!dynamic) {
                        std::cerr << "Currently static AOIs are not supported !"<< std::endl;
                    }
                } else {
                    std::cerr << "Could not read value from attribute field 'dynamic'"<< std::endl;
                }
                
                const char* type = attribute->Attribute("type");
                
                if (type != std::string("bbox")) {
                    std::cerr << "Encountered non-bbox type!"<< std::endl;
                }
                
                const char* name = attribute->Attribute("name");
                
                if (name != std::string("BoundingBox")) {
                    std::cerr << "Encountered non-BoundingBox name!"<< std::endl;
                }
            } else {
                std::cerr << "No element 'attribute' found"<< std::endl;
            }
        }
        else {
           std::cerr << "Unknown value" << descriptorType << "for attribute field 'type' in 'descriptor'" << std::endl;
        }
    } while((descriptor = descriptor->NextSiblingElement("descriptor")) != nullptr);
    
    // Might we encounter multiple SourceFiles ??
    XMLElement* data = root->FirstChildElement("data")->FirstChildElement("sourcefile");

    info.filename = std::string(data->Attribute("filename"));
    
    if (data == nullptr) {
        std::cerr << "No element 'data' found"<< std::endl;
        return false;
    }
    
    XMLElement* file = data->FirstChildElement("file");
    if (file == nullptr) {
        std::cerr << "No element 'file' found"<< std::endl;
        return false;
    }

    XMLElement* attribute = file->FirstChildElement("attribute");

    while(attribute != nullptr) {
        XMLElement *dvalue = attribute->FirstChildElement("data:dvalue");
        const char *name = attribute->Attribute("name");
        
        if (name != nullptr && std::string(name) == "NUMFRAMES") {
            if(dvalue->QueryIntAttribute("value", &info.numberOfFrames) != XMLError::XML_SUCCESS) {
                std::cerr << "Could not read NUMFRAMES from data:dvalue "<< std::endl;
                return false;
            }
        }
        else if (name != nullptr && std::string(name) == "H-FRAME-SIZE") {
            if(dvalue->QueryIntAttribute("value", &info.width) != XMLError::XML_SUCCESS) {
                std::cerr << "Could not read H-FRAME-SIZE from data:dvalue "<< std::endl;
                return false;
            }
        }
        else if (name != nullptr && std::string(name) == "V-FRAME-SIZE") {
            if(dvalue->QueryIntAttribute("value", &info.height) != XMLError::XML_SUCCESS) {
                std::cerr << "Could not read V-FRAME-SIZE from data:dvalue "<< std::endl;
                return false;
            }
        } else {
            std::cerr << "Unknown value for name field in element attribute "<< std::endl;
            return false;
        }
        attribute = attribute->NextSiblingElement();
    }

    ///////////////////////////////////////////////////////////////////////////
    //                           PROCESS OBJECTS                             //   
    //             <object id="0" framespan="1:625" name="Red Car">          //
    ///////////////////////////////////////////////////////////////////////////
    
    XMLElement* object = data->FirstChildElement("object");

    if (object == nullptr) {
        std::cerr << "No Object found !"<< std::endl;
        return false;
    }
    
    do {
        const char* id = object->Attribute("id");
        
        if (id == nullptr) {
            std::cerr << "Object has no attribute 'id' !"<< std::endl;
            return false;
        }
        
        const char* framespan = object->Attribute("framespan");
        int framespanStart, framespanEnd;
        
        if (framespan == nullptr || !extractFramespanBounds(std::string(framespan), framespanStart, framespanEnd)) {
            std::cerr << "Could not read value from attribute field 'framespan'"<< std::endl;
        } 
        
        const char* name = object->Attribute("name");
        
        if (name == nullptr) {
            std::cerr << "Object has no attribute 'name' !"<< std::endl;
            return false;
        }

        struct DynamicAOI dynamicAOI = {
            framespanStart, 
            framespanEnd,
            std::string(id), 
            std::string(name),
            {}
        };
        
        XMLElement* attribute = object->FirstChildElement("attribute");
        if (attribute != nullptr) {
            const char* name = attribute->Attribute("name");
            if (name == std::string("BoundingBox")) {
                
                ///////////////////////////////////////////////////////////////////////////
                //                              PROCESS BBOX                             //
                // <data:bbox width="74" x="-8" y="355" height="131" framespan="1:1"/>   //
                ///////////////////////////////////////////////////////////////////////////
                            
                XMLElement* databbox = attribute->FirstChildElement("data:bbox");

                if (databbox == nullptr) {
                    std::cerr << "There is no databbox !"<< std::endl;
                    return false;
                }

                do {
                    int width, height, x, y, framespanStart, framespanEnd;
                    
                    if (databbox == nullptr) {
                        std::cerr << "Could not read data::bbox !" << std::endl;
                        return false;
                    }
                    
                    if(databbox->QueryIntAttribute("width", &width) != XMLError::XML_SUCCESS) {
                        std::cerr << "Could not read value from attribute field 'width'"<< std::endl;
                    }
                    if(databbox->QueryIntAttribute("height", &height) != XMLError::XML_SUCCESS) {
                        std::cerr << "Could not read value from attribute field 'width'"<< std::endl;
                    }
                    if(databbox->QueryIntAttribute("x", &x) != XMLError::XML_SUCCESS) {
                        std::cerr << "Could not read value from attribute field 'width'"<< std::endl;
                    }
                    if(databbox->QueryIntAttribute("y", &y) != XMLError::XML_SUCCESS) {
                        std::cerr << "Could not read value from attribute field 'width'"<< std::endl;
                    }
                    
                    const char* framespan = databbox->Attribute("framespan");
                    if (framespan == nullptr  || !extractFramespanBounds(std::string(framespan), framespanStart, framespanEnd)) {
                        std::cerr << "Could not read value from attribute field 'framespan'"<< std::endl;
                    }
                    
                    struct AOIElement aoiElement = {
                        x,
                        y,
                        width,
                        height,
                        framespanStart,
                        framespanEnd
                    };
                    
                    dynamicAOI.elements.push_back(aoiElement);
                    
                } while ((databbox = databbox->NextSiblingElement("data:bbox")) != nullptr);
            } else {
                std::cerr << "Could not read value from attribute field 'name'"<< std::endl;
            }
        } 
        dynamicAOIs.push_back(dynamicAOI);
    } while ((object = object->NextSiblingElement("object")) != nullptr);
    return true;
}


bool AOIImporter::extractFramespanBounds(const std::string &strFramespan, int &framespanStart, int &framespanEnd) 
{
    std::stringstream ss(strFramespan);
    std::string number;
    
    // If formation is correct, split by ':' will result in two numbers
    for (int i = 0; std::getline(ss, number, ':'); ++i) {
        try {
            // First number
            if (i == 0) {
                framespanStart = std::stoi(number);
            }
            // Second number
            else if (i == 1) {
                framespanEnd = std::stoi(number);
            }
            // Split should not result in more than two items
            else {
                return false;
            }
        } catch (const std::invalid_argument& ia) {
            std::cerr << "Invalid argument at parsing framespan: " << ia.what() << std::endl;
        }
    }
    return true;
}
