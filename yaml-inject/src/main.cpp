#include "ModifyYaml.hpp"

int main() {
    ModifyYaml m("pod.yaml");
    m.insertInitContainer();
    m.insertEnvoyContainer();
    m.writeToFile("modified-pod.yaml");

    return 0;
}