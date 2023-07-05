extern "C" {
#include <kubernetes/api/CoreV1API.h>
#include <kubernetes/config/incluster_config.h>
#include <kubernetes/config/kube_config.h>
}
#include <iostream>
#include <string>

using std::cout, std::endl;

void list_pod(apiClient_t *apiClient) {
    v1_pod_list_t *pod_list;
    std::string    query_namespace = "default";

    pod_list = CoreV1API_listNamespacedPod(apiClient,
                                           query_namespace.data(), /*namespace */
                                           nullptr,                /* pretty */
                                           0,                      /* allowWatchBookmarks */
                                           nullptr,                /* continue */
                                           nullptr,                /* fieldSelector */
                                           nullptr,                /* labelSelector */
                                           0,                      /* limit */
                                           nullptr,                /* resourceVersion */
                                           nullptr,                /* resourceVersionMatch */
                                           0,                      /* sendInitialEvents */
                                           0,                      /* timeoutSeconds */
                                           0                       /* watch */
    );
    cout << "The return code of HTTP request=" << apiClient->response_code << endl;
    if (pod_list) {
        cout << "Get pod list:" << endl;
        listEntry_t *listEntry;
        v1_pod_t    *pod;
        list_ForEach(listEntry, pod_list->items) {
            pod = (v1_pod_t *)listEntry->data;
            cout << "\tThe pod name: " << pod->metadata->name << endl;
        }
        v1_pod_list_free(pod_list);
    } else {
        cout << "Cannot get any pod." << endl;
    }
}

int main() {
    char        *basePath  = nullptr;
    sslConfig_t *sslConfig = nullptr;
    list_t      *apiKeys   = nullptr;
    int          result    = load_incluster_config(&basePath, &sslConfig, &apiKeys);
    if (result != 0) {
        cout << "Cannot load kubernetes configuration." << endl;
        return -1;
    }
    apiClient_t *apiClient = apiClient_create_with_base_path(basePath, sslConfig, apiKeys);
    if (!apiClient) {
        cout << "Cannot create a kubernetes client." << endl;
        return -1;
    }

    list_pod(apiClient);

    apiClient_free(apiClient);
    free_client_config(basePath, sslConfig, apiKeys);
    basePath  = nullptr;
    sslConfig = nullptr;
    apiKeys   = nullptr;
    apiClient_unsetupGlobalEnv();

    return 0;
}