#include <kubernetes/api/CoreV1API.h>
#include <kubernetes/config/kube_config.h>
#include <string>

void list_pod(apiClient_t *apiClient) {
    v1_pod_list_t *pod_list;
    char          *s        = nullptr;
    strcpy(s, "default");

    pod_list = CoreV1API_listNamespacedPod(apiClient,
                                           s,       /*namespace */
                                           nullptr, /* pretty */
                                           0,       /* allowWatchBookmarks */
                                           nullptr, /* continue */
                                           nullptr, /* fieldSelector */
                                           nullptr, /* labelSelector */
                                           0,       /* limit */
                                           nullptr, /* resourceVersion */
                                           nullptr, /* resourceVersionMatch */
                                           0,       /* sendInitialEvents */
                                           0,       /* timeoutSeconds */
                                           0        /* watch */
    );
    printf("The return code of HTTP request=%ld\n", apiClient->response_code);
    if (pod_list) {
        printf("Get pod list:\n");
        listEntry_t *listEntry;
        v1_pod_t    *pod;
        list_ForEach(listEntry, pod_list->items) {
            pod = (v1_pod_t *)listEntry->data;
            printf("\tThe pod name: %s\n", pod->metadata->name);
        }
        v1_pod_list_free(pod_list);
    } else {
        printf("Cannot get any pod.\n");
    }
}

int main() {
    char        *basePath  = nullptr;
    sslConfig_t *sslConfig = nullptr;
    list_t      *apiKeys   = nullptr;
    int          rc        = load_kube_config(&basePath,
                              &sslConfig,
                              &apiKeys,
                              nullptr); /* NULL means loading configuration from $HOME/.kube/config */
    if (rc != 0) {
        printf("Cannot load kubernetes configuration.\n");
        return -1;
    }
    apiClient_t *apiClient = apiClient_create_with_base_path(basePath, sslConfig, apiKeys);
    if (!apiClient) {
        printf("Cannot create a kubernetes client.\n");
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