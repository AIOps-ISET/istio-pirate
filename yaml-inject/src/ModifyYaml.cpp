#include "ModifyYaml.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <yaml-cpp/yaml.h>

using std::cout;
using std::endl;

ModifyYaml::ModifyYaml(std::string const &fileName) {
    filename              = fileName;
    fileRootNode          = new YAML::Node;
    containersFatherNode  = new YAML::Node;
    *fileRootNode         = YAML::LoadFile(filename);
    *containersFatherNode = (*fileRootNode)["spec"];
};

void ModifyYaml::insertInitContainer() {
#ifdef DEBUG
    cout << "Inserting init container..." << endl;
#endif

    YAML::Node  containersNode = (*containersFatherNode)["containers"];
    std::string portsString;

    for (auto containerNode : containersNode) {
        portsString.append(containerNode["name"].as<std::string>());
        for (auto containerPort : containerNode["ports"]) {
            portsString.append(".").append(containerPort["containerPort"].as<std::string>());
        }
        portsString.append("#");
    }
    portsString.erase(portsString.end() - 1, portsString.end());

    YAML::Node initContainersNode;
    YAML::Node initContainerNode;
    initContainerNode["image"] = "local/init:latest";
    initContainerNode["name"]  = "init-container";
    YAML::Node envMap;
    envMap["name"]  = "APP_PORT";
    envMap["value"] = portsString;
    initContainerNode["env"].push_back(envMap);
    initContainersNode.push_back(initContainerNode);
    (*containersFatherNode)["initContainers"] = initContainersNode;

#ifdef DEBUG
    cout << "Insert init container finished!" << endl;
#endif
}

void ModifyYaml::insertEnvoyContainer() {
    YAML::Node envoyNode;
    envoyNode["name"]                          = "envoy-container";
    envoyNode["image"]                         = "local/envoy:latest";
    envoyNode["resources"]["limits"]["cpu"]    = "500m";
    envoyNode["resources"]["limits"]["memory"] = "128Mi";
    YAML::Node envoyPortsNode;
    envoyPortsNode[0]["containerPort"] = "9999";
    envoyNode["ports"]                 = envoyPortsNode;
    (*containersFatherNode)["containers"].push_back(envoyNode);
}


void ModifyYaml::writeToFile(std::string const &newFileName) {
#ifdef DEBUG
    cout << "Writng to config file..." << endl;
#endif

    std::ofstream of(newFileName);
    of << *fileRootNode;

#ifdef DEBUG
    cout << "Write to config file finished!" << endl;
#endif
}
