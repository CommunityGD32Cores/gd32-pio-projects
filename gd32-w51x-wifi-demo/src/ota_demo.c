/*!
    \file    ota_demo.c
    \brief   OTA demonstration program for GD32W51x WiFi SDK

    \version 2021-10-30, V1.0.0, firmware for GD32W51x
*/

/*
    Copyright (c) 2021, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software without
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/

#include "stdio.h"
#include "stdint.h"
#include "lwip/sockets.h"
#include "wrapper_os.h"
#include "nspe_region.h"
#if defined(CONFIG_TZ_ENABLED)
#include "rom_export.h"
#include "mbl_nsc_api.h"
#else
#include "mbl_api.h"
#endif

#define HTTP_GET_MAX_LEN          1024 //256
#define RECBUFFER_LEN             1516
#define INVALID_SOCKET            (-1)

#define DOWNLOAD_URL    "http://192.168.110.24/"
#define PORT            80
#define TERM            "\r\n"
#define ENDING          "\r\n\r\n"

/*!
    \brief      initialize http socket
    \param[in]  host: pointer to the input host name
    \param[in]  port: the port want to connect
    \param[out] none
    \retval     function status
      \arg        -1: host name error
      \arg        -2: create socket failed
      \arg        -3: connect failed
      \arg        other: created socket id
*/
static int32_t http_socket_init(char *host, uint32_t port)
{
    struct sockaddr_in sock;
    int32_t sid = INVALID_SOCKET;
    int32_t n = 1, ret;

    ret = strlen(host);
    if( ret <= 0 || ret > 17 )
        return -1;

    if ((sid = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        printf("Create socket failed.\r\n");
        return -2;
    } else {
        int recv_timeout_ms = 60000;
        setsockopt(sid, SOL_SOCKET, SO_RCVTIMEO, &recv_timeout_ms, sizeof(recv_timeout_ms));
        setsockopt(sid, SOL_SOCKET, SO_REUSEADDR, &n, sizeof(n));
    }
    printf("Socket ID: %d\r\n", sid);

    printf("Connect to:\r\n");
    printf("\tHost: %s", host);
    printf("\tPort: %d\r\n", port);

    sock.sin_family = AF_INET;
    sock.sin_port= htons(port);
    sock.sin_addr.s_addr = inet_addr(host);
    ret = connect(sid, (struct sockaddr *)&sock, sizeof(struct sockaddr_in));
    if (ret == 0) {
        printf("Connect successfully.\r\n");
        return sid;
    } else {
        printf("Connect failed.\r\n");
        close(sid);
        return -3;
    }
}

/*!
    \brief      get http responses code
    \param[in]  httpbuf: pointer to the http responses string
    \param[out] none
    \retval     http responses code value(0x00000000=0xffffffff)
*/
int32_t http_rsp_code(uint8_t *httpbuf)
{
    int32_t response_code;
    char* p_start = NULL;
    char* p_end = NULL;
    char re_code[10] ={0};

    memset(re_code, 0, sizeof(re_code));

    p_start = strstr((char *)httpbuf, "HTTP/1.");
    if (NULL == p_start) {
        return -1;
    }
    p_start += strlen("HTTP/1.1");
    while (*p_start == ' ')  p_start++;
    p_end = strstr(p_start, " ");
    if (p_end) {
        if (p_end - p_start > sizeof(re_code)) {
            return -2;
        }
        memcpy(re_code, p_start, (p_end - p_start));
    }

    response_code = atoi(re_code);
    return response_code;
}

/*!
    \brief      get http responses string head length
    \param[in]  httpbuf: pointer to the http responses string
    \param[out] none
    \retval     http responses string head length value(0x00000000=0xffffffff)
*/
int32_t http_hdr_len(uint8_t *httpbuf)
{
    char *p_start = NULL;
    char *p_end =NULL;
    int32_t headlen=0;
    p_start = (char *)httpbuf;
    p_end = strstr((char *)httpbuf, ENDING);
    if (p_end == NULL) {
        printf("Can't not find the http head!\r\n");
        return 0;
    }
    p_end = p_end + strlen(ENDING);
    headlen = (p_end - p_start);
    return headlen;
}

/*!
    \brief      get http responses string body length
    \param[in]  httpbuf: pointer to the http responses string
    \param[out] none
    \retval     http responses string body length value(0x00000000=0xffffffff)
*/
int32_t http_body_len(uint8_t *httpbuf)
{
    char *p_start = NULL;
    char *p_end = NULL;
    char bodyLenbuf[10] = {0};
    int32_t bodylen = 0;  //Content-Length:

    p_start = strstr((char *)httpbuf, "Content-Length:");
    if (p_start == NULL)
        return 0;

    p_start = p_start + strlen("Content-Length:");
    while (*p_start == ' ')  p_start++;
    p_end = strstr(p_start, TERM);
    if (p_end == NULL)
        return 0;

    memcpy(bodyLenbuf, p_start, (p_end - p_start));
    bodylen = atoi(bodyLenbuf);
    return bodylen;
}

/*!
    \brief      get http host
    \param[in]  download_url: pointer to the download url
    \param[out] host: pointer to the http host
    \param[out] url: pointer to the http url
    \retval     function status
      \arg        -1: input is not a http download url
      \arg        -2: can not find http host
      \arg        -3: malloc http host space fail
      \arg        -4: can not find a rigth url
      \arg        0: get http host succee
*/
int http_host_get(char *downloadurl, char **host, char **url)
{
    char *p_start = NULL;
    char *p_end =NULL;
    // int len;
    char hostlen;
    int ret = 0;

    // len = strlen(downloadurl);
    p_start = strstr( downloadurl,"http://" );
    if (p_start == NULL)
        return -1;
    p_start = p_start + strlen("http://");
    p_end = strstr(p_start, "/");
    if (p_end == NULL)
        return -2;
    *host = (char *)sys_malloc(p_end - p_start + 1);
    if (NULL == *host){
        return -3;
    }
    memcpy(&(*host)[0], p_start, p_end - p_start);
    (*host)[p_end - p_start] = '\0';
    hostlen = p_end - p_start;

    p_start = p_end;
    p_end = p_start + strlen(downloadurl) - hostlen;
    *url = (char *)sys_malloc(p_end - p_start + 1);
    if (NULL == *url) {
        ret = -4;
        goto exit;
    }
    memcpy(&(*url)[0], p_start, p_end - p_start);
    (*url)[p_end - p_start] = '\0';

    return 0;
exit:
    if (*host) {
        sys_mfree(*host);
        *host = NULL;
    }
    return ret;
}

/*!
    \brief      send get http responses image information
    \param[in]  sid: http socket id
    \param[in]  url: pointer to the http url
    \param[in]  host: pointer to the http host
    \param[in]  bin: pointer to the bin name
    \param[out] none
    \retval     function status
      \arg        -1: input value have null value
      \arg        -2: malloc fail
      \arg        -3: malloc http host space fail
      \arg        0: send get http responses image information succee
*/
static int32_t http_req_image(int32_t sid, char *url, char *host, uint16_t port, char *bin)
{
    char *getBuf = NULL;
    int32_t totalLen = 0;
    int32_t ret = 0;

    if ((url == NULL) || (host == NULL) || (bin == NULL)) {
        ret = -1;
        goto exit;
    }

    getBuf = (char*)sys_malloc(HTTP_GET_MAX_LEN);
    if (getBuf == NULL) {
        ret = -2;
        goto exit;
    }

    snprintf(getBuf, HTTP_GET_MAX_LEN, "%s %s%s %s%s%s%s:%d%s%s%s",
                                        "GET", url, bin, "HTTP/1.1", TERM,
                                        "Host:", host, port, TERM,
                                        "Connection: keep-alive\r\n", ENDING);


    printf("Send: %s", getBuf);
    totalLen = strlen(getBuf);
    ret = send(sid, getBuf, totalLen, 0);
    if (ret > 0) {
        ret = 0;
    }
    sys_mfree(getBuf);

exit:
    if (NULL != host) {
        sys_mfree(host);
    }
    if (NULL != url) {
        sys_mfree(url);
    }
    return ret;
}

/*!
    \brief      get http responses image
    \param[in]  sid: http socket id
    \param[in]  running_idx: runing image idx
    \param[out] none
    \retval     function status
      \arg        -1: malloc space fail
      \arg        -2: get nothing from http service
      \arg        -3: get http responses code is not 200
      \arg        -4: write flash fail
      \arg        -5: get data from http service fail
      \arg        0: run succee
*/
static int32_t http_rsp_image(int32_t sid, uint32_t running_idx)
{
    uint8_t *recvbuf, *buf;
    int32_t recv_len, hdr_len, body_len, offset;
    uint32_t new_img_addr = 0xFFFFFFFF, new_img_len;
    int32_t ret = 0;

    recvbuf = sys_malloc(RECBUFFER_LEN);
    if (recvbuf == NULL) {
        return -1;
    }
    memset(recvbuf, 0, RECBUFFER_LEN);

    recv_len = recv(sid, recvbuf, RECBUFFER_LEN, 0);
    if (recv_len <= 0) {
        ret = -2;
        goto Exit;
    }

    if (200 != http_rsp_code(recvbuf)) {
        ret = -3;
        goto Exit;
    }
    printf("HTTP response 200 ok\r\n");
    hdr_len = http_hdr_len(recvbuf);
    body_len = http_body_len(recvbuf);
    printf("Content length: %d\r\n", body_len);
    if (running_idx == IMAGE_0) {
        new_img_addr = RE_IMG_1_PROT_OFFSET;
        new_img_len = RE_IMG_1_END_OFFSET - RE_IMG_1_PROT_OFFSET;
    } else {
        new_img_addr = RE_IMG_0_PROT_OFFSET;
        new_img_len = RE_IMG_1_PROT_OFFSET - RE_IMG_0_PROT_OFFSET;
    }
    printf("Running index: %d\r\n", running_idx);
    mbl_flash_erase(new_img_addr, new_img_len);
    printf("Flash erase OK (start 0x%x, len 0x%x)\r\n", new_img_addr, new_img_len);

    offset = 0;
    buf = recvbuf + hdr_len;
    recv_len -= hdr_len;
    if (recv_len < 0) {
        ret = -4;
        goto Exit;
    }
    do {
        if (recv_len > 0) {
            // printf("Write to 0x%x with len %d\r\n", offset, recv_len);
            if (mbl_flash_write((new_img_addr + offset), buf, recv_len) < 0) {
                ret = -5;
                goto Exit;
            }
        }
        offset += recv_len;
        recv_len = body_len - offset;
        if (0 == recv_len)
            break;
        if (recv_len > RECBUFFER_LEN) {
            recv_len = RECBUFFER_LEN;
        }
        recv_len = recv(sid, recvbuf, recv_len, 0);
        if (recv_len <= 0) {
            ret = -6;
            goto Exit;
        }
        buf = recvbuf;
    } while(offset < body_len);

Exit:
    if (ret < 0) {
        // buffer_dump("Recv:", recvbuf, recv_len);
    }
    sys_mfree(recvbuf);
    return ret;
}

/*!
    \brief      task of ota demo
    \param[in]  param: pointer to the input parameter
    \param[out] none
    \retval     none
*/
void ota_demo_task(void *param)
{
    char *url = NULL;
    char *host = NULL;
    int32_t http_socketid = -1;
    uint8_t running_idx = IMAGE_0;
    char *bin_name = (char *)param;
    int32_t ret = 0, res;

    printf("Start OTA test...\r\n");

    res = mbl_sys_status_get(SYS_RUNNING_IMG, LEN_SYS_RUNNING_IMG, &running_idx);
    if (res < 0) {
        printf("sys get running idx failed! (res = %d)\r\n", res);
        ret = -1;
        goto Exit;
    }

    res = http_host_get(DOWNLOAD_URL, &host, &url);
    if (res < 0) {
        printf("http get host failed! (res = %d)\r\n", res);
        ret = -2;
        goto Exit;
    }
    http_socketid = http_socket_init(host, PORT);
    if (http_socketid < 0) {
        printf("Init socket failed! (sid = %d)\r\n", http_socketid);
        ret = -3;
        goto Exit;
    }

    res = http_req_image(http_socketid, url, host, PORT, bin_name);
    if (0 == res) {
        res = http_rsp_image(http_socketid, running_idx);
        if (res < 0) {
            printf("Get Firmware Reponse failed! (res = %d)\r\n", res);
            ret = -4;
            goto Exit;
        }
    } else {
        printf("Request Firmware failed! (res = %d)\r\n", res);
        ret = -5;
        goto Exit;
    }

    /* Set image status */
    res = mbl_sys_img_flag_set(running_idx, (IMG_FLAG_IA_MASK | IMG_FLAG_NEWER_MASK), (IMG_FLAG_IA_OK | IMG_FLAG_OLDER));
    res |= mbl_sys_img_flag_set(!running_idx, (IMG_FLAG_IA_MASK | IMG_FLAG_VERIFY_MASK | IMG_FLAG_NEWER_MASK), 0);
    res |= mbl_sys_img_flag_set(!running_idx, IMG_FLAG_NEWER_MASK, IMG_FLAG_NEWER);
    if (res != 0) {
        printf("Set sys image status failed! (res = %d)\r\n", res);
        ret = -6;
    }
    printf("Download new image succesffuly. Please reboot now.\r\n");
Exit:
    if (http_socketid != -1)
        close(http_socketid);
    if (ret < 0)
        printf("ota demo return %d\r\n", ret);
    sys_mfree(param);

    sys_task_delete(NULL);
}

/*!
    \brief      ota demo
    \param[in]  bin_name: pointer to the image bin name
    \param[out] none
    \retval     function status
      \arg        -1: malloc space fail
      \arg        -2: create ota demo task failed
      \arg        0: run succee
*/
int32_t ota_demo(char *bin_name)
{
    /* Test with Python3 HTTP Server: (copy the ota bin file to the following cmd running directory)
       # python -m http.server 80 --bind 192.168.110.24
    */
    void *handle;
    char *param = sys_malloc(strlen(bin_name) + 1);
    if (param == NULL) {
        return -1;
    }
    memcpy(param, bin_name, (strlen(bin_name) + 1));
    handle = sys_task_create(NULL, (const uint8_t *)"ota_demo", NULL,
                    512, 4, TASK_PRIO_APP_BASE,
                    (task_func_t)ota_demo_task, param);
    if (handle == NULL) {
        printf("Create ota demo task failed.\r\n");
        sys_mfree(param);
        return -2;
    }
    return 0;
}
