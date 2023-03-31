#include "ModifyYaml.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <yaml-cpp/yaml.h>

using std::cout;
using std::endl;

ModifyYaml::ModifyYaml(std::string file) {
    this->filename = file;
    this->yamlFile = YAML::LoadFile(file);
};

ModifyYaml::~ModifyYaml() {
    cout << "Writng to config file..." << endl;
    std::ofstream of(this->filename);
    of << yamlFile;
    cout << "Write to config file finished!" << endl;
}

void ModifyYaml::insert_init_container() {
    cout << "Modifying config file..." << endl;

    size_t containers_size = this->yamlFile["containers"].size();
    bool   find_same       = false;
    for (int i = 0; i < containers_size; i++) {
        const std::string name = yamlFile["containers"][i]["image"].as<std::string>();
        if (name == "init-container-name") {
            find_same = true;
            break;
        }
    }

    if (!find_same) {
        YAML::Node node;
        node["image"] = "init-container-name";
        node["args"].push_back("arg1");
        node["args"].push_back("arg2");
        node["args"].push_back("arg3");
        this->yamlFile["containers"].push_back(node);
    } else {
        cout << "find same!" << endl;
    }

    cout << "Modify config file finished!" << endl;
}