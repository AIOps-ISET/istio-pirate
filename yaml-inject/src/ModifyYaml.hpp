#include <string>
#include <yaml-cpp/yaml.h>

class ModifyYaml {
public:
    ModifyYaml(std::string file);
    ~ModifyYaml();
    void insert_init_container();

private:
    std::string filename;
    YAML::Node *yamlFile;
};