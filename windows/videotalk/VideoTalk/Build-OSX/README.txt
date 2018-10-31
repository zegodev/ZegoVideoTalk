mac平台下LiveDemo5 QT版使用注意事项
一、mac平台脚本构建好的app可以直接运行。
二、若需要运行xcode工程文件：
1.需要安装qt5.7.1_mac环境；
2.删除bin目录下除了generate.sh和build.sh以外的所有文件；
3.先执行generate.sh生成xcode工程，再执行build.sh构建工程。