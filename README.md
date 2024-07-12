# TJU Socket Project

***socket实验完成了四个lab内容，最终给分97分***

## 1. Files
```
│  DockerFile
│  error.log
│  Makefile
│  README.md
│  week4.tar
│
├─include
│      logger.h
│      parse.h
│      response.h
│
├─samples
│      request_400
│      request_404
│      request_501
│      request_5011
│      request_505
│      request_error1
│      request_error2
│      request_error3
│      request_error4
│      request_error5
│      request_Error_Return
│      request_get
│      request_getl
│      request_head
│      request_head2
│      request_new_get
│      request_nnpi
│      request_npipline
│      request_only
│      request_pipeline
│      request_pipeline1
│      request_post
│      request_post1
│      sample_request_example
│
├─src
│      example.c
│      lexer.l
│      liso_client.c
│      liso_server.c
│      logger.c
│      parse.c
│      parser.y
│      response.c
│
└─static_site
    │  111.html
    │  index.html
    │  register.html
    │  style.css
    │  style_register.css
    │
    └─images
            liso_header.png
```

## 2. Finished
| TIME  | LAB                        |
| ----- | -------------------------- |
| Week1 | 实现简单的 Echo Web Server |
| Week2 | 实现 HEAD, GET and POST    |
| Week3 | 实现 HTTP 的并发请求       |
| Week4 | 实现多个客户端的并发处理   |
