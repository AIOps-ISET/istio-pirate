# ISTIO-pirate

本项目分为两个部分：

1. yaml注入

2. init容器(配置pod内iptables)与流量劫持

## init 容器

在docker中运行时(docker run)，应加上 `--privileged` 参数，以使iptables命令能够生效。

在k8s中创建pod时应加上安全上下文:

```yaml
apiVersion: v1
kind: Pod
metadata:
  name: app-pod
spec:
  containers:
  - name: init-container
    image: locale/init:alpine
    securityContext:
      capabilities:
        add: ["NET_ADMIN"] # 使iptables命令生效
```