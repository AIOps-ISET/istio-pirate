#include "ModifyYaml.hpp"

int main() {
    ModifyYaml m("config.yaml");
    m.insert_init_container();
    return 0;
}