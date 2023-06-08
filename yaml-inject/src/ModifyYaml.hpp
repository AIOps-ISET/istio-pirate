#include <string>
#include <yaml-cpp/yaml.h>

class ModifyYaml {
public:
    explicit ModifyYaml(std::string const &fileName);
    void insertInitContainer();
    void insertEnvoyContainer();
    void writeToFile(std::string const &newFileName);

private:
    std::string filename;
    YAML::Node *fileRootNode;
    YAML::Node *containersFatherNode;
};