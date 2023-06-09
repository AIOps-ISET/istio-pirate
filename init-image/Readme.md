# init容器

这个Dockerfile文件中的内容是将一个名为“init.sh”的脚本复制到Debian操作系统的根目录中，并授予该脚本可执行权限。

然后，使用CMD指令将“init.sh”作为容器的默认命令来运行。

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