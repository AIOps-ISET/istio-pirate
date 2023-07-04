#include "Ev.h"
#include "EvHTTP.h"
using namespace Net::Event;
using namespace Net::HTTP;

#include "admission/v1/types.h"
#include "core/v1/types.h"
#include "json.hpp"
#include <string>

using nlohmann::json,std::string;

class HTTPDemoHandler : public Mux::IHandler {
public:
    virtual void handle(_INOUT_ ResponseWriter *writer, Request &req, void *data = NULL) {
        (void)(data);
        req.inspect();
        if (req.getMethod() != HTTP_METHOD_POST)
            return errTips(writer);
        vector<char>                         reqData = req.read();
        apis::admission::v1::AdmissionReview review;
        if (!apis::ResDecodeFromJsonStr(review, reqData)) {
            writer->generateSimpleResponse("BadRequest", "BadRequest", 403).close();
        }

        apis::core::v1::Pod pod;
        if (!apis::ResDecodeFromJsonStr(pod, review.request->object.jsonData)) {
            return responseFail(writer, review.request->uid, 403, "decode object failed");
        }

        if (req.getUri(true) == "/mutating-bysvc") {
            if (pod.metadata.labels.find("demo-svc") == pod.metadata.labels.end()) {
                string portsString;

                for (apis::core::v1::Container c : pod.spec.containers) {
                    portsString.append(c.name);
                    for (apis::core::v1::ContainerPort containerPort : c.ports) {
                        portsString.append(".").append(std::to_string(containerPort.containerPort));
                    }
                    portsString.append("#");
                }
                portsString.erase(portsString.end() - 1, portsString.end());

                auto containerLength = static_cast<uint32_t>(pod.spec.containers.size());
                json jsonPatchData;
                //  patch init container
                jsonPatchData[0]                       = {{"op", "add"}, {"path", "/spec/initContainers"}};
                jsonPatchData[0]["value"][0]["name"]   = "init-container";
                jsonPatchData[0]["value"][0]["image"]  = "local/init:latest";
                jsonPatchData[0]["value"][0]["env"][0] = {{"name", "APP_PORT"}, {"value", portsString}};
                //  patch envoy container
                string containerPath                   = "/spec/containers/" + std::to_string(containerLength);
                jsonPatchData[1]                       = {{"op", "add"}, {"path", containerPath}};
                jsonPatchData[1]["value"]["name"]      = "envoy";
                jsonPatchData[1]["value"]["image"]     = "local/envoy:latest";
                jsonPatchData[1]["value"]["resources"]["limits"]["memory"] = "128Mi";
                jsonPatchData[1]["value"]["resources"]["limits"]["cpu"]    = "500m";
                string       jsonPatchStr                                  = jsonPatchData.dump();
                vector<char> jsonPatchDatastr;
                jsonPatchDatastr.insert(jsonPatchDatastr.end(), jsonPatchStr.begin(), jsonPatchStr.end());
                // 创建补丁
                return responsePass(writer,
                                    review.request->uid,
                                    apis::admission::v1::PatchTypeJSONPatch,
                                    jsonPatchData);
            }
            return responsePass(writer, review.request->uid);
        }

        return errTips(writer);
    }

private:
    void errTips(_INOUT_ ResponseWriter *writer) {
        return writer
            ->generateSimpleResponse("Request Error",
                                     "Available Method: POST<br>Available Path: /mutating-byurl, "
                                     "/mutating-bysvc, /validating-byurl, /validating-bysvc",
                                     403)
            .close();
    }

    void responsePass(_INOUT_ ResponseWriter *writer,
                      string                  uid,
                      string                  patchType = "",
                      vector<char>            patchData = vector<char>()) {
        return response(writer, uid, true, 200, "", patchType, patchData);
    }

    void responseFail(_INOUT_ ResponseWriter *writer, string uid, int code = 0, string msg = "") {
        return response(writer, uid, false, code, msg);
    }

    void response(_INOUT_ ResponseWriter *writer,
                  string                  uid,
                  bool                    allowed,
                  int                     code      = 0,
                  string                  msg       = "",
                  string                  patchType = "",
                  vector<char>            patchData = vector<char>()) {
        apis::admission::v1::AdmissionReview review;
        review.response->uid             = uid;
        review.response->allowed         = allowed;
        review.response->status->code    = code;
        review.response->status->message = msg;
        if (!patchType.empty()) {
            *review.response->patchType = patchType;
            review.response->patch      = patchData;
        }
        vector<char> respData;
        if (!apis::ResEncodeToJsonStr(review, respData))
            return writer->generateSimpleResponse("BadRequest", "BadRequest", 500).close();
        return writer->Type("application/json").Write(respData).Returns(code).close();
    }
};

HTTPDemoHandler handler;

int main() {
    Mux mux;
    mux.addRoute("/", &handler, true);
    Server server(&mux);

    const char *ip   = "0.0.0.0";
    int         port = 10801;

    printf("[INFO] HTTP Serving at %s:%d\n", ip, port);
    server.serve(ip, port);
    return 0;
}
